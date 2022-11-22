// #include "pico/stdlib.h"

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
} Node;

// Grid contains all Node information
typedef struct Grid
{
    Node gridArray[7][9];
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

//array used by navigation code
int navigationArray[9][11];

void MarkWall(int x, int y, Node gridArray[][9], float ultrasonicDistance, Direction ultrasonicDirection)
{
    // if ultrasonic value is more than distance, it is not a wall
    if (ultrasonicDistance > 13.5f) // check how many cm is wall from car (one grid is 27cm)
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
void CheckNode(int x, int y, Node gridArray[][9], Direction directionFacing)
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

//convert 4x5 into 9x11 
//conversion guide: (lengthx2) + 1
void conversionConstructor(Node gridArray[4][5])
{
    //initialization node centers
    //0 = wall, 1 = space
    
    for(int x=0; x<9;x++)
    {
        for(int y=0; y<11; y++)
        {
            navigationArray[x][y]=0;
        }
    }

    //constructing navigation array map
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<5; j++)
        {
            int refRow = (i*2)+1;
            int refCol = (j*2)+1;

            //blocking out diagonals for each node
            navigationArray[refRow-1][refCol-1]=1;
            navigationArray[refRow-1][refCol+1]=1;
            navigationArray[refRow+1][refCol-1]=1;
            navigationArray[refRow+1][refCol+1]=1;

            //creating blocked off walls
            if(gridArray[i][j].northIsWall == 0)
            {
               navigationArray[refRow-1][refCol]=1;
            }
            if(gridArray[i][j].southIsWall == 0)
            {
               navigationArray[refRow+1][refCol]=1;
            }
            if(gridArray[i][j].eastIsWall == 0)
            {
               navigationArray[refRow][refCol-1]=1;
            }
            if(gridArray[i][j].westIsWall == 0)
            {
               navigationArray[refRow][refCol+1]=1;
            }
        }
    }
}

// The Map
Grid grid;  // map is global so that its values are 0 at initialisation

int main()
{
    // Initialising Objects
    Car car;    // Our Car

    //Set Car Starting Position
    SetCar(&car, 3, 4, North);

    // counter for number of nodes visited
    int numNodeVisited = 0;

    while (numNodeVisited < 20)
    {
        CheckNode(car.xCoord, car.yCoord, grid.gridArray, car.directionFacing); // mark current node

        numNodeVisited++; // add 1 to number of nodes visited
        
        Node *node = &grid.gridArray[car.xCoord][car.yCoord];   // pointer of current node

        //int *forward, *right, *left, *back;
        Direction currLeft, currRight, currBack;
        switch (car.directionFacing)
        {
        case North:
            car.forward = &node->northIsWall;
            car.right = &node->eastIsWall;
            car.left = &node->westIsWall;
            car.back = &node->southIsWall;
            currRight = East;
            currLeft = West;
            currBack = South;
            break;
        case East:
            car.forward = &node->eastIsWall;
            car.right = &node->southIsWall;
            car.left = &node->northIsWall;
            car.back = &node->westIsWall;
            currRight = South;
            currLeft = North;
            currBack = West;
            break;
        case West:
            car.forward = &node->westIsWall;
            car.right = &node->northIsWall;
            car.left = &node->southIsWall;
            car.back = &node->eastIsWall;
            currRight = North;
            currLeft = South;
            currBack = East;
            break;
        case South:
            car.forward = &node->southIsWall;
            car.right = &node->westIsWall;
            car.left = &node->eastIsWall;
            car.back = &node->northIsWall;
            currRight = West;
            currLeft = East;
            currBack = North;
            break;
        default:
            break;
        }

        if (*car.forward == 1)
        {
            *car.forward = 2;   // set to isTravelled

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
        else if (*car.right == 1)
        {
            *car.right = 2;   // set to isTravelled

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
        else if (*car.left == 1)
        {
            *car.left = 2;   // set to isTravelled

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
        else if (*car.back == 1)
        {
            *car.back = 2;   // set to isTravelled

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
