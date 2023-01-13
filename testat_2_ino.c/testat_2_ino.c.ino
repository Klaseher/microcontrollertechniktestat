#include "testat_2.h"
#include <avr/interrupt.h>

// Zeitwerte
int8_t hour = 10;   // Stundenwert
int8_t minute = 34;  // Minutenwert
int8_t second = 5; // Sekundenwert

int8_t modus = UHRMODUS; // Variable, die den aktuellen Modus der Uhr bestimmt
bool point = true;  // bestimmt, ob der Punkt an der 2. Stelle angezeigt werden soll
bool entry = true;

void init()
{
  /* Set DIO pins to outputs */
  SET(DDRB, DATA);             /* alle drei Bit Output */
  SET(DDRD, (CLOCK | LATCH));
  CLEAR(PORTD, (CLOCK | LATCH)); /* SHIFT und LATCH Low */
  
  pinMode(TASTE1, INPUT);
  pinMode(TASTE2, INPUT);
  pinMode(TASTE3, INPUT);

  // Interrupts zum Einstellen deaktivieren
  cli();

  // Resetten der Timerregister
  TCCR1A = 0b00000000;
  TCCR1B = 0b00000000;

  // Prescaler auf 256
  TCCR1B |= 0b00000100;

  // Halbe Sekunden Timer --> 16MHz Takt --> 62,5 ns pro takt * 256 = 16ms --> 500.000ms / 16ms = 31250 --> OCR1A = 31250
  OCR1A = 31250;  // Wert, mit dem TCNT1 verglichen werden soll
  TCNT1 = 0;      // Wert, der vom Timer hochgezählt wird

  // Aktiviert den Interrupt für OCIE1A
  TIMSK1 |= 0b00000010;
  
  PCMSK1 |= 0b00001000;
  PCICR  |= 0b00000010;

  // Interrupts aktivieren
  sei();
}

#define N 20                            /* muß hier 20 mal konstant sein */
/* benötigte Variablen zur Tastenentprellung */
unsigned char Zustand[3];       /* entprellter Zustand der Taste */
unsigned char Flanke[3]={0,0,0};      /* 1 - wenn Flanke erkannt */


/* Routine zum Entprellen, muß zyklisch aufgerufen werden */
void Entprellen (void)
{
  unsigned char T_neu;                /* neuer Zustand der Taste */
  static unsigned char T_alt = 0;     /* alter Zustand der Taste */
  static int Counter;

  for(int i = 0; i < 3; i++){
    T_neu = liesTaste(i+1);                /* neuen Zustand der Taste abfragen */
    if (T_neu != T_alt)
        Counter = 0;                    /* Taste prellt noch */
    else {
        Counter++;                      /* erneut unverändert */
        if (Counter == N) {             /* N-mal konstant -> Prellen zu Ende */
            Zustand[i] = T_neu;       /* merke Tastenzustand */
            Flanke[i] = 1;            /* registriere Aenderung */
        } 
        if (Counter > N+1)
            Counter = N+1;              /* Zähler begrenzen */
    }
  }
  T_alt = T_neu;                       /* aktuellen Zustand Taste merken */
}

/* liest den Zustand der konkreten Taste aus und liefert ihn
   als binären Wert zurück (true - gedrückt) */
unsigned char liesTaste(int taste) 
{
  if ((PINC & (1 << taste)) != 0)
    return 1;
  else
    return 0;
}

/* Main program */
int main()
{
  init();
  
  while (1)
  {
    if (modus == STELLMODUS)
    {
      if (entry)
      {
        cli();
        entry = false;
      }
      
      Entprellen();
      
      if ((Flanke[1] == 1) && (Zustand[1] == 1))
      {
        Flanke[1] = 0; 
        AddHour();
      }
      else if ((Flanke[2] == 1) && (Zustand[2] == 1))
      {
        Flanke[2] = 0; 
        entry = true;
        modus = UHRMODUS;
        sei();
      }
    }
    
    show_clock();
  }
}


/* Schreibt einen Dezimalwert zwischen 0-9 (Value) auf einer der 4 Stellen (Segment) der 7-Segment-Anzeige */
void WriteNumberToSegment(byte segment, byte value)
{
  // Setzt den Punkt, falls das Boolean "point" true ist
  if (segment == 1 && point)
    value   = ~(numbers[value] | PUNKT);
  else
    value   = ~numbers[value];
    
  segment = stellen[segment];
  
  // Latch Low setzen um am Ende dieser Funktion die Anzeige aktualisieren zu können
  CLEAR(PORTD, LATCH); 

  // Ausgabe der anzuzeigenden Ziffer  
  for (int i = 0; i < 8; i++) {
    if (value & BIT(7)) {
      SET(PORTB, DATA);
    } 
    else {
      CLEAR(PORTB, DATA);
    }
    value <<= 1;

    /* Bittakt bei jedem Bit */
    SET(PORTD, CLOCK);
    CLEAR(PORTD, CLOCK);
  }

  /* Ausgabe Stellenmuster auf DATA */
  for (int i = 0; i < 8; i++) {
    if (segment & BIT(7)) {
      SET(PORTB, DATA);
    } else {
      CLEAR(PORTB, DATA);
    }
    segment <<= 1;

    SET(PORTD, CLOCK);
    CLEAR(PORTD, CLOCK);
  }

  // Anzeige aktualisieren
  SET(PORTD, LATCH);
}

/* Aktualisiert die aktuell angezeigte Uhrzeit an die entsprechenden Stellen */
void show_clock()
{
  WriteNumberToSegment(0, hour / 10);
  WriteNumberToSegment(1, hour % 10);
  WriteNumberToSegment(2, minute / 10);
  WriteNumberToSegment(3, minute % 10);
  // WriteNumberToSegment(2, second / 10);
  // WriteNumberToSegment(3, second % 10);
}


/* ISR für Timer1 Compare-Match-Register A */
// Toggled jede halbe Sekunde den Punkt und setzt jede Sekunde den second-Wert nach oben
ISR(TIMER1_COMPA_vect)
{
  // Counter resetten
  TCNT1 = 0;
  
  // ISR triggert jede halbe Sekunde --> alle 2 Interrupt Sekunde neu setzen
  if (point)
    AddSecond();

  // Punkt togglen
  point = !point;
}

/* Sperrt Interrupts und aktiviert Stellmodus */
ISR(PCINT1_vect)
{
  modus = STELLMODUS;
  point = true;
}


/* Funktionen zum Fortschreiten der Uhrzeit */
// Fügt der Uhrzeit eine Sekunde hinzu und erhöht nach 60 Sekunden die Minuten um 1
void AddSecond()
{
  second < 59 ? second++ : second = AddMinute();
}

// Fügt der Uhrzeit eine Minute hinzu und erhöht nach 60 Minute die Stunden um 1
int AddMinute()
{
  minute < 59 ? minute++ : minute = AddHour();
  return 0;
}

// Fügt der Uhrzeit eine Stunde hinzu und setzt diese nach 24h zurück auf 0
int AddHour()
{
  hour < 23 ? hour++ : hour = 0;
  return 0;
}
