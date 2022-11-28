#ifndef MAPPING_H
#define MAPPING_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "motor.h"
#include "ultrasonic.h"

#define NUMBER_OF_NODES 20
#define MAPPING_GRID_SIZE_X 7
#define MAPPING_GRID_SIZE_Y 9
#define CAR_START_X 3
#define CAR_START_Y 4

// Node is used by Grid
typedef struct Node
{
    // unsigned int used to save space, only 1 bit used.
    unsigned int isVisited : 1; // 0 = false, 1 = true

    // 0 = isWall, 1 = isGap, 2 = isTravelledGap
    int northIsWall;
    int southIsWall;
    int eastIsWall;
    int westIsWall;

    // to store first node xy that car entered node from
    int prevX;
    int prevY;
} Node;

// Grid contains all Node information
typedef struct Grid
{
    Node gridArray[MAPPING_GRID_SIZE_X][MAPPING_GRID_SIZE_Y];
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

    // pointers to node walls that car is in
    int *forward;
    int *right;
    int *left;
    int *back;

    Direction directionFacing;
    Direction directionLeft;
    Direction directionRight;
    Direction directionBack;
} Car;

typedef struct Stack
{
    int x;
    int y;
} Stack;

typedef struct Coordinate
{
    int row;
    int col;

} Coordinate;

// extern
extern Grid grid;                       // The Map
extern Car car;                         // Our Car
extern int numNodeVisited;              // counter for number of nodes visited
extern int stackTop;                    // to track the top element of stack array
extern Stack dfsStack[NUMBER_OF_NODES]; // Store Node to of where the stack can travel to but has not visited
extern int carPrevX;
extern int carPrevY;
extern Node finalMapArray[4][5];

// navigation
extern int carRow;
extern int carCol;
extern int destRow;
extern int destCol;
extern int backfill;
extern uint8_t navigationArray[9][11];
extern bool visitedArray[9][11];
extern bool exitFound;
extern Coordinate movementList[90];

// declare functions
void MappingMain();
void CheckCurrentNode();
void MoveCarToStackPos();
void SavePrevXYToCurrentNode();
void Backtrack();
void Push(int x, int y);
Stack Pop();
Stack Peek();
void AddStackIfNotExist(int x, int y);
void MarkWall(float ultrasonicDistance, Direction ultrasonicDirection);
void CheckUltrasonic();
void SetCar(Car *car, int xPos, int yPos, Direction direction);
void SetCarWallPointers();
void AddDirectionToStack(Direction directionToCheck);
Direction CheckDirectionOfNode(int nodeXPos, int nodeYPos);
void ChangeCarDirection(Direction directionToGo, int xPosToGo, int yPosToGo);
Direction GetLeftDirection(Direction frontDirection);
Direction GetRightDirection(Direction frontDirection);
Direction GetBackDirection(Direction frontDirection);
void ConvertMappedGrid();

// navigation
void setCoord(int carrow, int carcol, int destrow, int destcol);
void conversionConstructor(Node gridArray[4][5]);
bool validMove(uint8_t navigationArray[9][11], bool visitedArray[9][11], int newRow, int newCol);
bool navigateTo(uint8_t navigationArray[9][11], bool visitedArray[9][11], int currRow, int currCol);
void targetLocator(bool exitFound, Coordinate movementList[90], int backfill);

// comms
void receiveCoordinate();

#endif
