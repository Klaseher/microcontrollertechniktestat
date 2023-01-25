#ifndef TESTAT_2
#define TESTAT_2

/*
    a
    _
  f|g| b
    -
  e| | c   
    -    . 
    d    h

7 6 5 4 3 2 1 0
h g f e d c b a
*/

// Zahlen       //hgfedcba
#define NUL     0b00111111
#define EINS    0b00000110
#define ZWEI    0b01011011
#define DREI    0b01001111
#define VIER    0b01100110
#define FUENF   0b01101101
#define SECHS   0b01111101
#define SIEBEN  0b00000111
#define ACHT    0b01111111
#define NEUN    0b01101111
#define PUNKT   0b10000000

const byte nummer[] = {NUL, EINS, ZWEI, DREI, VIER, FUENF, SECHS, SIEBEN, ACHT, NEUN};

// Stelle der 7-Segment Anzeige
#define STELLE1 0xF1
#define STELLE2 0xF2
#define STELLE3 0xF4
#define STELLE4 0xF8

const byte stellen[] = {STELLE1, STELLE2, STELLE3, STELLE4};

//Bitmanipulation
#define BIT(b)      (1<<b)    // Setzt das Bit b
#define SET(x,y)    (x |= y)  // Setzt ein Bit y eines Ports x
#define CLEAR(x,y)  (x &=~ y) // Cleart das Bit y eines Ports x

// Modi
#define UHRMODUS    0b0
#define STELLMODUS  0b1

/* 7-Segment-Ports */
#define DATA    BIT(0)  // Datenregister auf PortB
#define CLOCK   BIT(7)  // Clockregister auf PortD
#define LATCH   BIT(4)  // Latchregister auf PortD

// Tasten-Ports
#define TASTE3    BIT(3)  // Bit der dritten Taste beim PCICR
#define PRELLUNG  30      // muß hier 20 mal konstant sein 

// Timer-Ports
#define PRE256  BIT(2)  // wird genutzt um den Prescaler des Timer1 auf 256 zu setzen
#define INTOC1  BIT(1)  // wird genutzt um den Output-Compare-Interrupt zu aktivieren
#define PCIE1   BIT(1)  // wird genutzt um den Pin-Change-Interrupt zu aktivieren

//Funktionen
void WriteNumberToSegment(byte Segment, byte Value);
void show_clock();
void AddSecond();
int AddMinute();
int AddHour();


#endif
