#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "ultrasonic.h"

#define timeOut 26100   // unknown reason for number
#define timeLoop 500    // sample every few (milliseconds)
#define numOfPins 4
#define filterPoints 10.0

float getPulse(int trigPin, int echoPin)
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
    return (float)absolute_time_diff_us(startTime, endTime);
}

void init_ultrasonic() {
    // pin number assignment
    for (int i = 0; i < numOfPins; i++)
    {
        gpio_init(i + 10);
        gpio_init(i + 16); 
        gpio_set_dir(i + 10, GPIO_OUT);
        gpio_set_dir(i + 16, GPIO_IN); 
    }
}

float ultrasonicPulse(int trigPin, int echoPin) {
    float mean = 0.0;
    for (int i = 0; i < filterPoints; i++)
    {
        mean = mean + (getPulse(trigPin, echoPin) / 58);
    }
    mean /= filterPoints;
    printf("value is %f\n", mean);
    return mean;
}