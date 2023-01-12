#include "testat_2.h"
#include <avr/interrupt.h>
#include <avr/io.h>

int8_t hour    = 19;
int8_t minute  = 57;
int8_t second  = 0;
bool point = true;
volatile int i;


void init()
{
  /* Set DIO pins to outputs */
  pinMode(ANZEIGEPORT, OUTPUT);
  pinMode(TAKTPORT,    OUTPUT);
  pinMode(DATENPORT,   OUTPUT);

  cli();
  
  TCCR1A = 0b00000000; 
  TCCR1B = 0b00000000; 

  // Prescaler auf 256 --> Halbe Sekunden Timer --> 16MHz Takt --> 62,5 ns pro takt * 256 = 16ms pro takt --> 16 * 315250 = 0,5s  
  TCCR1A |= 0b10000000; 
  TCCR1B |= 0b00000100;
  OCR1A   = 31250;
  TCNT1  = 0;
  
  TIMSK1 |= 0b00000010; // enable Comparematch auf OCR1A
  
  sei();
} 

/* Main program */
int main()
{ 
  init();

  while(1){
    show_clock();
  }    
}

/* Write a decimal number between 0 and 9 to one of the 4 digits of the display */
void WriteNumberToSegment(byte Segment, byte Value)
{
  digitalWrite(ANZEIGEPORT, LOW);
  if(Segment == 1 && point)
    shiftOut(DATENPORT, TAKTPORT, MSBFIRST, ~(numbers[Value] | PUNKT));
  else
    shiftOut(DATENPORT, TAKTPORT, MSBFIRST, ~numbers[Value]);
    
  shiftOut(DATENPORT, TAKTPORT, MSBFIRST, stellen[Segment]);
  digitalWrite(ANZEIGEPORT, HIGH);
}

void show_clock()
{
  WriteNumberToSegment(0, hour   / 10);
  WriteNumberToSegment(1, hour   % 10);
  WriteNumberToSegment(2, minute / 10);
  WriteNumberToSegment(3, minute % 10);
  //WriteNumberToSegment(2, second / 10);
  //WriteNumberToSegment(3, second % 10);
}

ISR(TIMER1_COMPA_vect)
{
  if(point)
    AddSecond();
    
  point = !point;
  TCNT1 = 0;
}

//Fügt der Uhrzeit eine Sekunde hinzu und erhöht nach 60 Sekunden die Minuten um 1
void AddSecond()
{
  second < 59 ? second++ : second = AddMinute();  
}

//Fügt der Uhrzeit eine Minute hinzu und erhöht nach 60 Minute die Stunden um 1
int AddMinute()
{
  minute < 59 ? minute++ : minute = AddHour();
  return 0;
}
    
//Fügt der Uhrzeit eine Stunde hinzu und setzt diese nach 24h zurück auf 0
int AddHour()
{
  hour < 23 ? hour++ : hour = 0; 
  return 0;
}
