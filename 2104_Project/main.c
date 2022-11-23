/* Includes */
#include <stdio.h>
#include "pico/stdlib.h"

/*
 * Instructions to be passed as first parameter for send functions (comms)
 * Used by M5 to differentiate the data received
 */
#define M5_BARCODE 97
#define M5_DISTANCE 98
#define M5_HUMP 99
#define M5_MAP 100

/* Global variables */
// Coordinates received from M5
extern uint8_t coords[2];
extern uint8_t x, y;

/* Main program */
int main()
{
    stdio_init_all(); // Enable UART so we can print status output
    return 0;
}