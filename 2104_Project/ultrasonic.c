#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"

int timeOut = 26100; // unknown reason for number
int timeLoop = 500;  // sample every few (milliseconds)

const int numOfPins = 1;
const int filterPoints = 10;

// set up pins for ultrasound
void setupUltrasonicPins(uint trigPin, uint echoPin)
{
    gpio_init(trigPin);
    gpio_init(echoPin);
    gpio_set_dir(trigPin, GPIO_OUT);
    gpio_set_dir(echoPin, GPIO_IN);
}

uint64_t getPulse(uint trigPin, uint echoPin)
{
    gpio_put(trigPin, 1); // start of trigger signal
    sleep_us(10);         // duration of trigger signal
    gpio_put(trigPin, 0); // end of trigger signal

    uint64_t timeTaken = 0;

    while (gpio_get(echoPin) == 0)
        tight_loop_contents();                       // ensure timer/interrupts/callbacks get called on time
    absolute_time_t startTime = get_absolute_time(); // record start time
    while (gpio_get(echoPin) == 1)
    {
        timeTaken++;
        sleep_us(1);
        if (timeTaken > timeOut)
            return 0;
    }
    absolute_time_t endTime = get_absolute_time(); // record end time
    return absolute_time_diff_us(startTime, endTime);
}

int checkDistance(uint trigPin, uint echoPin)
{
    int meanDist = 0;
    for (int i = 0; i < filterPoints; i++)
    {
        meanDist = meanDist + (int)(getPulse(trigPin, echoPin) / 58);
    }
    meanDist = meanDist / filterPoints;

    return meanDist;
}

void init_ultrasonic() {
    // pin number assignment
    int trigPin[numOfPins];
    int echoPin[numOfPins];
    for (int i = 0; i < numOfPins; i++)
    {
        trigPin[i] = i + 10;
        echoPin[i] = i + 16;
    }

    for (int i = 0; i < numOfPins; i++)
        setupUltrasonicPins(trigPin[i], echoPin[i]); // set up pins
}

int ultrasonicPulse(int trigPin, int echoPin) {
    int mean;

    for (int i = 0; i < numOfPins; i++)
    {
        mean = checkDistance(trigPin, echoPin);
        //printf(" Sensor is %d cm away\n", mean);
        sleep_ms(timeLoop);
    }
    return mean;
}