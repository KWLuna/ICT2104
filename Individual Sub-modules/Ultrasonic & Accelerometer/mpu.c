#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "mpu.h"
#include "comms.h"

#define Pi 3.14159265
#define RAD_TO_DEG 57.29577951308232
#define HYPOTENUSE 20
#define minVal 265
#define maxVal 402
#define detection_threshold 0.7

// globals
float offsetHeight = 0.0;
float highestHeight = 0.0;
bool bumpDataReady = false;

float pitch = 0.0;
float pitchAcc;
float P_CompCoeff = 0.98;

// Bus address for the MPU
static int addr = 0x68;

// Defining i2c port
i2c_inst_t *i2cAccel = i2c1;

static void mpu6050_reset()
{
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    uint8_t buf[] = {0x6B, 0x00};
    i2c_write_blocking(i2cAccel, addr, buf, 2, false);
}

static void mpu6050_read_rawA(int16_t accel[3])
{
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];

    // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = 0x3B;
    i2c_write_blocking(i2cAccel, addr, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2cAccel, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++)
    {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }
}
static void mpu6050_read_rawG(int16_t gyro[3])
{
    uint8_t buffer[6];
    // Now gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    uint8_t val = 0x43;
    i2c_write_blocking(i2cAccel, addr, &val, 1, true);
    i2c_read_blocking(i2cAccel, addr, buffer, 6, false); // False - finished with bus

    for (int i = 0; i < 3; i++)
    {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
        ;
    }
}
int calcAccAngle(int16_t accVal, int16_t inMin, int16_t inMax, int16_t outMin, int16_t outMax)
{
    int16_t aAngle = (accVal - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    return aAngle;
}
float calcTiltAngleX(int16_t aAngleY, int16_t aAngleZ)
{
    // calculate tilt angle using the complementary Y & Z axis
    float tAngleX = (atan2(-aAngleY, -aAngleZ) + Pi);
    return tAngleX;
}
float calcHeight(float Angle)
{
    // Calculate height using trigonometry
    return (sin(Angle) * HYPOTENUSE);
}
void mpu6050_calibration()
{
    int16_t acc_calibration[3];
    int count = 0;
    // float errorOffset = 0.0;

    int16_t cali_accAngleX = 0, cali_accAngleY = 0, cali_accAngleZ = 0;
    float cali_tiltX = 0.0;

    float cali_Height = 0.0;

    // Measure X angle 300 times to calculate the mean offset
    while (count < 300)
    {
        mpu6050_read_rawA(acc_calibration);
        cali_accAngleX = calcAccAngle(acc_calibration[0], minVal, maxVal, -90, 90);
        cali_accAngleY = calcAccAngle(acc_calibration[1], minVal, maxVal, -90, 90);
        cali_accAngleZ = calcAccAngle(acc_calibration[2], minVal, maxVal, -90, 90);

        cali_tiltX = (atan2(-cali_accAngleY, -cali_accAngleZ) + Pi);

        cali_Height += sin(cali_tiltX) * HYPOTENUSE;
        printf("Calibrating...%d\n", count);
        printf("caliHeight %f", cali_Height);
        count++;
        sleep_ms(30);
    }

    offsetHeight = cali_Height / 300.0;
    // return offsetHeight;
}
float getHeight()
{
    int16_t acceleration[3], gyro[3];
    int16_t accAngleX = 0, accAngleY = 0, accAngleZ = 0;
    int16_t count = 0;

    float tiltX = 0;
    float elevatedHeight = 0.0;
    float highestHeight = 0.0;

    while (count < 20)
    {
        mpu6050_read_rawA(acceleration);
        accAngleX = calcAccAngle(acceleration[0], minVal, maxVal, -90, 90);
        accAngleY = calcAccAngle(acceleration[1], minVal, maxVal, -90, 90);
        accAngleZ = calcAccAngle(acceleration[2], minVal, maxVal, -90, 90);

        tiltX = calcTiltAngleX(accAngleY, accAngleZ);
        // tiltY = RAD_TO_DEG * (atan2(-accAngleX, -accAngleZ) + Pi);
        // tiltZ = RAD_TO_DEG * (atan2(-accAngleY, -accAngleX) + Pi);

        elevatedHeight += calcHeight(tiltX);

        count++;
        // sleep_ms(1);
    }

    elevatedHeight = elevatedHeight / 20.0;
    // printf("Offset. Y = %f\n", offsetY);
    // printf("Offset. Height = %f\n", offsetHeight);
    // printf("Tilt Angle. X = %f\n", tiltX);
    // printf("Height: %.2f cm\n", elevatedHeight);
    return elevatedHeight;
}
void ComplementaryFilter(int16_t ax, int16_t ay, int16_t az, int16_t gy, int16_t gz, float elapsed)
{
    long squaresum = (long)ay * ay + (long)az * az;
    // calculate pitch from raw gyro data
    pitch += ((-gy / 32.8) * (elapsed / 1000.0));
    // calculate pitch from raw acclerometer data
    pitchAcc = atan(ax / sqrt(squaresum)) * RAD_TO_DEG;
    // combine and complement the data to get the accurate pitch angle
    pitch = P_CompCoeff * pitch + (1.0 - P_CompCoeff) * pitchAcc;
}
void getPitch()
{
    int16_t p_acceleration[3], p_gyro[3];
    int16_t accX = 0, accY = 0, accZ = 0;
    int16_t gyX = 0, gyY = 0, gyZ = 0;

    absolute_time_t prevTime, currentTime;
    float prevTimems = 0.0, currentTimems = 0.0, elapsedTime = 0.0;

    // read raw acceleration values from MPU6050
    mpu6050_read_rawA(p_acceleration);
    accX = p_acceleration[0];
    accY = p_acceleration[1];
    accZ = p_acceleration[2];

    // Get elapsed time for the calculation of raw gyro data in deg/s
    prevTimems = currentTimems;
    currentTime = get_absolute_time();
    currentTimems = to_ms_since_boot(currentTime);
    elapsedTime = (currentTimems - prevTimems) / 1000.0;

    // read raw gyro values from MPU6050
    mpu6050_read_rawG(p_gyro);
    gyX = p_gyro[0];
    gyY = p_gyro[1];
    gyZ = p_gyro[2];

    ComplementaryFilter(accX, accY, accZ, gyY, gyZ, elapsedTime);
    printf("Pitch Angle: %.2f\n", pitch);
}
void init_accel()
{
    // initialize port for i2c1 400KHz
    i2c_init(i2cAccel, 400 * 1000);

    // initialize port for i2c pins
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);

    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(2, 3, GPIO_FUNC_I2C));

    // Reset & initialize MPU
    mpu6050_reset();

    // MPU Calibration
    mpu6050_calibration();
}
bool checkBumpISR(struct repeating_timer *t)
{
    float detectedHeight = 0.0;

    detectedHeight = getHeight();
    detectedHeight -= offsetHeight;

    // Start of bump/bump detected
    if (detectedHeight > detection_threshold)
    {
        if (detectedHeight > highestHeight)
        {
            highestHeight = detectedHeight;
        }
    }
    // End of bump - back to flat ground
    if (detectedHeight < detection_threshold && highestHeight > 0)
    {
        bumpDataReady = true;
    }
    if (bumpDataReady)
    {
        // send to comms
        printf("Sending to comms: %.2f cm\n", highestHeight);

        // reset bump height & bump flag
        highestHeight = 0.0;
        bumpDataReady = false;

        // reset bump height
        bumpDataReady = false;
        highestHeight = 0.0;
    }

    printf("Height: %.2f cm\n", detectedHeight);
    // printf("Offset Height %.2f cm\n", offsetHeight);

    return true;
}