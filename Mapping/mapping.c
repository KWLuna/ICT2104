/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// #include "pico/stdlib.h"

// Node is used by Grid
typedef struct Node
{
    // unsigned int is replacing boolean cz c does not have boolean data type. 
    // 0 = false, 1 = true
    unsigned int isVisited:1; 
    int northIsWall;
    int southIsWall;
    int eastIsWall;
    int westIsWall;
} Node;

// Grid contains all Node information
typedef struct Grid
{
    Node gridArray[8][10];
} Grid;

typedef enum Direction
{
    North,
    East,
    South,
    West
} Direction;

typedef struct Car
{
    int xCoord;
    int yCoord;
    Direction directionFacing;
} Car;

void SetCar(Car *car, int xPos, int yPos, Direction direction)
{
    car->xCoord = xPos;
    car->yCoord = yPos;
    car->directionFacing = direction;
}

void CheckNode(int xCoord, int yCoord, Node *gridArray)
{
    //dummy
    grid.gridArray[car.xCoord][car.yCoord].isVisited = 1;
    grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
    grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
    grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
    grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;

}

int main() 
{
    // Initialising Objects
    Car car;    // Our Car
    Grid grid;  // The Map

   
    //Set Car Starting Position
    SetCar(&car, 4, 5, North);

    while (1)
    {
        CheckNode(car.xCoord, car.yCoord, grid.gridArray);
        
        Node node = grid.gridArray[car.xCoord][car.yCoord];

        int forward, right, left, back;
        Direction currLeft, currRight;
        switch (car.directionFacing)
        {
        case North:
            forward = node.northIsWall;
            right = node.eastIsWall;
            left = node.westIsWall;
            back = node.southIsWall;
            currRight = East;
            currLeft = West;
            break;
        case East:
            forward = node.eastIsWall;
            right = node.southIsWall;
            left = node.northIsWall;
            back = node.westIsWall;
            currRight = South;
            currLeft = North;
            break;
        case West:
            forward = node.westIsWall;
            right = node.northIsWall;
            left = node.southIsWall;
            back = node.eastIsWall;
            currRight = North;
            currLeft = South;
            break;
        case South:
            forward = node.southIsWall;
            right = node.westIsWall;
            left = node.eastIsWall;
            back = node.northIsWall;
            currRight = West;
            currLeft = East;
            break;
        default:
            break;
        }

        if(forward == 1)
        {
            forward = 2;
            // Travel Forward <to be done>

        }
        else if (right == 1)
        {
            right = 2;
            // Travel Right <to be done>
            SetCar(&car, car.xCoord, car.yCoord, currRight);
        }
        else if(left == 1)
        {
            left = 2;
            // Travel Left <to be done>
            SetCar(&car, car.xCoord, car.yCoord, currLeft);
        }
        else if (back == 1)
        {
            back = 2;
            // reverse 1 Node <to be Done>

        }

    }

    return 0;
}
