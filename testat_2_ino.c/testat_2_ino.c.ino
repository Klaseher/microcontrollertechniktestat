#include "testat_2.h"

int8_t hour    = 15;
int8_t minute  = 15;
int8_t second  = 0;
volatile int i;

void init()
{
  /* Set DIO pins to outputs */
  pinMode(ANZEIGEPORT, OUTPUT);
  pinMode(TAKTPORT,    OUTPUT);
  pinMode(DATENPORT,   OUTPUT);
} 
 
/* Write a decimal number between 0 and 9 to one of the 4 digits of the display */
void WriteNumberToSegment(byte Segment, byte Value)
{
  digitalWrite(ANZEIGEPORT, LOW);
  if(Segment == 1)
    shiftOut(DATENPORT, TAKTPORT, MSBFIRST, ~numbers[Value] | PUNKT);    
  else
    shiftOut(DATENPORT, TAKTPORT, MSBFIRST, ~numbers[Value]);
    
  shiftOut(DATENPORT, TAKTPORT, MSBFIRST, stellen[Segment]);
  digitalWrite(ANZEIGEPORT, HIGH);
}

void show_clock()
{
  WriteNumberToSegment(0, hour   / 10);
  for (volatile unsigned long i = 0; i < 10000; i++);;
  WriteNumberToSegment(1, hour   % 10);
  for (volatile unsigned long i = 0; i < 10000; i++);;
  WriteNumberToSegment(2, minute / 10);
  for (volatile unsigned long i = 0; i < 10000; i++);;
  WriteNumberToSegment(3, minute % 10);
  for (volatile unsigned long i = 0; i < 10000; i++);;
}

/* Main program */
int main()
{ 
  init();
  /* Update the display with the current counter value */
  while(1){
    show_clock();
    
    hour   < 23 ? hour++   : hour = 0;
    minute < 59 ? minute++ : minute = 0;
    
   // for (volatile unsigned long i = 0; i < 100000; i++);;
  }    
}
