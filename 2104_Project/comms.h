/*
 * Instructions to be passed as first parameter for send functions (comms)
 * Used by M5 to differentiate the data received
 */
#define M5_BARCODE ((uint8_t)97)  // a
#define M5_DISTANCE ((uint8_t)98) // b
#define M5_HUMP ((uint8_t)99)     // c
#define M5_MAP ((uint8_t)100)     // d

/* UART settings */
#define UART_BAUD_RATE 115200

/* I2C settings */
#define M5_SLAVE 8               // Slave address of M5StickC Plus
#define I2C_BAUD_RATE 100 * 1000 // 100KHz

// Coordinates received from M5
extern bool dataAvailable;
extern uint8_t coords[2];
extern uint8_t x, y;

/* UART functions */
void uart_start();
void uart_send_barcode(uint8_t inst, char data);
void uart_send_float(uint8_t inst, float data);
void uart_send_map(uint8_t mapArr[9][11]);
void uart_read_data();

/* I2C functions */
void i2c_start();
void i2c_send_barcode(uint8_t inst, char data);
void i2c_send_float(uint8_t inst, float data);
void i2c_read_data();