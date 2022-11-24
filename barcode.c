#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "time.h"

// line sensor
#define ADC_NUM 0
#define ADC_PIN (26 + ADC_NUM) // GPIO pin 26 ADC0
#define ADC_VREF 3.3
#define ADC_RANGE (1 << 12)                      // 2^12
#define ADC_CONVERT (ADC_VREF / (ADC_RANGE - 1)) // 3.3/4097

/*Bar code variables*/
long currentMillis = 0;  // current timing for black and white bar
long previousMillis = 0; // previous timing for black and white bar
const uint delay = 10; // 1s delay 1000ms = 1s
char barcode[9];         // to store decoded letter

//line sensor
void init_barcode()
{
    stdio_init_all();

    // line sensor adc pins initialise
    adc_gpio_init(ADC_PIN);
    adc_init();
    adc_select_input(ADC_NUM);
}

// function to print array element
void printArray(char arr[], int size)
{
    int i = 0;

    printf("\nElements are : ");

    for (i = 0; i < size; i++)
    {
        printf("\n\tarr[%d] : %d", i, arr[i]);
    }
    printf("\n");
}

// function to compare array elements
char compareArray(char a[], char b[], int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        if (a[i] != b[i])
            return 1;
    }
    return 0;
}

// function to decode barcode
void decodeBarcode()
{
    char zero[9] = {0, 0, 0, 1, 1, 0, 1, 0, 0};
    char one[9] = {1, 0, 0, 1, 0, 0, 0, 0, 1};
    char two[9] = {0, 0, 1, 1, 0, 0, 0, 0, 1};
    char three[9] = {1, 0, 1, 1, 0, 0, 0, 0, 0};
    char four[9] = {0, 0, 0, 1, 1, 0, 0, 0, 1};
    char five[9] = {1, 0, 0, 1, 1, 0, 0, 0, 0};
    char six[9] = {0, 0, 1, 1, 1, 0, 0, 0, 0};
    char seven[9] = {0, 0, 0, 1, 0, 0, 1, 0, 1};
    char eight[9] = {1, 0, 0, 1, 0, 0, 1, 0, 0};
    char nine[9] = {0, 0, 1, 1, 0, 0, 1, 0, 0};
    char A[9] = {1, 0, 0, 0, 0, 1, 0, 0, 1};
    char B[9] = {0, 0, 1, 0, 0, 1, 0, 0, 1};
    char C[9] = {1, 0, 1, 0, 0, 1, 0, 0, 0};
    char D[9] = {0, 0, 0, 0, 1, 1, 0, 0, 1};
    char E[9] = {1, 0, 0, 0, 1, 1, 0, 0, 0};
    char F[9] = {0, 0, 1, 0, 1, 1, 0, 0, 0};
    char G[9] = {0, 0, 0, 0, 0, 1, 1, 0, 1};
    char H[9] = {1, 0, 0, 0, 0, 1, 1, 0, 0};
    char I[9] = {0, 0, 1, 0, 0, 1, 1, 0, 0};
    char J[9] = {0, 0, 0, 0, 1, 1, 1, 0, 0};
    char K[9] = {1, 0, 0, 0, 0, 0, 0, 1, 1};
    char L[9] = {0, 0, 1, 0, 0, 0, 0, 1, 1};
    char M[9] = {1, 0, 1, 0, 0, 0, 0, 1, 0};
    char N[9] = {0, 0, 0, 0, 1, 0, 0, 1, 1};
    char O[9] = {1, 0, 0, 0, 1, 0, 0, 1, 0};
    char P[9] = {0, 0, 1, 0, 1, 0, 0, 1, 0};
    char Q[9] = {0, 0, 0, 0, 0, 0, 1, 1, 1};
    char R[9] = {1, 0, 0, 0, 0, 0, 1, 1, 0};
    char S[9] = {0, 0, 1, 0, 0, 0, 1, 1, 0};
    char T[9] = {0, 0, 0, 0, 1, 0, 1, 1, 0};
    char U[9] = {1, 1, 0, 0, 0, 0, 0, 0, 1};
    char V[9] = {0, 1, 1, 0, 0, 0, 0, 0, 1};
    char W[9] = {1, 1, 1, 0, 0, 0, 0, 0, 0};
    char X[9] = {0, 1, 0, 0, 1, 0, 0, 0, 1};
    char Y[9] = {1, 1, 0, 0, 1, 0, 0, 0, 0};
    char Z[9] = {0, 1, 1, 0, 1, 0, 0, 0, 0};
    char hyphen[9] = {0, 1, 0, 0, 0, 0, 1, 0, 1};
    char dot[9] = {1, 1, 0, 0, 0, 0, 1, 0, 0};
    char space[9] = {0, 1, 1, 0, 0, 0, 1, 0, 0};
    char dollar[9] = {0, 1, 0, 1, 0, 1, 0, 0, 0};
    char slash[9] = {0, 1, 0, 1, 0, 0, 0, 1, 0};
    char plus[9] = {0, 1, 0, 0, 0, 1, 0, 1, 0};
    char percent[9] = {0, 0, 0, 1, 0, 1, 0, 1, 0};
    char asterisk[9] = {0, 1, 0, 0, 1, 0, 1, 0, 0};

    if (compareArray(zero, barcode, 9) == 0)
    {
        printArray(zero, 9);
        printArray(barcode, 9);
        printf("Character identified is 0\n");
    }
    else if (compareArray(one, barcode, 9) == 0)
    {
        printArray(one, 9);
        printArray(barcode, 9);
        printf("Character identified is 1\n");
    }
    else if (compareArray(two, barcode, 9) == 0)
    {
        printArray(two, 9);
        printArray(barcode, 9);
        printf("Character identified is 2\n");
    }
    else if (compareArray(three, barcode, 9) == 0)
    {
        printArray(three, 9);
        printArray(barcode, 9);
        printf("Character identified is 3\n");
    }
    else if (compareArray(four, barcode, 9) == 0)
    {
        printArray(four, 9);
        printArray(barcode, 9);
        printf("Character identified is 4\n");
    }
    else if (compareArray(five, barcode, 9) == 0)
    {
        printArray(five, 9);
        printArray(barcode, 9);
        printf("Character identified is 5\n");
    }
    else if (compareArray(six, barcode, 9) == 0)
    {
        printArray(six, 9);
        printArray(barcode, 9);
        printf("Character identified is 6\n");
    }
    else if (compareArray(seven, barcode, 9) == 0)
    {
        printArray(seven, 9);
        printArray(barcode, 9);
        printf("Character identified is 7\n");
    }
    else if (compareArray(eight, barcode, 9) == 0)
    {
        printArray(eight, 9);
        printArray(barcode, 9);
        printf("Character identified is 8\n");
    }
    else if (compareArray(nine, barcode, 9) == 0)
    {
        printArray(nine, 9);
        printArray(barcode, 9);
        printf("Character identified is 9\n");
    }
    else if (compareArray(A, barcode, 9) == 0)
    {
        printArray(A, 9);
        printArray(barcode, 9);
        printf("Character identified is A\n");
    }
    else if (compareArray(B, barcode, 9) == 0)
    {
        printArray(B, 9);
        printArray(barcode, 9);
        printf("Character identified is B\n");
    }
    else if (compareArray(C, barcode, 9) == 0)
    {
        printArray(C, 9);
        printArray(barcode, 9);
        printf("Character identified is C\n");
    }
    else if (compareArray(D, barcode, 9) == 0)
    {
        printArray(D, 9);
        printArray(barcode, 9);
        printf("Character identified is D\n");
    }
    else if (compareArray(E, barcode, 9) == 0)
    {
        printArray(E, 9);
        printArray(barcode, 9);
        printf("Character identified is E\n");
    }
    else if (compareArray(F, barcode, 9) == 0)
    {
        printArray(F, 9);
        printArray(barcode, 9);
        printf("Character identified is F\n");
    }
    else if (compareArray(G, barcode, 9) == 0)
    {
        printArray(G, 9);
        printArray(barcode, 9);
        printf("Character identified is G\n");
    }
    else if (compareArray(H, barcode, 9) == 0)
    {
        printArray(H, 9);
        printArray(barcode, 9);
        printf("Character identified is H\n");
    }
    else if (compareArray(I, barcode, 9) == 0)
    {
        printArray(I, 9);
        printArray(barcode, 9);
        printf("Character identified is I\n");
    }
    else if (compareArray(J, barcode, 9) == 0)
    {
        printArray(J, 9);
        printArray(barcode, 9);
        printf("Character identified is J\n");
    }
    else if (compareArray(K, barcode, 9) == 0)
    {
        printArray(K, 9);
        printArray(barcode, 9);
        printf("Character identified is K\n");
    }
    else if (compareArray(L, barcode, 9) == 0)
    {
        printArray(L, 9);
        printArray(barcode, 9);
        printf("Character identified is L\n");
    }
    else if (compareArray(M, barcode, 9) == 0)
    {
        printArray(M, 9);
        printArray(barcode, 9);
        printf("Character identified is M\n");
    }
    else if (compareArray(N, barcode, 9) == 0)
    {
        printArray(N, 9);
        printArray(barcode, 9);
        printf("Character identified is N\n");
    }
    else if (compareArray(O, barcode, 9) == 0)
    {
        printArray(O, 9);
        printArray(barcode, 9);
        printf("Character identified is O\n");
    }
    else if (compareArray(P, barcode, 9) == 0)
    {
        printArray(P, 9);
        printArray(barcode, 9);
        printf("Character identified is P\n");
    }
    else if (compareArray(Q, barcode, 9) == 0)
    {
        printArray(Q, 9);
        printArray(barcode, 9);
        printf("Character identified is Q\n");
    }
    else if (compareArray(R, barcode, 9) == 0)
    {
        printArray(R, 9);
        printArray(barcode, 9);
        printf("Character identified is R\n");
    }
    else if (compareArray(S, barcode, 9) == 0)
    {
        printArray(S, 9);
        printArray(barcode, 9);
        printf("Character identified is S\n");
    }
    else if (compareArray(T, barcode, 9) == 0)
    {
        printArray(T, 9);
        printArray(barcode, 9);
        printf("Character identified is T\n");
    }
    else if (compareArray(U, barcode, 9) == 0)
    {
        printArray(U, 9);
        printArray(barcode, 9);
        printf("Character identified is U\n");
    }
    else if (compareArray(V, barcode, 9) == 0)
    {
        printArray(V, 9);
        printArray(barcode, 9);
        printf("Character identified is V\n");
    }
    else if (compareArray(W, barcode, 9) == 0)
    {
        printArray(W, 9);
        printArray(barcode, 9);
        printf("Character identified is W\n");
    }
    else if (compareArray(X, barcode, 9) == 0)
    {
        printArray(X, 9);
        printArray(barcode, 9);
        printf("Character identified is X\n");
    }
    else if (compareArray(Y, barcode, 9) == 0)
    {
        printArray(Y, 9);
        printArray(barcode, 9);
        printf("Character identified is Y\n");
    }
    else if (compareArray(Z, barcode, 9) == 0)
    {
        printArray(Z, 9);
        printArray(barcode, 9);
        printf("Character identified is Z\n");
    }
    else if (compareArray(hyphen, barcode, 9) == 0)
    {
        printArray(hyphen, 9);
        printArray(barcode, 9);
        printf("Character identified is -\n");
    }
    else if (compareArray(dot, barcode, 9) == 0)
    {
        printArray(dot, 9);
        printArray(barcode, 9);
        printf("Character identified is .\n");
    }
    else if (compareArray(space, barcode, 9) == 0)
    {
        printArray(space, 9);
        printArray(barcode, 9);
        printf("Character identified is a space\n");
    }
    else if (compareArray(dollar, barcode, 9) == 0)
    {
        printArray(dollar, 9);
        printArray(barcode, 9);
        printf("Character identified is $\n");
    }
    else if (compareArray(slash, barcode, 9) == 0)
    {
        printArray(slash, 9);
        printArray(barcode, 9);
        printf("Character identified is /\n");
    }
    else if (compareArray(plus, barcode, 9) == 0)
    {
        printArray(plus, 9);
        printArray(barcode, 9);
        printf("Character identified is +\n");
    }
    else if (compareArray(percent, barcode, 9) == 0)
    {
        printArray(percent, 9);
        printArray(barcode, 9);
        printf("Character identified is %c\n", '%');
    }
    else if (compareArray(asterisk, barcode, 9) == 0)
    {
        printArray(asterisk, 9);
        printArray(barcode, 9);
        printf("Character identified is *\n");
    }
    else
    {
        printf("Different elements, no matches\n");
    }
}

clock_t clock()
{
    return (clock_t)time_us_32() / 10000; // to milisecond
}

void detectBar()
{
    uint adc_raw;
    static volatile float normalizedADCRes;
    volatile int lineCount = 0;
    bool lineDetected = false;
    int lineThreshold = 1200;
    float totalTimeTaken = time_us_32();

    clock_t prevtime;
    clock_t blackTime;
    clock_t whiteTime;
    double currentTimeWhite;
    double currentTimeBlack;

    while (lineCount <= 9)
    {
        adc_raw = adc_read(); // raw voltage from ADC
        normalizedADCRes = adc_raw * ADC_CONVERT;
        //printf("Raw value: %u, voltage: %f V\n", adc_raw, normalizedADCRes);
        sleep_ms(delay);

        if (lineDetected == false)
        {
            if (adc_raw > lineThreshold) // black
            {
                blackTime = clock(); // keep running and capture it now
                double currentTimeBlack = (double)(blackTime - prevtime) / CLOCKS_PER_SEC;
                prevtime = blackTime;
                printf("%.8f seconds\n", currentTimeBlack);

                // add elements into array
                for (int c = 9 - 1; c >= lineCount - 1; c--)
                {
                    barcode[c + 1] = barcode[c];
                }

                if (currentTimeWhite == 0)
                {
                
                    barcode[lineCount] = 0; // thin
                }
                else
                {
                    barcode[lineCount] = 1; // thick
                }

                printf("Resultant array is\n");
                printArray(barcode, 9);

                lineCount++;
                lineDetected = true;
                printf("%i, black %d\n", lineCount, barcode[lineCount]);
            }
        }
        else if (lineDetected == true)
        {
            if (adc_raw < lineThreshold) // white
            {
                whiteTime = clock(); // keep running and capture it now;
                double currentTimeWhite = (double)(whiteTime - prevtime) / CLOCKS_PER_SEC;
                prevtime = whiteTime;
                printf("%.8f seconds\n", currentTimeWhite);

                if (currentTimeWhite <= currentTimeBlack)
                {
                    barcode[lineCount] = 0; // thin
                }
                else
                {
                    barcode[lineCount] = 1; // thick
                }

                // add elements into array
                for (int c = 9 - 1; c >= lineCount - 1; c--)
                {
                    barcode[c + 1] = barcode[c];
                }

                printf("Resultant array is\n");
                printArray(barcode, 9);

                lineCount++;
                lineDetected = false;
                printf("%i white %d\n", lineCount, barcode[lineCount]);
            }
        }
        decodeBarcode();
    }
    printf("1 character detected\n"); // not execute until it is more than 9
}