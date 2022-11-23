/* Includes */
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

/* I2C settings */
#define M5_SLAVE 8           // Slave address of M5StickC Plus
#define BAUD_RATE 100 * 1000 // 100KHz

/* Global variables */
uint8_t coords[2];
uint8_t x;
uint8_t y;

/* Configure I2C pins */
void i2c_start()
{
    i2c_init(i2c0, BAUD_RATE); // Use I2C0 instance with baud rate of 100kHz

    // Configure GP4 and GP5 for I2C comms (with pull-up resistors)
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C); // I2C0 SDA
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C); // I2C0 SCL
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
}

/* Send data to M5 for display purposes */
void i2c_send_data(uint8_t inst, uint8_t data)
{
    if (i2c_write_blocking(i2c0, M5_SLAVE, &inst, 1, true) != PICO_ERROR_GENERIC)
    {
        i2c_write_blocking(i2c0, M5_SLAVE, &data, 1, false);
        printf("Instruction %d and data %d successfully sent\n", (inst, data));
    }
}

/* Read x and y coordinates from M5 */
void i2c_read_data()
{
    i2c_read_blocking(i2c0, M5_SLAVE, coords, 2, false); // Request 2 bytes from slave 8 (M5)
    x = coords[0];                                       // Retrieve x-coordinate
    y = coords[1];                                       // Retrieve y-coordinate

    printf("x: %d\n", x);
    printf("y: %d\n", y);
}