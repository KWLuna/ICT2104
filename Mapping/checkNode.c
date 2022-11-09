//#include "pico/stdlib.h"

typedef struct Node
{
    int isVisited; // 0 false, 1 true
    
    // 0 = isWall, 1 = isGap, 2 = isTravelledGap
    int northIsWall;
    int southIsWall;
    int eastIsWall;
    int westIsWall;
} Node;

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
    Direction directionFacing;
} Car;

Direction GetLeftDirection(Direction frontDirection)
{
    switch(frontDirection)
    {
        case North:
            return West;
            break;
        case East:
            return North;
            break;
        case South:
            return East;
            break;
        case West:
            return South;
            break;
    }
}

Direction GetRightDirection(Direction frontDirection)
{
    switch(frontDirection)
    {
        case North:
            return East;
            break;
        case East:
            return South;
            break;
        case South:
            return West;
            break;
        case West:
            return North;
            break;
    }
}

Direction GetBackDirection(Direction frontDirection)
{
    switch(frontDirection)
    {
        case North:
            return South;
            break;
        case East:
            return West;
            break;
        case South:
            return North;
            break;
        case West:
            return East;
            break;
    }
}

// checks current node's walls
void CheckNode(int x, int y, Node gridArray[][10], Direction directionFacing)
{
    // if current node has not been visted
    if (gridArray[x][y].isVisited == 0)
    {
        // set node to visited
        gridArray[x][y].isVisited = 1;

        // check front, left, right sides for wall
        float frontUltrasonicDist = 0; // call function to get front ultrasonic distance
        float leftUltrasonicDist = 0; // call function to get left ultrasonic distance
        float rightUltrasonicDist = 0; // call function to get right ultrasonic distance

        MarkWall(x, y, gridArray, frontUltrasonicDist, directionFacing, directionFacing);
        MarkWall(x, y, gridArray, leftUltrasonicDist, directionFacing, GetLeftDirection(directionFacing));
        MarkWall(x, y, gridArray, rightUltrasonicDist, directionFacing, GetRightDirection(directionFacing));

        // set wall that car came from to isGap
        MarkWall(x, y, gridArray, 100.f, directionFacing, GetBackDirection(directionFacing));
    }
}

void MarkWall(int x, int y, Node gridArray[][10], float ultrasonicDistance, Direction ultrasonicDirection)
{
    // if ultrasonic value is more than distance, it is not a wall
    if (ultrasonicDistance > 9.f) // check how many cm is wall from car
    {
        // mark ultrasonic direction as gap
        switch(ultrasonicDirection)
        {
            case North:
                gridArray[x][y].northIsWall = 1;
                break;
            case East:
                gridArray[x][y].eastIsWall = 1;
                break;
            case South:
                gridArray[x][y].southIsWall = 1;
                break;
            case West:
                gridArray[x][y].westIsWall = 1;
                break;
        }
    }
    else    // it is a wall
    {
        // mark ultrasonic direction as wall
        switch(ultrasonicDirection)
        {
            case North:
                gridArray[x][y].northIsWall = 0;
                break;
            case East:
                gridArray[x][y].eastIsWall = 0;
                break;
            case South:
                gridArray[x][y].southIsWall = 0;
                break;
            case West:
                gridArray[x][y].westIsWall = 0;
                break;
        }
    }
}

int main()
{
    
}
