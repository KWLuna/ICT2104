#include "motor.h"
#include "encoder.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

// uint1IN4_t duty_cycle;
uint16_t set_duty_cycle;

//Integers for pulse counters
float target_notches; 

//defining pins for motor
#define PWM0A 0
#define PWM0B 1
#define IN4 6
#define IN3 7
#define IN2 8
#define IN1 9


//Timer ISR for PID every 100ms
bool ISR_PID(struct repeating_timer *t){  
    float right = get_RightCounter();
    float left = get_LeftCounter();
   
    float rightRotation = (right / 20) * 60.00; //calculate RPM for right motor
    reset_RightCounter();//reset counter to 0

    float leftRotation = (left / 20) * 60.00;
    reset_LeftCounter();//reset counter to 0
    
    //PID values
    float kp = 0.17; 
    float ki = 0.95;
    float kd = 0.2; 
    float lasterror; 
 
    //PID Calculation 
    float target_position = rightRotation; 
    float error = target_position - leftRotation;
    float integral = ki + error; 
    float derivative = error - lasterror; 
    
    //error correction 
    float pwm = (kp * error) + (ki * integral) + (kd * derivative);
    printf("Error: %f, target: %f\n", error, target_position);
    float new_duty_cycle = set_duty_cycle + pwm;
    init_pwmChanB(new_duty_cycle);
    lasterror = error;
    return true;

}

void init_motor() {
    //Initialise gpio pins for IN1, IN2, IN3 & IN4
    gpio_init(IN4); 
    gpio_init(IN3);
    gpio_init(IN2);
    gpio_init(IN1);
    
    //Set pins as output 
    gpio_set_dir(IN4, GPIO_OUT);
    gpio_set_dir(IN3, GPIO_OUT);
    gpio_set_dir(IN2, GPIO_OUT);
    gpio_set_dir(IN1, GPIO_OUT);

}

//Motor configuration 
void init_pwmChanA(uint16_t duty) {
	const uint32_t freq_pwm = 1000; //Frequency we want to generate, in Hz
    set_duty_cycle = duty; //Duty cycle, in percent

    // Tell GPIO 0 that they are allocated to the PWM
	gpio_set_function(PWM0A, GPIO_FUNC_PWM); 
    //Get PWM slice for GPIO 0 (it's slice 0)
	uint slice_num = pwm_gpio_to_slice_num(0); 

	//Set frequency
	//Determine top given Hz - assumes free-running counter rather than phase-correct
	uint32_t f_sys = clock_get_hz(clk_sys);     //typically 125'000'000 Hz
	float divider = f_sys / 1000000UL;          //let's arbitrarily choose to run pwm clock at 1MHz
	pwm_set_clkdiv(slice_num, divider);         //pwm clock should now be running at 1MHz
	uint32_t top =  1000000UL/freq_pwm - 1;     //TOP is u16 has a max of 65535, being 65536 cycles
	pwm_set_wrap(slice_num, top);

	//Set duty cycle
	uint16_t level = (top + 1) * set_duty_cycle / 100 - 1; //Calculate channel level from given duty cycle in %
	pwm_set_chan_level(slice_num, PWM_CHAN_A, level);	
	pwm_set_enabled(slice_num, true); 

}

//Motor configuration 
void init_pwmChanB(uint16_t duty) {
	const uint32_t freq_pwm = 1000; 
    set_duty_cycle = duty; 

    gpio_set_function(PWM0B, GPIO_FUNC_PWM);

	uint slice_num = pwm_gpio_to_slice_num(0); 
	uint32_t f_sys = clock_get_hz(clk_sys); 
	float divider = f_sys / 1000000UL; 
	pwm_set_clkdiv(slice_num, divider); 
	uint32_t top =  1000000UL/freq_pwm - 1; 
	pwm_set_wrap(slice_num, top); 

	uint16_t level = (top + 1) * set_duty_cycle / 100 - 1; 
    pwm_set_chan_level(slice_num, PWM_CHAN_B, level);	
	pwm_set_enabled(slice_num, true); 

}


void moveBackward() {
    set_duty_cycle = 70;
    init_pwmChanA(set_duty_cycle);
    init_pwmChanB(set_duty_cycle);

    //Set pin direction 
    gpio_put(IN4, 1);
    gpio_put(IN3, 0);

    gpio_put(IN2, 1);
    gpio_put(IN1, 0);
}

void moveForward() {
    set_duty_cycle = 70;
    init_pwmChanA(set_duty_cycle);
    init_pwmChanB(set_duty_cycle);
    //Set pin direction 
    gpio_put(IN4, 0);
    gpio_put(IN3, 1);

    gpio_put(IN2, 0);
    gpio_put(IN1, 1);
}

void stop() {
    //Off all pins 
    gpio_put(IN4, 0);
    gpio_put(IN3, 0);

    gpio_put(IN2, 0);
    gpio_put(IN1, 0);
}

//Increase duty cycle by 10 %
void speedUp() {

    if (set_duty_cycle < 90){
        set_duty_cycle = set_duty_cycle + 10;
        init_pwmChanA(set_duty_cycle);
        init_pwmChanB(set_duty_cycle);
    }

}
//Decrease duty cycle by 10 %
void slowDown() {
    if (set_duty_cycle > 10){
        set_duty_cycle =  set_duty_cycle - 10;
        init_pwmChanA(set_duty_cycle);
        init_pwmChanB(set_duty_cycle);        
    }

}

void turnLeft180(){
    reset_RightNotches(); 
    moveForward();
    // put all pins except for IN1 to low
    gpio_put(IN4, 0);
    gpio_put(IN3, 0);
    gpio_put(IN2, 0);
    gpio_put(IN1, 1);
    init_pwmChanB(0);
   float rightNotches = get_RightNotches();
    while(rightNotches < 37){
        rightNotches = get_RightNotches();
    }
    //reset_RightNotches();
    stop();
}

void turnRight180(){
    reset_LeftNotches();
    moveForward(); 
    // put all pins except for IN3 to low
    gpio_put(IN4, 0);
    gpio_put(IN3, 1);
    gpio_put(IN2, 0);
    gpio_put(IN1, 0);
    init_pwmChanA(0);
    float leftNotches = get_LeftNotches();
    while(leftNotches < 37){
        leftNotches = get_LeftNotches();
    }
    //reset_LeftNotches();
    stop();
}

void turnLeft90(){
    reset_RightNotches();
    moveForward(); 
    // put all pins except for IN1 to low
    gpio_put(IN4, 0);
    gpio_put(IN3, 0);
    gpio_put(IN2, 0);
    gpio_put(IN1, 1);
    init_pwmChanB(0);
    float rightNotches = get_RightNotches();
    while(rightNotches < 20){
        rightNotches = get_RightNotches();
    }
    stop();
}

void turnRight90(){
    reset_LeftNotches();
    moveForward();
    // put all pins except for IN3 to low
    gpio_put(IN4, 0);
    gpio_put(IN3, 1);
    gpio_put(IN2, 0);
    gpio_put(IN1, 0);
    init_pwmChanA(0);
    float leftNotches = get_LeftNotches();
    while(leftNotches < 20){
        leftNotches = get_LeftNotches();
    }
    //reset_LeftNotches();
    stop();
}

void turnLeft45(){
    reset_RightNotches();
    moveForward();
    // put all pins except for IN1 to low
    gpio_put(IN4, 0);
    gpio_put(IN3, 0);
    gpio_put(IN2, 0);
    gpio_put(IN1, 1);
    init_pwmChanB(0);
    float rightNotches = get_RightNotches();
    while(rightNotches < 13){
        rightNotches = get_RightNotches();
    }
    stop();
}
    

void turnRight45(){
    reset_LeftNotches();
    moveForward();
    // put all pins except for IN3 to low
    gpio_put(IN4, 0);
    gpio_put(IN3, 1);
    gpio_put(IN2, 0);
    gpio_put(IN1, 0);
    init_pwmChanA(0);
    float leftNotches = get_LeftNotches();
    while(leftNotches < 13){
        leftNotches = get_LeftNotches();
    }
    stop();
}

void turnLeft15(){
    reset_RightNotches();
    moveForward();
    // put all pins except for IN1 to low
    gpio_put(IN4, 0);
    gpio_put(IN3, 0);
    gpio_put(IN2, 0);
    gpio_put(IN1, 1);
    init_pwmChanB(0);
    float rightNotches = get_RightNotches();
    while(rightNotches < 4){
        rightNotches = get_RightNotches();
    }
    stop();
}

void turnRight15(){
    reset_LeftNotches();
    moveForward();
    // put all pins except for IN3 to low
    gpio_put(IN4, 0);
    gpio_put(IN3, 1);
    gpio_put(IN2, 0);
    gpio_put(IN1, 0);
    init_pwmChanA(0);
    float leftNotches = get_LeftNotches();
    while(leftNotches < 4){
        leftNotches = get_LeftNotches();
    }
    stop();
}

//move forward by distance 
void moveForwardBY(float cm) {
    reset_notches();
    float target_notches = CMtoSteps(cm);
    set_duty_cycle = 70;
    init_pwmChanA(set_duty_cycle);
    init_pwmChanB(set_duty_cycle);
    gpio_put(IN4, 0);
    gpio_put(IN3, 1);

    gpio_put(IN2, 0);
    gpio_put(IN1, 1);
    float notches; 
    while (notches < target_notches){
        notches = get_current_notches();
        printf("notches: %f target: %f\n", notches, target_notches);
    }
    //reset_notches();
    stop();
}

//move backward by distance 
void moveBackwardBY(float cm) {
    reset_notches();
    float target_notches = CMtoSteps(cm);
    set_duty_cycle = 70;
    init_pwmChanA(set_duty_cycle);
    init_pwmChanB(set_duty_cycle);
    gpio_put(IN4, 1);
    gpio_put(IN3, 0);

    gpio_put(IN2, 1);
    gpio_put(IN1, 0);
    float notches; 
    while (notches < target_notches){
        notches = get_current_notches();
        printf("notches: %f target: %f\n", notches, target_notches);
    }
    //reset_notches();
    stop();
}