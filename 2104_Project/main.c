/* Includes */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "motor.h"
#include "barcode.h"
#include "encoder.h"
#include "mapping.h"
#include "ultrasonic.h"
#include "comms.h"

#define LEFT_ENCODER_PIN 15
#define RIGHT_ENCODER_PIN 14

// Mapping
// int numNodeVisited = 0;          // counter for number of nodes visited
// int stackTop = -1;               // to track the top element of stack array
// int carPrevX = CAR_START_X;
// int carPrevY = CAR_START_Y;

/* Main program */
int main()
{
    stdio_init_all(); // Enable UART so we can print status output

    init_motor();      // initialize motor
    init_pins();       // initialize pins for encoder
    init_ultrasonic(); // initialize pins for ultrasonic
    struct repeating_timer timer;
    add_repeating_timer_ms(100, ISR_PID, NULL, &timer);   // set pid timer for every 100ms
    gpio_set_irq_enabled_with_callback(RIGHT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true, gpio_callback);
    gpio_set_irq_enabled_with_callback(LEFT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true, gpio_callback);

    moveBackwardBY(60);

    // MappingMain();

    // multidimensional array is defined row X column / vert X hori
    // in this case access goes from 0,0 to 9,8
    // NOTE: convert array coordinate to grid based because coordinate is x,y while array is y,x
    // there is also absolute and relative coordinates to consider with reference to google sheets

    // // grid instantiation on startup
    // for (int i = 0; i < 4; i++)
    //     for (int j = 0; j < 5; j++)
    //     {
    //         visitGrid[i][j].isVisited = false;
    //         visitGrid[i][j].northIsWall = -1;
    //         visitGrid[i][j].eastIsWall = -1;
    //         visitGrid[i][j].southIsWall = -1;
    //         visitGrid[i][j].westIsWall = -1;
    //     }

    // visitGrid[0][0].westIsWall = 0;
    // visitGrid[0][0].northIsWall = 0;

    // visitGrid[0][1].northIsWall = 0;

    // visitGrid[0][2].northIsWall = 0;

    // visitGrid[0][3].northIsWall = 0;

    // visitGrid[0][4].northIsWall = 0;
    // visitGrid[0][4].eastIsWall = 0;

    // // uncomment this line and comment away the preconstructed array below when mapping is confirm a-go
    // // conversionConstructor(visitGrid);

    // int navigationArray[9][11] =
    //     {
    //         // sample map, go from 1,1 to 1,9
    //         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    //         1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1,
    //         1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1,
    //         1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1,
    //         1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1,
    //         1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1,
    //         1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1,
    //         1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    //         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    // // visualization of navmap
    // for (int x = 0; x < 9; x++)
    // {
    //     printf("\n");
    //     for (int y = 0; y < 11; y++)
    //     {
    //         printf(" %d", navigationArray[x][y]);
    //     }
    // }

    // // allow custom coordinate here once we fix with other teams
    // destRow = 4;
    // destCol = 1;

    // // allow custom coordinate here once we fix with other teams
    // carRow = 1;
    // carCol = 1;

    // navigateTo(navigationArray, visitedArray, carRow, carCol);

    // printf("\n");
    // // printing travelled bool map
    // for (int x = 0; x < 9; x++)
    // {
    //     printf("\n");
    //     for (int y = 0; y < 11; y++)
    //     {
    //         if (visitedArray[x][y] == true)
    //             printf(" T");
    //         if (visitedArray[x][y] == false)
    //             printf(" F");
    //     }
    // }

    // if (found)
    // {
    //     printf("\nEXIT FOUND!");
    //     printf("\nTOTAL STEPS: %d", backfill - 1);
    // }
    // else
    //     printf("\nNO ROUTE POSSIBLE!");

    // // debug lines
    // printf("\nX:%d, Y:%d", movementList[0].col, movementList[0].row);
    // printf("\nX:%d, Y:%d", movementList[1].col, movementList[1].row);
    // printf("\nX:%d, Y:%d", movementList[2].col, movementList[2].row);
    // printf("\nX:%d, Y:%d", movementList[3].col, movementList[3].row);

    // if (backfill == 2)
    // {
    // }

    // for (int i = 0; i < backfill; i++)
    // {
    //     if (i == 0)
    //     {
    //         printf("\nStarting location: X:%d, Y:%d", movementList[backfill - 1].col, movementList[backfill - 1].row);
    //         printf("\nTarget Location: X:%d, Y:%d", destCol, destRow);
    //         continue;
    //     }

    //     // this means that the row increased by one, aka the coordinate moved down
    //     if (movementList[backfill - i - 1].row == movementList[backfill - i].row + 1)
    //     {
    //         // move car IRL southwards half a grid worth of distance
    //         printf("\nDOWN");
    //     }
    //     // this means that the row decreased by one, aka the coordinate moved up
    //     else if (movementList[backfill - i - 1].row == movementList[backfill - i].row - 1)
    //     {
    //         // move car IRL northwards half a grid worth of distance
    //         printf("\nUP");
    //     }
    //     // this means that the col increased by one, aka the coordinate moved right
    //     else if (movementList[backfill - i - 1].col == movementList[backfill - i].col + 1)
    //     {
    //         // move car IRL eastwards half a grid worth of distance
    //         printf("\nRIGHT");
    //     }
    //     // this means that the col decreased by one, aka the coordinate moved left
    //     else if (movementList[backfill - i - 1].col == movementList[backfill - i].col - 1)
    //     {
    //         // move car IRL westwards half a grid worth of distance
    //         printf("\nLEFT");
    //     }
    // }

    while (1)
    {
        tight_loop_contents();
    };

    return 0;
}