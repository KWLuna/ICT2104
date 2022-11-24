
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "motor.h"

#define LEFT_ENCODER_PIN 15
#define RIGHT_ENCODER_PIN 14
#define LED_PIN 25
#define PI 3.142
#define WHEEL_RADIUS 3 //in cm


//Float for number of slots in encoder disk
const float stepCount = 20.00;


//Integers for pulse counters
volatile int rightCounter = 0;
volatile int leftCounter = 0;
volatile int current_notches = 0;

//right encoder pulse count ISR
void ISR_rightEncoder(){
    rightCounter++;
    current_notches++;
}

//left encoder pulse count ISR
void ISR_leftEncoder(){
    leftCounter++;
}

int get_RightCounter(){
    return rightCounter;
}

int get_LeftCounter(){
    return leftCounter;
}

float get_current_notches(){
    return current_notches;
}
 
void reset_notches(){
    current_notches = 0; 
}
//convert cm to steps
int CMtoSteps(float cm){
    int result;
    float circumference = 2 * PI * WHEEL_RADIUS; //in cm
    float cm_step = circumference / stepCount; //cm per steps

    float f_result = cm / cm_step; //calculate result as float
    return f_result;
}


void gpio_callback(uint gpio, uint32_t events){
    
    if(gpio == RIGHT_ENCODER_PIN)
    {
        ISR_rightEncoder();

    }
    else if(gpio == LEFT_ENCODER_PIN){
        ISR_leftEncoder();
    }
}

void init_pins()
{
    stdio_init_all();

    // LED initialise
    gpio_init(LED_PIN); 
    gpio_set_dir(LED_PIN, GPIO_OUT);

    //encoder
    gpio_init(RIGHT_ENCODER_PIN);
    gpio_set_dir(RIGHT_ENCODER_PIN, GPIO_IN);
    gpio_pull_up(RIGHT_ENCODER_PIN);

    gpio_init(LEFT_ENCODER_PIN);
    gpio_set_dir(LEFT_ENCODER_PIN, GPIO_IN);
    gpio_pull_up(LEFT_ENCODER_PIN);
}
//Timer ISR
bool ISR_timer(struct repeating_timer *t){  
    float rightRotation = (rightCounter / stepCount) * 60.00; //calculate RPM for right motor
    rightCounter = 0; //reset counter to 0

    float leftRotation = (leftCounter / stepCount) * 60.00;
    leftCounter = 0; //reset counter to 0
    return true;

}
