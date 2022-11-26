#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"

#define ADC_NUM 0
#define BARCODE_PIN 26 // GPIO pin 26 ADC0
#define ADC_VREF 3.3
#define ADC_RANGE (1 << 12)                      // 2^12
#define ADC_CONVERT (ADC_VREF / (ADC_RANGE - 1)) // 3.3/4097
#define N_SAMPLES 100000

// set boolean to indidcate thats the barcode
// detect the white, next is a black bar
// set a benchmark to indicate that that the thick bar from asterisk

/* Global variables */
const uint delay = 500; // 1s delay 1000ms = 1s
char barcode[9];        // to store decoded letter
int timeLoop = 500;     // sample every few (milliseconds)
volatile int lineCount = 0;

/* line sensor */
void init_barcode()
{
    adc_init();
    adc_gpio_init(BARCODE_PIN);
    adc_select_input(ADC_NUM);
    adc_set_temp_sensor_enabled(false);
    adc_fifo_setup(true, true, 1, false, false);
}

/* To print array element */
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

/* To compare array elements */
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

/* To decode barcode */
void decodeBarcode()
{
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
    char asterisk[9] = {0, 1, 0, 0, 1, 0, 1, 0, 0};

    if (compareArray(A, barcode, 9) == 0)
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

static uint64_t get_time(void)
{
    // Reading low latches the high value
    uint32_t lo = timer_hw->timelr;
    uint32_t hi = timer_hw->timehr;
    return ((uint64_t)hi << 32u) | lo;
}
/// \end::get_time[]

/// \tag::alarm_standalone[]
// Use alarm 0
#define ALARM_NUM 0
#define ALARM_IRQ TIMER_IRQ_0

// Alarm interrupt handler
static volatile bool alarm_fired;

static void alarm_irq(void)
{
    // Clear the alarm irq
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM);

    // Assume alarm 0 has fired
    printf("Interrupt IRQ fired\n");
    alarm_fired = true;
}

static void alarm_in_us(uint32_t delay_us)
{
    // Enable the interrupt for our alarm (the timer outputs 4 alarm irqs)
    hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);
    // Set irq handler for alarm irq
    irq_set_exclusive_handler(ALARM_IRQ, alarm_irq);
    // Enable the alarm irq
    irq_set_enabled(ALARM_IRQ, true);
    // Enable interrupt in block and at processor

    // Alarm is only 32 bits so if trying to delay more
    // than that need to be careful and keep track of the upper
    // bits
    uint64_t target = timer_hw->timerawl + delay_us;

    // Write the lower 32 bits of the target time to the alarm which
    // will arm it
    timer_hw->alarm[ALARM_NUM] = (uint32_t)target;
}

void detectBar()
{
    uint adc_raw;
    static volatile float normalizedADCRes;
    volatile int lineCount = 0;
    bool lineDetected = false;
    int lineThreshold = 2000;

    absolute_time_t blackStartTime;
    absolute_time_t whiteEndTime;
    absolute_time_t blackEndTime;
    absolute_time_t whiteStartTime;
    int blackTime;
    int whiteTime;
    bool startOfBarcode = false;

    while (lineCount <= 9)
    {
        adc_raw = adc_read(); // raw voltage from ADC
        normalizedADCRes = adc_raw * ADC_CONVERT;
        printf("Raw value: %u, voltage: %.2f V\n", adc_raw, normalizedADCRes);

        alarm_fired = false;      // Set alarm every 2 seconds
        alarm_in_us(1000000 * 1); // alarm_in_us(1000);
        while (!alarm_fired)
            ; // Wait for alarm to fire

        if (startOfBarcode == false) // keep finding for the first white line
        {
            if (adc_raw < lineThreshold) // white
            {
                // start of the barcode
                startOfBarcode = true;
                printf("Next detect first black line\n");
            }
        }
        else if (startOfBarcode == true) // find for the first black line
        {
            printf("Start of barcode\n");           // first black line
            blackStartTime = get_absolute_time(); // record black start time
            whiteEndTime = get_absolute_time();   // record white end time
            blackTime = absolute_time_diff_us(blackEndTime, blackStartTime);
            printf("black %d microseconds\n", blackTime);

            if (lineDetected == false)
            {
                printf("detect black\n");
                if (adc_raw > lineThreshold) // black
                {
                    // add elements into array
                    for (int c = 9 - 1; c >= lineCount - 1; c--)
                    {
                        barcode[c + 1] = barcode[c];
                    }

                    // detect thin or thick bar
                    if (blackTime < whiteTime)
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
                if (adc_raw > 300 && adc_raw < lineThreshold) // white
                {
                    blackEndTime = get_absolute_time();
                    whiteStartTime = get_absolute_time(); // record white start time
                    whiteTime = absolute_time_diff_us(whiteEndTime, whiteStartTime);
                    printf("white %d microseconds\n", whiteTime);

                    // add elements into array
                    for (int c = 9 - 1; c >= lineCount - 1; c--)
                    {
                        barcode[c + 1] = barcode[c];
                    }

                    // detect thin or thick bar
                    if (whiteTime < blackTime)
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
                    lineDetected = false;
                    printf("%i white %d\n", lineCount, barcode[lineCount]);
                }
            }
            decodeBarcode();
        }
    }
}