#include "testat_2.h"
#include <avr/interrupt.h>
#include <avr/io.h>

int8_t hour = 20;   // Stundenwert
int8_t minute = 2;  // Minutenwert
int8_t second = 10; // Sekundenwert
bool point = true;  // bestimmt, ob der Punkt an der 2. Stelle angezeigt werden soll

void init()
{
  /* Set DIO pins to outputs */
  pinMode(ANZEIGEPORT, OUTPUT);
  pinMode(TAKTPORT, OUTPUT);
  pinMode(DATENPORT, OUTPUT);

  // Interrupts zum Einstellen deaktivieren
  cli();

  // Resetten der Timerregister
  TCCR1A = 0b00000000;
  TCCR1B = 0b00000000;

  // Prescaler auf 256
  TCCR1B |= 0b00000100;

  // Halbe Sekunden Timer --> 16MHz Takt --> 62,5 ns pro takt * 256 = 16ms * 31250 = 0,5s --> OCR1A = 31250
  OCR1A = 31250;  // Wert, mit dem TCNT1 verglichen werden soll
  TCNT1 = 0;      // Wert, der vom Timer hochgezählt wird

  // Aktiviert den Interrupt für OCR1A
  TIMSK1 |= 0b00000010;

  // Interrupts aktivieren
  sei();
}

/* Main program */
int main()
{
  init();

  while (1)
  {
    show_clock();
  }
}

/* Schreibt einen Dezimalwert zwischen 0-9 (Value) auf einer der 4 Stellen (Segment) der 7-Segment-Anzeige */
void WriteNumberToSegment(byte Segment, byte Value)
{
  // Anzeigeport Low setzen um am Ende dieser Funktion die Anzeige aktualisieren zu können
  digitalWrite(ANZEIGEPORT, LOW);

  // Setzt den Punkt, falls das Boolean "point" true ist
  if (Segment == 1 && point)
    shiftOut(DATENPORT, TAKTPORT, MSBFIRST, ~(numbers[Value] | PUNKT));
  else
    shiftOut(DATENPORT, TAKTPORT, MSBFIRST, ~numbers[Value]);

  // Bestimmt die Stelle, bei der das Value angezeigt werden soll
  shiftOut(DATENPORT, TAKTPORT, MSBFIRST, stellen[Segment]);

  // Anzeige aktualisieren
  digitalWrite(ANZEIGEPORT, HIGH);
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
  // ISR triggert jede halbe Sekunde --> alle 2 Interrupt Sekunde neu setzen
  if (point)
    AddSecond();

  // Punkt togglen
  point = !point;

  // Counter resetten
  TCNT1 = 0;
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
