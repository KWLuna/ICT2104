// #include "pico/stdlib.h"

// Node is used by Grid
typedef struct Node
{
    // unsigned int is replacing boolean cz c does not have boolean data type. 
    
    // 0 = false, 1 = true
    unsigned int isVisited:1; 

    // 0 = isWall, 1 = isGap, 2 = isTravelledGap
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

void MarkWall(int x, int y, Node gridArray[][10], float ultrasonicDistance, Direction ultrasonicDirection)
{
    // if ultrasonic value is more than distance, it is not a wall
    if (ultrasonicDistance > 18.f) // check how many cm is wall from car (one grid is 27cm)
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

// checks current node's walls
void CheckNode(int x, int y, Node gridArray[][10], Direction directionFacing)
{
    // if current node has not been visted
    if (gridArray[x][y].isVisited == 0)
    {
        // set node to visited
        gridArray[x][y].isVisited = 1;

        // check front, left, right sides for wall
        float frontUltrasonicDist = 2.f; // call function to get front ultrasonic distance <to be done>
        float leftUltrasonicDist = 20.f; // call function to get left ultrasonic distance <to be done>
        float rightUltrasonicDist = 20.f; // call function to get right ultrasonic distance <to be done>

        MarkWall(x, y, gridArray, frontUltrasonicDist, directionFacing);
        MarkWall(x, y, gridArray, leftUltrasonicDist, GetLeftDirection(directionFacing));
        MarkWall(x, y, gridArray, rightUltrasonicDist, GetRightDirection(directionFacing));

        // set wall that car came from to isGap
        MarkWall(x, y, gridArray, 100.f, GetBackDirection(directionFacing));
    }
}

void SetCar(Car *car, int xPos, int yPos, Direction direction)
{
    car->xCoord = xPos;
    car->yCoord = yPos;
    car->directionFacing = direction;
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
        CheckNode(car.xCoord, car.yCoord, grid.gridArray, car.directionFacing);
        
        Node *node = &grid.gridArray[car.xCoord][car.yCoord];

        int *forward, *right, *left, *back;
        Direction currLeft, currRight, currBack;
        switch (car.directionFacing)
        {
        case North:
            forward = &node->northIsWall;
            right = &node->eastIsWall;
            left = &node->westIsWall;
            back = &node->southIsWall;
            currRight = East;
            currLeft = West;
            currBack = South;
            break;
        case East:
            forward = &node->eastIsWall;
            right = &node->southIsWall;
            left = &node->northIsWall;
            back = &node->westIsWall;
            currRight = South;
            currLeft = North;
            currBack = West;
            break;
        case West:
            forward = &node->westIsWall;
            right = &node->northIsWall;
            left = &node->southIsWall;
            back = &node->eastIsWall;
            currRight = North;
            currLeft = South;
            currBack = East;
            break;
        case South:
            forward = &node->southIsWall;
            right = &node->westIsWall;
            left = &node->eastIsWall;
            back = &node->northIsWall;
            currRight = West;
            currLeft = East;
            currBack = North;
            break;
        default:
            break;
        }

        if (*forward == 1)
        {
            *forward = 2;   // set to isTravelled

            // Travel Forward <to be done>
            

            // update grid x y
            switch(car.directionFacing)
            {
                case North:
                    car.yCoord++;
                    break;
                case East:
                    car.xCoord++;
                    break;
                case West:
                    car.xCoord--;
                    break;
                case South:
                    car.yCoord--;
                    break;
            }    
        }
        else if (*right == 1)
        {
            *right = 2;   // set to isTravelled

            // Travel Right <to be done>


            // update grid x y
            switch(car.directionFacing)
            {
                case North:
                    car.xCoord++;
                    break;
                case East:
                    car.yCoord--;
                    break;
                case West:
                    car.yCoord++;
                    break;
                case South:
                    car.xCoord--;
                    break;
            }
            // update car direction
            SetCar(&car, car.xCoord, car.yCoord, currRight);
        }
        else if (*left == 1)
        {
            *left = 2;   // set to isTravelled

            // Travel Left <to be done>


            // update grid x y
            switch(car.directionFacing)
            {
                case North:
                    car.xCoord--;
                    break;
                case East:
                    car.yCoord++;
                    break;
                case West:
                    car.yCoord--;
                    break;
                case South:
                    car.xCoord++;
                    break;
            }
            // update car direction
            SetCar(&car, car.xCoord, car.yCoord, currLeft);
        }
        else if (*back == 1)
        {
            *back = 2;   // set to isTravelled

            // reverse 1 Node <to be Done>


            // update grid x y
            switch(car.directionFacing)
            {
                case North:
                    car.yCoord--;
                    break;
                case East:
                    car.xCoord--;
                    break;
                case West:
                    car.xCoord++;
                    break;
                case South:
                    car.yCoord++;
                    break;
            }
            // update car direction
            SetCar(&car, car.xCoord, car.yCoord, currBack);
        }
    }

    return 0;
}
