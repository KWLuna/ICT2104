#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdio.h>
#include "pico/stdlib.h"

#define LEFT_ENCODER_PIN 15
#define RIGHT_ENCODER_PIN 14
#define LED_PIN 25
#define PI 3.142
#define WHEEL_RADIUS 3 //in cm

void init_pins();
bool encoder_timer(struct repeating_timer *t);
void ISR_rightEncoder();
void ISR_leftEncoder();
int get_RightNotches();
int get_LeftNotches();
int get_RightCounter();
int get_LeftCounter();
float get_current_notches();
void reset_notches();
void reset_RightCounter();
void reset_LeftCounter();
int CMtoSteps(float cm);
void gpio_callback(uint gpio, uint32_t events);

#endif /* ENCODER_H_*/
