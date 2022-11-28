#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdio.h>
#include "pico/stdlib.h"

void init_motor();
void init_pwmChanA(uint16_t duty);
void init_pwmChanB(uint16_t duty);
void moveBackward();
void moveBackwardBY(float cm);
void moveForward(); 
void moveForwardBY(float cm); 
void stop();
void speedUp();
void slowDown();
void turnLeft180();
void turnRight180();
void turnLeft90();
void turnRight90();
void turnLeft45();
void turnRight45();
void turnLeft15();
void turnRight15();
bool ISR_PID(struct repeating_timer *t);

#endif /* MOTOR_H_*/