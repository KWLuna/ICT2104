/* Includes */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "motor.h"
#include "encoder.h"
/*
 * Instructions to be passed as first parameter for send functions (comms)
 * Used by M5 to differentiate the data received
 */
#define M5_BARCODE 97
#define M5_DISTANCE 98
#define M5_HUMP 99
#define M5_MAP 100
#define LEFT_ENCODER_PIN 15
#define RIGHT_ENCODER_PIN 14

/* Global variables */
// Coordinates received from M5
extern uint8_t coords[2];
extern uint8_t x, y;

/* Main program */
int main()
{
    stdio_init_all(); // Enable UART so we can print status output
    
    init_motor(); //initialize motor
    init_pins(); //initialize pins for encoder 
    struct repeating_timer timer;
    add_repeating_timer_ms(500, ISR_timer, NULL, &timer); //set timer to calculate rotation
    add_repeating_timer_ms(100, ISR_PID, NULL, &timer);   //set pid timer for every 100ms
    gpio_set_irq_enabled_with_callback(RIGHT_ENCODER_PIN,  GPIO_IRQ_EDGE_RISE, true, gpio_callback);
    gpio_set_irq_enabled_with_callback(LEFT_ENCODER_PIN,  GPIO_IRQ_EDGE_RISE, true, gpio_callback);
    
    
    while (1)
    {
        tight_loop_contents();

    };
   
    return 0;
}