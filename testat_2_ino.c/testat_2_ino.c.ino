#include "testat_2.h"

void init()
{
  /* Set DIO pins to outputs */
  pinMode(ANZEIGEPORT_D,OUTPUT);
  pinMode(TAKTPORT_D,OUTPUT);
  pinMode(DATENPORT_B,OUTPUT);
} 
 
/* Write a decimal number between 0 and 9 to one of the 4 digits of the display */
void WriteNumberToSegment(byte Segment, byte Value)
{
  digitalWrite(ANZEIGEPORT_D,LOW);
  shiftOut(DATENPORT_B, TAKTPORT_D, MSBFIRST, ~numbers[Value]);
  shiftOut(DATENPORT_B, TAKTPORT_D, MSBFIRST, stellen[Segment]);
  digitalWrite(ANZEIGEPORT_D,HIGH);
}

/* Main program */
int main()
{ 
  init();
  /* Update the display with the current counter value */
  while(1){
    WriteNumberToSegment(0 , 0);
    WriteNumberToSegment(1 , 1);
    WriteNumberToSegment(2 , 2);
    WriteNumberToSegment(3 , 10);
    
  }
  
}
