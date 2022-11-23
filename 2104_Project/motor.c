#include "motor.h"
#include "encoder.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

// uint16_t duty_cycle;
uint16_t set_duty_cycle;

#define PWM0A 0
#define PWM0B 1
#define IN4 6
#define IN3 7
#define IN2 8
#define IN1 9


//Timer ISR for PID every 100ms
bool ISR_PID(struct repeating_timer *t){  
    //retrieve current number of notches for each wheel 
    float rightCounter =  get_RightCounter();
    float leftCounter = get_LeftCounter(); 
    //calculate RPM for both motor
    float rightRotation = (rightCounter / 20) * 60.00; 
    float leftRotation = (leftCounter / 20) * 60.00;
   
    //PID values
    float kp = 0.19; 
    float ki = 0.95;
    float kd = 0.2; 
    float lasterror; 

    //PID calculation 
    float target_position = rightRotation; 
    float error = target_position - leftRotation;
    float integral = ki + error; 
    float derivative = error - lasterror; 

    float pwm = (kp * error) + (ki * integral) + (kd * derivative);
    printf("Error: %f, target: %f\n", error, target_position);

    //error correction 
    float new_duty_cycle= set_duty_cycle+ pwm; 
    init_pwmChanB(new_duty_cycle);
    return true;

}


void init_motor() {
    // initialise gpio pins for IN1, IN2, IN3 & IN4
    gpio_init(IN4); 
    gpio_init(IN3);
    gpio_init(IN2);
    gpio_init(IN1);

    gpio_set_dir(IN4, GPIO_OUT);
    gpio_set_dir(IN3, GPIO_OUT);
    gpio_set_dir(IN2, GPIO_OUT);
    gpio_set_dir(IN1, GPIO_OUT);

}

void init_pwmChanA(uint16_t duty) {
    // set your desired inputs here
	const uint32_t freq_pwm = 1000; // frequency we want to generate, in Hz
    set_duty_cycle= duty; // duty cycle, in percent

    // Tell GPIO 0 and 1 they are allocated to the PWM
	gpio_set_function(PWM0A, GPIO_FUNC_PWM); 

	uint slice_num = pwm_gpio_to_slice_num(0); // get PWM slice for GPIO 0 & 1 (it's slice 0)

	// set frequency
	// determine top given Hz - assumes free-running counter rather than phase-correct
	uint32_t f_sys = clock_get_hz(clk_sys); // typically 125'000'000 Hz
	float divider = f_sys / 1000000UL;  // let's arbitrarily choose to run pwm clock at 1MHz
	pwm_set_clkdiv(slice_num, divider); // pwm clock should now be running at 1MHz
	uint32_t top =  1000000UL/freq_pwm - 1; // TOP is u16 has a max of 65535, being 65536 cycles
	pwm_set_wrap(slice_num, top);

	// set duty cycle
	uint16_t level = (top + 1) * set_duty_cycle/ 100 - 1; // calculate channel level from given duty cycle in %
	pwm_set_chan_level(slice_num, PWM_CHAN_A, level);
	
	pwm_set_enabled(slice_num, true); // let's go!

}
void init_pwmChanB(uint16_t duty) {
    // set your desired inputs here
	const uint32_t freq_pwm = 1000; // frequency we want to generate, in Hz
    set_duty_cycle= duty; // duty cycle, in percent

    // Tell GPIO 0 and 1 they are allocated to the PWM
    gpio_set_function(PWM0B, GPIO_FUNC_PWM);

	uint slice_num = pwm_gpio_to_slice_num(0); // get PWM slice for GPIO 0 & 1 (it's slice 0)

	// set frequency
	// determine top given Hz - assumes free-running counter rather than phase-correct
	uint32_t f_sys = clock_get_hz(clk_sys); // typically 125'000'000 Hz
	float divider = f_sys / 1000000UL;  // let's arbitrarily choose to run pwm clock at 1MHz
	pwm_set_clkdiv(slice_num, divider); // pwm clock should now be running at 1MHz
	uint32_t top =  1000000UL/freq_pwm - 1; // TOP is u16 has a max of 65535, being 65536 cycles
	pwm_set_wrap(slice_num, top);

	// set duty cycle
	uint16_t level = (top + 1) * set_duty_cycle/ 100 - 1; // calculate channel level from given duty cycle in %
    pwm_set_chan_level(slice_num, PWM_CHAN_B, level);
	
	pwm_set_enabled(slice_num, true); // let's go!

}

//move backward
void moveBackward() {
    set_duty_cycle= 70;
    init_pwmChanA(set_duty_cycle);
    init_pwmChanB(set_duty_cycle);
    gpio_put(6, 1);
    gpio_put(7, 0);

    gpio_put(8, 1);
    gpio_put(9, 0);
}

//move forward
void moveForward() {
    set_duty_cycle= 70;
    init_pwmChanA(set_duty_cycle);
    init_pwmChanB(set_duty_cycle);
    gpio_put(6, 0);
    gpio_put(7, 1);

    gpio_put(8, 0);
    gpio_put(9, 1);
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
        printf("notches: %f target: %f", notches, target_notches);
    }
    reset_notches();
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
        printf("notches: %f target: %f", notches, target_notches);
    }
    reset_notches();
}

//stop the car
void stop() {
    gpio_put(6, 0);
    gpio_put(7, 0);

    gpio_put(8, 0);
    gpio_put(9, 0);
}

//increase duty cycle by 10%
void speedUp() {
    if (set_duty_cycle < 90){
        set_duty_cycle = set_duty_cycle + 10;
        init_pwmChanA(set_duty_cycle);
        init_pwmChanB(set_duty_cycle);
    }
}

//decrease duty cycle by 10%
void slowDown() {
    if(set_duty_cycle > 10)
    {
        set_duty_cycle= set_duty_cycle - 10;
        init_pwmChanA(set_duty_cycle);
        init_pwmChanB(set_duty_cycle);
    }
   
}

void turnLeft180(){
    gpio_put(6, 0);
    gpio_put(7, 0);
    init_pwmChanA(100);
    init_pwmChanB(0);
    sleep_ms(1000);
    stop();
}

void turnRight180(){
    gpio_put(8, 0);
    gpio_put(9, 0);
    init_pwmChanA(0);
    init_pwmChanB(100);
    sleep_ms(1000);
    stop();
}

void turnLeft90(){
    gpio_put(6, 0);
    gpio_put(7, 0);
    init_pwmChanA(100);
    init_pwmChanB(0);
    sleep_ms(500);
    stop();
}

void turnRight90(){
    gpio_put(8, 0);
    gpio_put(9, 0);
    init_pwmChanA(0);
    init_pwmChanB(100);
    sleep_ms(500);
    stop();
}

void turnLeft45(){
    gpio_put(6, 0);
    gpio_put(7, 0);
    init_pwmChanA(100);
    init_pwmChanB(0);
    sleep_ms(250);
    stop();
}

void turnRight45(){
    gpio_put(8, 0);
    gpio_put(9, 0);
    init_pwmChanA(0);
    init_pwmChanB(100);
    sleep_ms(250);
    stop();
}