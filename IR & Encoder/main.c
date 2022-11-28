#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "barcode.h"
#include "encoder.h"


int main()
{
    stdio_init_all();
    init_barcode();
    init_pins();

    // struct repeating_timer timer;                   
    // add_repeating_timer_ms(100, encoder_timer, NULL, &timer); // set timer for every 100ms

    // gpio_set_irq_enabled_with_callback(RIGHT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true, gpio_callback); //encoder
    // gpio_set_irq_enabled_with_callback(LEFT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true, gpio_callback); //encoder
    

    while (1)
    {
      barcode_IRQ();
    }
    return 0;
}
