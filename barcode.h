#ifndef BARCODE_H_
#define BARCODE_H_

#include <stdio.h>

void init_barcode();
void printArray(char arr[], int size);
char compareArray(char a[], char b[], int size);
void decodeBarcode();
void detectBar();

#endif /*BARCODE_H*/