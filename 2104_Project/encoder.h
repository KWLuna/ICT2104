#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdio.h>
#include "pico/stdlib.h"

void ISR_rightEncoder();
void ISR_leftEncoder();
int get_RightCounter();
int get_LeftCounter();
int CMtoSteps(float cm);
void gpio_callback(uint gpio, uint32_t events);
void init_pins();
bool ISR_timer(struct repeating_timer *t);
float get_current_notches();
void reset_notches();

#endif /* ENCODER_H_*/
