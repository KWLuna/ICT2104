/* Includes */
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "comms.h"

/* Configure I2C pins */
void i2c_start()
{
    i2c_init(i2c0, I2C_BAUD_RATE); // Use I2C0 instance with baud rate of 100kHz

    // Configure GP4 and GP5 for I2C comms (with pull-up resistors)
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C); // I2C0 SDA
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C); // I2C0 SCL
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
}

/* Send uint8_t data to M5 */
void i2c_send_barcode(uint8_t inst, char data)
{
    uint8_t finalData[2];
    finalData[0] = inst;
    finalData[1] = (uint8_t)data;

    if (i2c_write_blocking(i2c0, M5_SLAVE, finalData, 2, false) != PICO_ERROR_GENERIC)
        printf("Instruction %d and data %d successfully sent\n", (inst, data));
    else
        printf("UINT8_T SEND ERROR\n");
}

/* Send float data (distance, etc.) to M5 */
void i2c_send_float(uint8_t inst, float data)
{
    uint8_t finalData[3];
    finalData[0] = inst;

    uint8_t left = (uint8_t)data;                   // Before d.p.
    uint8_t right = (uint8_t)((data - left) * 100); // After d.p., cast to prevent errors if >2.
    finalData[1] = left;
    finalData[2] = right;

    if (i2c_write_blocking(i2c0, M5_SLAVE, finalData, 3, false) != PICO_ERROR_GENERIC)
        printf("Instruction %d and data %.2f successfully sent\n", (inst, data));
    else
        printf("FLOAT SEND ERROR\n");
}

/* Read x and y coordinates from M5 */
void i2c_read_data()
{
    i2c_read_blocking(i2c0, M5_SLAVE, coords, 2, false); // Request 2 bytes from slave 8 (M5)
    x = coords[0];                                       // Retrieve x-coordinate
    y = coords[1];                                       // Retrieve y-coordinate

    dataAvailable = true;
    printf("x: %d\n", x);
    printf("y: %d\n", y);
}