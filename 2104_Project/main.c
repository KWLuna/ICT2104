/* Includes */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "motor.h"
#include "encoder.h"
#include "mapping.h"

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

// Navigation
int navigationArray[9][11];

// Mapping
Grid grid;  // The Map 
Car car;    // Our Car
int numNodeVisited = 0; // counter for number of nodes visited
int stackTop = -1;  // to track the top element of stack array
Stack dfsStack[NUMBER_OF_NODES]; // Store Node to of where the stack can travel to but has not visited
int carPrevX = CAR_START_X;
int carPrevY = CAR_START_Y;

/* Main program */
int main()
{
    stdio_init_all(); // Enable UART so we can print status output
    
    init_motor(); //initialize motor
    init_pins(); //initialize pins for encoder
    init_ultrasonic(); //initialize pins for ultrasonic 
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