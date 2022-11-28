#ifndef barcode_h
#define barcode_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/adc.h"

#define ADC_NUM 0
#define BARCODE_PIN 26 // GPIO pin 26 ADC0
#define ADC_VREF 3.3
#define ADC_RANGE (1 << 12)                      // 2^12
#define ADC_CONVERT (ADC_VREF / (ADC_RANGE - 1)) // 3.3/4097
#define NUM_BLACK_BAR 15
#define NUM_WHITE_BAR 15

void init_barcode();
void combineArray(char bars[], char spaces[], char combined[]);
void decipherBarcode(char* combined);
void printArray(char arr[], int size);
char compareArray(char a[], char b[], int size);
void decodeBarcode();
void detectBar(int adc_reading);
void barcode_sampling();
void barcode_IRQ();

#endif /*BARCODE_H*/