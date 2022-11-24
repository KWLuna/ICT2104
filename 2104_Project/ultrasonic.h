#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

void init_ultrasonic();
void setupUltrasonicPins(int trigPin, int echoPin);
int ultrasonicPulse(int trigPin, int echoPin);
int checkDistance(int trigPin, int echoPin);
int getPulse(int trigPin, int echoPin);

#endif /* ULTRASONIC_H_*/