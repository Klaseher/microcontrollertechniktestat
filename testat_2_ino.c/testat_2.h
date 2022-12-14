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

// Zahlen
                //hgfedcba
#define NULL    0b00111111
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

const byte numbers[] = {NULL, EINS, ZWEI, DREI, VIER, FUENF, SECHS, SIEBEN, ACHT, NEUN};

// Stelle der 7-Segment Anzeige
#define STELLE1 0xF1
#define STELLE2 0xF2
#define STELLE3 0xF4
#define STELLE4 0xF8

const byte stellen[] = {STELLE1, STELLE2, STELLE3, STELLE4};

// Modi
#define TIMERMODUS  0b0
#define STELLMODUS  0b1

/* 7-Segment-Ports */
#define DATENPORT_B   8
#define TAKTPORT_D    7
#define ANZEIGEPORT_D 4

// Tasten-Ports
#define TASTE1  PC1
#define TASTE2  PC2
#define TASTE3  PC3

// Timer-Ports
#

#endif