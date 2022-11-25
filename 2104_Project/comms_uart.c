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

/* Send data to M5 for display purposes */
void uart_send_data(uint8_t inst, uint8_t data)
{
    uint8_t finalData[2] = {inst, data};
    uart_write_blocking(uart1, finalData, 2);
}

/* Read x and y coordinates from M5 */
void uart_read_data()
{
    if (uart_is_readable(uart1)) // Check if any data in serial buffer
    {
        uart_read_blocking(uart1, coords, 2); // Read 2 bytes from RX FIFO
        x = coords[0];                        // Retrieve x-coordinate
        y = coords[1];                        // Retrieve y-coordinate

        printf("x: %d\n", x);
        printf("y: %d\n", y);
    }
}