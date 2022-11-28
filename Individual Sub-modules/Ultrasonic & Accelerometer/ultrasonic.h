#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

void init_ultrasonic();
float ultrasonicPulse(int trigPin, int echoPin);
float getPulse(int trigPin, int echoPin);

#endif /* ULTRASONIC_H_*/