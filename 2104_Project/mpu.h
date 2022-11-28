#ifndef MPU_H_
#define MPU_H_

static void mpu6050_reset();
static void mpu6050_read_rawA(int16_t accel[3]);
static void mpu6050_read_rawG(int16_t gyro[3]);
int calcAccAngle(int16_t accVal, int16_t inMin, int16_t inMax, int16_t outMin, int16_t outMax);
float calcTiltAngleX(int16_t aAngleY, int16_t aAngleZ);
float calcHeight(float Angle);
void mpu6050_calibration();
float getHeight();
void ComplementaryFilter(int16_t ax, int16_t ay, int16_t az, int16_t gy, int16_t gz, float elapsed);
void getPitch();
void init_accel();
bool checkBumpISR(struct repeating_timer *t);

#endif
