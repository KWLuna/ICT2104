/*
Code for blackbox testing of communications sub-module
*/


/* Necessary includes */
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "comms.h"

/* Preprocessor directives */
#define UART_SEL 1 // Decides whether to test UART or I2C

/* Single test functions */
// BB2 - Send map data
void send_map()
{
    uint8_t map[9][11] = {
        {1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1},
        {1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1},
        {1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1},
        {1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1},
        {1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1},
        {1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1},
        {1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1},
        {1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1},
        {1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1}
    }

#if defined(UART_SEL)
    uart_send_map(map);
#else
    i2c_send_map(map);
#endif
}

// BB3 - Send hump data
void send_hump()
{
    float height = 123.45;

#if defined(UART_SEL)
    uart_send_float(M5_HUMP, height);
#else
    i2c_send_float(M5_HUMP, height);
#endif
}

// BB5 - Send distance data
void send_distance()
{
    float distance = 543.21;

#if defined(UART_SEL)
    uart_send_float(M5_DISTANCE, distance);
#else
    i2c_send_float(M5_DISTANCE, distance);
#endif
}

// BB9 - Send barcode data
void send_barcode()
{
    char barcode = 'a';

#if defined(UART_SEL)
    uart_send_barcode(M5_BARCODE, barcode);
#else
    i2c_send_barcode(M5_BARCODE, barcode);
#endif
}

/* Multi-test functions */
// BB4 - Send hump + map data
void send_hump_map()
{
    send_map();
    send_hump();
}

// BB6 - Send distance + map data
void send_distance_map()
{
    send_distance();
    send_map();
}

// BB7 - Send distance + hump data
void send_distance_hump()
{
    send_distance();
    send_hump();
}

// BB8 - Send distance + hump + map data
void send_distance_hump_map()
{
    send_distance();
    send_hump();
    send_map();
}

// BB10 - Send barcode + map data
void send_barcode_map()
{
    send_barcode();
    send_map();
}

// BB11 - Send barcode + hump data
void send_barcode_hump()
{
    send_barcode();
    send_hump();
}

// BB12 - Send barcode + hump + map data
void send_barcode_hump_map()
{
    send_barcode();
    send_hump();
    send_map();
}

// BB13 - Send barcode + distance data
void send_barcode_distance()
{
    send_barcode();
    send_distance();
}

// BB14 - Send barcode + distance + map data
void send_barcode_distance_map()
{
    send_barcode();
    send_distance();
    send_hump();
}

// BB15 - Send barcode + distance + hump data
void send_barcode_distance_hump()
{
    send_barcode();
    send_distance();
    send_hump();
}

// BB16 - Send barcode + distance + hump + map data
void send_barcode_distance_hump_map()
{
    send_barcode();
    send_distance();
    send_hump();
    send_map();
}

int main()
{
    stdio_init_all();
#if defined(UART_SEL)
    uart_start();
#else
    i2c_start();
#endif

    while (1)
    {
        // send_map();
        // send_hump();
        // send_distance();
        // send_barcode();
        // send_hump_map();
        // send_distance_map();
        // send_distance_hump();
        // send_distance_hump_map();
        // send_barcode_map();
        // send_barcode_hump();
        // send_barcode_hump_map();
        // send_barcode_distance();
        // send_barcode_distance_map();
        // send_barcode_distance_hump();
        send_barcode_distance_hump_map();
    }

    return 0;
}