#include "testat_2.h"
#include <avr/interrupt.h>

// Zeitwerte
int8_t hour = 10;   // Stundenwert
int8_t minute = 34;  // Minutenwert
int8_t second = 5; // Sekundenwert

int8_t modus = UHRMODUS;  // Variable, die den aktuellen Modus der Uhr bestimmt
int8_t point = 1;         // bestimmt, ob der Punkt an der 2. Stelle angezeigt werden soll
int8_t entry = 1;

void init()
{
  /* Set DIO pins to outputs */
  SET(DDRB, DATA);             /* alle drei Bit Output */
  SET(DDRD, (CLOCK | LATCH));
  CLEAR(PORTD, (CLOCK | LATCH)); /* SHIFT und LATCH Low */
  
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

#define N 20                          /* muß hier 20 mal konstant sein */
/* benötigte Variablen zur Tastenentprellung */
uint8_t Zustand[3] = {0,0,0};         /* entprellter Zustand der Taste */
uint8_t Flanke[3]  = {0,0,0};         /* 1 - wenn Flanke erkannt */
uint8_t static Counter[3] = {0,0,0};  


/* Routine zum Entprellen, muß zyklisch aufgerufen werden */
void Entprellen(int8_t t)
{
  uint8_t T_neu;                /* neuer Zustand der Taste */
  uint8_t static T_alt[3]   = {0,0,0};  /* alter Zustand der Taste */
  
  T_neu = liesTaste(t+1);       /* neuen Zustand der Taste abfragen */
  if (T_neu != T_alt[t])
    Counter[t] = 0;             /* Taste prellt noch */
  else {
    Counter[t]++;               /* erneut unverändert */
    if (Counter[t] == N) {      /* N-mal konstant -> Prellen zu Ende */
        Flanke[t] = 1;          /* registriere Aenderung */
    } 
    if (Counter[t] > N+1){
        Counter[t] = N+1;       /* Zähler begrenzen */
    }
    T_alt[t] = T_neu;           /* aktuellen Zustand Taste merken */
  }
}

/* liest den Zustand der konkreten Taste aus und liefert ihn
   als binären Wert zurück (true - gedrückt) */
unsigned char liesTaste(int t) 
{
  if ((PINC & (1 << t)) != 0)
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
        PCICR  &= 0;
        TIMSK1 &= 0;
        sei();
        entry = 0;
      }
      else{
        Entprellen(0);
        if(Flanke[0] == 1 && Zustand[0] == 0){
            AddHour();
            Flanke[0] = 0;
        }
        
        Entprellen(1);
        if(Flanke[1] == 1 && Zustand[1] == 0){
            AddMinute();
            Flanke[1] = 0;
        }    
  
        Entprellen(2);
        if(Flanke[2] == 1 && Zustand[2] == 0){
          entry = 1;
          Flanke[2] = 0;
          
          for(volatile unsigned long i = 0; i < 200000; i++);;
          cli();
          PCICR  |= 0x02;
          TIMSK1 |= 0x02;
          sei();
        }
      }
    }
    
    show_clock();
  }
}


/* Schreibt einen Dezimalwert zwischen 0-9 (Value) auf einer der 4 Stellen (Segment) der 7-Segment-Anzeige */
void WriteNumberToSegment(byte segment, byte value)
{
  // Setzt den Punkt, falls das point 1 ist
  if (segment == 1 && point)
    value   = ~(nummer[value] | PUNKT);
  else
    value   = ~nummer[value];
    
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
  if (modus == UHRMODUS){
    modus = STELLMODUS;
    point = 1;
  }
  else{
    modus = UHRMODUS;
  }
    
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
