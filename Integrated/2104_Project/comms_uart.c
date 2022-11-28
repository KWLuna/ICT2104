/* Includes */
#include <stdio.h>
#include "pico/stdlib.h"
#include "comms.h"

/* Configure UART pins */
void uart_start()
{
    uart_init(uart1, UART_BAUD_RATE);

    // Configure GP4 and GP5 for UART
    gpio_set_function(4, GPIO_FUNC_UART); // UART TX
    gpio_set_function(5, GPIO_FUNC_UART); // UART RX
}

/* Send uint8_t data to M5 */
void uart_send_barcode(uint8_t inst, char data)
{
    uint8_t finalData[2];
    finalData[0] = inst;
    finalData[1] = (uint8_t)data;
    uart_write_blocking(uart1, finalData, 2);
}

/* Send float data (distance, etc.) to M5 */
void uart_send_float(uint8_t inst, float data)
{
    uint8_t finalData[3];
    finalData[0] = inst;

    uint8_t left = (uint8_t)data;                   // Before d.p.
    uint8_t right = (uint8_t)((data - left) * 100); // After d.p., cast to prevent errors if >2.
    finalData[1] = left;
    finalData[2] = right;
    uart_write_blocking(uart1, finalData, 3);
}

/* Send map data */
void uart_send_map(uint8_t mapArr[9][11])
{
    uart_putc_raw(uart1, (char)M5_MAP); // Instruction for M5
    for (int i = 0; i < 9; i++)
        uart_write_blocking(uart1, mapArr[i], 11);
}

/* Read x and y coordinates from M5 */
void uart_read_data()
{
    if (uart_is_readable(uart1)) // Check if any data in serial buffer
    {
        uart_read_blocking(uart1, coords, 2); // Read 2 bytes from RX FIFO
        x = coords[0];                        // Retrieve x-coordinate
        y = coords[1];                        // Retrieve y-coordinate

        dataAvailable = true;
        printf("x: %d\n", x);
        printf("y: %d\n", y);
    }
}