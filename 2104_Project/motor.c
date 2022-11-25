#include "motor.h"
#include "encoder.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

// uint16_t duty_cycle;
uint16_t set_duty_cycle;

//Integers for pulse counters
float target_notches; 


#define PWM0A 0
#define PWM0B 1
#define IN4 6
#define IN3 7
#define IN2 8
#define IN1 9


//Timer ISR for PID every 100ms
bool ISR_PID(struct repeating_timer *t){  
    float target_notches = CMtoSteps(60);
    float notches = get_current_notches();
    float right = get_RightCounter();
    float left = get_LeftCounter();
    printf("Right: %f, left: %f, notches: %f, target: %f\n", right, left, notches, target_notches);
    
    float rightRotation = (right / 20) * 60.00; //calculate RPM for right motor
    //printf("Right motor speed: %f RPM \n", rightRotation);
    reset_RightCounter();//reset counter to 0

    float leftRotation = (left / 20) * 60.00;
    //printf("Left motor speed: %f RPM \n", leftRotation);
    reset_LeftCounter();//reset counter to 0
    

    float kp = 0.17; 
    float ki = 0.95;
    float kd = 0.2; 
    float lasterror; 

    float target_position = rightRotation; 
    float error = target_position - leftRotation;
    float integral = ki + error; 
    float derivative = error - lasterror; 

    float pwm = (kp * error) + (ki * integral) + (kd * derivative);
    //printf("Error: %f, target: %f\n", error, target_position);
    //printf("Right: %f, left: %f, notches: %f, target: %f\n", right, left, notches, target_notches);

    float new_duty_cycle = set_duty_cycle + pwm;
    init_pwmChanB(new_duty_cycle);
    lasterror = error;
    return true;

}

void init_motor() {
    // initialise gpio pins for IN1, IN2, IN3 & IN4
    gpio_init(6); 
    gpio_init(7);
    gpio_init(8);
    gpio_init(9);

    gpio_set_dir(6, GPIO_OUT);
    gpio_set_dir(7, GPIO_OUT);
    gpio_set_dir(8, GPIO_OUT);
    gpio_set_dir(9, GPIO_OUT);

}

void init_pwmChanA(uint16_t duty) {
    // set your desired inputs here
	const uint32_t freq_pwm = 1000; // frequency we want to generate, in Hz
    set_duty_cycle = duty; // duty cycle, in percent

    // Tell GPIO 0 and 1 they are allocated to the PWM
	gpio_set_function(0, GPIO_FUNC_PWM); 

	uint slice_num = pwm_gpio_to_slice_num(0); // get PWM slice for GPIO 0 & 1 (it's slice 0)

	// set frequency
	// determine top given Hz - assumes free-running counter rather than phase-correct
	uint32_t f_sys = clock_get_hz(clk_sys); // typically 125'000'000 Hz
	float divider = f_sys / 1000000UL;  // let's arbitrarily choose to run pwm clock at 1MHz
	pwm_set_clkdiv(slice_num, divider); // pwm clock should now be running at 1MHz
	uint32_t top =  1000000UL/freq_pwm - 1; // TOP is u16 has a max of 65535, being 65536 cycles
	pwm_set_wrap(slice_num, top);

	// set duty cycle
	uint16_t level = (top + 1) * set_duty_cycle / 100 - 1; // calculate channel level from given duty cycle in %
	pwm_set_chan_level(slice_num, PWM_CHAN_A, level);
	
	pwm_set_enabled(slice_num, true); // let's go!

}
void init_pwmChanB(uint16_t duty) {
    // set your desired inputs here
	const uint32_t freq_pwm = 1000; // frequency we want to generate, in Hz
    set_duty_cycle = duty; // duty cycle, in percent

    // Tell GPIO 0 and 1 they are allocated to the PWM
    gpio_set_function(1, GPIO_FUNC_PWM);

	uint slice_num = pwm_gpio_to_slice_num(0); // get PWM slice for GPIO 0 & 1 (it's slice 0)

	// set frequency
	// determine top given Hz - assumes free-running counter rather than phase-correct
	uint32_t f_sys = clock_get_hz(clk_sys); // typically 125'000'000 Hz
	float divider = f_sys / 1000000UL;  // let's arbitrarily choose to run pwm clock at 1MHz
	pwm_set_clkdiv(slice_num, divider); // pwm clock should now be running at 1MHz
	uint32_t top =  1000000UL/freq_pwm - 1; // TOP is u16 has a max of 65535, being 65536 cycles
	pwm_set_wrap(slice_num, top); 

	// set duty cycle
	uint16_t level = (top + 1) * set_duty_cycle / 100 - 1; // calculate channel level from given duty cycle in %
    pwm_set_chan_level(slice_num, PWM_CHAN_B, level);
	
	pwm_set_enabled(slice_num, true); // let's go!

}

void moveBackward() {
    set_duty_cycle = 70;
    init_pwmChanA(set_duty_cycle);
    init_pwmChanB(set_duty_cycle);
    gpio_put(6, 1);
    gpio_put(7, 0);

    gpio_put(8, 1);
    gpio_put(9, 0);
}

void moveForward() {
    set_duty_cycle = 70;
    init_pwmChanA(set_duty_cycle);
    init_pwmChanB(set_duty_cycle);
    gpio_put(6, 0);
    gpio_put(7, 1);

    gpio_put(8, 0);
    gpio_put(9, 1);
}

void stop() {
    gpio_put(6, 0);
    gpio_put(7, 0);

    gpio_put(8, 0);
    gpio_put(9, 0);
}

void speedUp() {
    set_duty_cycle = set_duty_cycle + 10;
    init_pwmChanA(set_duty_cycle);
    init_pwmChanB(set_duty_cycle);
}

void slowDown() {
     set_duty_cycle =  set_duty_cycle - 10;
    init_pwmChanA(set_duty_cycle);
    init_pwmChanB(set_duty_cycle);
}

void turnLeft180(){
    gpio_put(6, 0);
    gpio_put(7, 0);
    gpio_put(8, 0);
    gpio_put(9, 1);
    init_pwmChanA(100);
    init_pwmChanB(0);
    sleep_ms(1785);
    stop();
}

void turnRight180(){
    gpio_put(6, 0);
    gpio_put(7, 1);
    gpio_put(8, 0);
    gpio_put(9, 0);
    init_pwmChanA(0);
    init_pwmChanB(100);
    sleep_ms(1785);
    stop();
}

void turnLeft90(){
    gpio_put(6, 0);
    gpio_put(7, 0);
    gpio_put(8, 0);
    gpio_put(9, 1);
    init_pwmChanA(100);
    init_pwmChanB(0);
    sleep_ms(1100);
    stop();
}

void turnRight90(){
    gpio_put(6, 0);
    gpio_put(7, 1);
    gpio_put(8, 0);
    gpio_put(9, 0);
    init_pwmChanA(0);
    init_pwmChanB(100);
    sleep_ms(1100);
    stop();
}

void turnLeft45(){
    gpio_put(6, 0);
    gpio_put(7, 0);
    gpio_put(8, 0);
    gpio_put(9, 1);
    init_pwmChanA(100);
    init_pwmChanB(0);
    sleep_ms(600);
    stop();
}

void turnRight45(){
    gpio_put(6, 0);
    gpio_put(7, 1);
    gpio_put(8, 0);
    gpio_put(9, 0);
    init_pwmChanA(0);
    init_pwmChanB(100);
    sleep_ms(600);
    stop();
}

void turnLeft15(){
    gpio_put(6, 0);
    gpio_put(7, 0);
    gpio_put(8, 0);
    gpio_put(9, 1);
    init_pwmChanA(100);
    init_pwmChanB(0);
    sleep_ms(0);
    stop();
}

void turnRight15(){
    gpio_put(6, 0);
    gpio_put(7, 1);
    gpio_put(8, 0);
    gpio_put(9, 0);
    init_pwmChanA(0);
    init_pwmChanB(100);
    sleep_ms(250);
    stop();
}

//move forward by distance 
void moveForwardBY(float cm) {
    float target_notches = CMtoSteps(cm);
    set_duty_cycle = 70;
    init_pwmChanA(set_duty_cycle);
    init_pwmChanB(set_duty_cycle);
    gpio_put(6, 0);
    gpio_put(7, 1);

    gpio_put(8, 0);
    gpio_put(9, 1);
    float notches; 
    while (notches < target_notches){
        notches = get_current_notches();
        printf("notches: %f target: %f\n", notches, target_notches);
    }
    reset_notches();
    stop();
}

//move backward by distance 
void moveBackwardBY(float cm) {
    float target_notches = CMtoSteps(cm);
    set_duty_cycle = 70;
    init_pwmChanA(set_duty_cycle);
    init_pwmChanB(set_duty_cycle);
    gpio_put(6, 1);
    gpio_put(7, 0);

    gpio_put(8, 1);
    gpio_put(9, 0);
    float notches; 
    while (notches < target_notches){
        notches = get_current_notches();
        printf("notches: %f target: %f\n", notches, target_notches);
    }
    reset_notches();
    stop();
}

