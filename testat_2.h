#ifndef TESTAT_2
#define TESTAT_2

/*
    a
    _
  f|g| b
    -
  e| | c
    -
    d

7 6 5 4 3 2 1 0
h g f e d c b a
*/

// Zahlen
                //hgfedcba
#define EINS    0b00000110
#define ZWEI    0b01011011
#define DREI    0b01001111
#define VIER    0b01100110
#define FUENF   0b01101101
#define SECHS   0b01111101
#define SIEBEN  0b00000111
#define ACHT    0b01111111
#define NEUN    0b01101111

// Stelle der 7-Segment Anzeige
#define STELLE1 0b00000000
#define STELLE2 0b00000000
#define STELLE3
#define STELLE4

// Modi
#define TIMERMODUS  0
#define STELLMODUS  1
#define STUNDEN     10
#define MINUTEN     11

// 7-Segment-Ports
#define DATENPORT   PB0
#define TAKTPORT    PD7
#define ANZEIGEPORT PD4

// Tasten-Ports
#define TASTE1  PC1
#define TASTE2  PC2
#define TASTE3  PC3

#endif