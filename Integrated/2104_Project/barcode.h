#ifndef BARCODE_H_
#define BARCODE_H_

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

// Alarm interrupt handler
static volatile bool alarm_fired;

void init_barcode();
static uint64_t get_time(void);
void printArray(char arr[], int size);
char compareArray(char a[], char b[], int size);
void decodeBarcode();
void detectBar();
void addArrayElement();

#endif /*BARCODE_H*/