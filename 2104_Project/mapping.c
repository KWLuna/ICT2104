// #include "pico/stdlib.h"

#define NUMBER_OF_NODES 20
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
    Direction directionLeft;
    Direction directionRight;
    Direction directionBack;
} Car;

typedef struct Stack
{
    int x;
    int y;
} Stack;

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

// global so that its values are 0 at initialisation
Grid grid;  // The Map 
Car car;    // Our Car
int numNodeVisited = 0; // counter for number of nodes visited
int stackTop = -1;  // to track the top element of stack array
Stack dfsStack[NUMBER_OF_NODES];  // max 20 as there are 20 nodes
int carPrevX = CAR_START_X;
int carPrevY = CAR_START_Y;

// declare functions
void CheckCurrentNode();
void MoveCarToStackPos();
void SavePrevXYToCurrentNode();
void Push(int x, int y);
Stack Pop();
Stack Peek();
void AddStackIfNotExist(int x, int y);
void MarkWall(float ultrasonicDistance, Direction ultrasonicDirection);
void CheckUltrasonic();
void SetCar(Car *car, int xPos, int yPos, Direction direction);
void SetCarWallPointers();
void AddDirectionToStack(Direction directionToCheck);
void conversionConstructor(Node gridArray[4][5]);

int main()
{
    // Initialise Nodes prevX and prevY to 100
    for (int i = 0; i < 7; i++)
    {
        for (int p = 0; p < 9; p++)
        {
            grid.gridArray[i][p].prevX = 100;
            grid.gridArray[i][p].prevY = 100;
        }
    }

    //Set Car Starting Position and Direction
    SetCar(&car, CAR_START_X, CAR_START_Y, North);

    while (numNodeVisited < 20)
    {
        CheckCurrentNode();
        MoveCarToStackPos();
        SavePrevXYToCurrentNode();
    }
}

int OldMain()
{
    // Initialising Objects
    Car car;    // Our Car

    //Set Car Starting Position
    SetCar(&car, 3, 4, North);

    while (numNodeVisited < 20)
    {
        CheckNode(car.xCoord, car.yCoord, grid.gridArray, car.directionFacing); // mark current node
        
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
            // no need update car direction cuz reversing
            // SetCar(&car, car.xCoord, car.yCoord, currBack);
        }
    }

    return 0;
}

void CheckCurrentNode()
{
    // if not visited
    if (grid.gridArray[car.xCoord][car.yCoord].isVisited == 0)
    {
        grid.gridArray[car.xCoord][car.yCoord].isVisited = 1;
        numNodeVisited++;

        // mark out walls and gaps based on sensors
        CheckUltrasonic();

        SetCarWallPointers();   // set car's current node's wall pointers
        // check if surrounding nodes of car has nodes that have not been visited
        if (car.back == 1)   // if back isGap
        {
            // add node at back to stack
            AddDirectionToStack(car.directionBack);
        }
        if (car.left == 1)   // if left isGap
        {
            // add node at left to stack
            AddDirectionToStack(car.directionLeft);
        }
        if (car.right == 1)   // if right isGap
        {
            // add node at right to stack
            AddDirectionToStack(car.directionRight);
        }
        if (car.forward == 1)   // if front isGap
        {
            // add node at front to stack
            AddDirectionToStack(car.directionFacing);
        }
    }
}

void MoveCarToStackPos()
{

}

void SavePrevXYToCurrentNode()
{
    // if prevX and prevY has not been set before
    if (grid.gridArray[car.xCoord][car.yCoord].prevX == 100 && grid.gridArray[car.xCoord][car.yCoord].prevY == 100)
    {
        grid.gridArray[car.xCoord][car.yCoord].prevX = carPrevX;
        grid.gridArray[car.xCoord][car.yCoord].prevY = carPrevY;
    }
}

// push element to top of stack
void Push(int x, int y)
{
    if (stackTop == NUMBER_OF_NODES - 1)
        printf("Stack is full!");
    else
    {
        stackTop++;
        dfsStack[stackTop].x = x;
        dfsStack[stackTop].y = y;
    }
}

// pop element from top of stack and return element
Stack Pop()
{
    if (stackTop == -1)
        printf("Stack is empty!");
    else
    {
        stackTop--;
        return dfsStack[stackTop-1];
    }
}

// view element at top of stack
Stack Peek()
{
    if (stackTop == -1)
        printf("Stack is empty!");
    else
        return dfsStack[stackTop];
}

// Add node to stack if it does not exist in stack
void AddStackIfNotExist(int x, int y)
{
    int isInStack = 0;
    // check through stack for x y
    for (int i = stackTop; i > -1; i--)
    {
        if (dfsStack[i].x == x && dfsStack[i].y == y)
        {
            // is in stack
            isInStack = 1;
            break;
        }
    }

    // if not in stack, push
    if (isInStack == 0)
        Push(x, y);
}

void MarkWall(float ultrasonicDistance, Direction ultrasonicDirection)
{
    // if ultrasonic value is more than distance, it is not a wall
    if (ultrasonicDistance > 13.5f) // check how many cm is wall from car (one grid is 27cm)
    {
        // mark ultrasonic direction as gap
        switch(ultrasonicDirection)
        {
            case North:
                grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
                break;
            case East:
                grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
                break;
            case South:
                grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
                break;
            case West:
                grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
                break;
        }
    }
    else    // it is a wall
    {
        // mark ultrasonic direction as wall
        switch(ultrasonicDirection)
        {
            case North:
                grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
                break;
            case East:
                grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
                break;
            case South:
                grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
                break;
            case West:
                grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
                break;
        }
    }
}

// checks current node's walls
void CheckUltrasonic()
{
    // check front, left, right, back sides for wall
    float frontUltrasonicDist = 2.f; // call function to get front ultrasonic distance <to be done>
    float rightUltrasonicDist = 20.f; // call function to get right ultrasonic distance <to be done>
    float leftUltrasonicDist = 20.f; // call function to get left ultrasonic distance <to be done>
    float backUltrasonicDist = 20.f; // call function to get back ultrasonic distance <to be done>

    MarkWall(frontUltrasonicDist, car.directionFacing);
    MarkWall(rightUltrasonicDist, GetRightDirection(car.directionFacing));
    MarkWall(leftUltrasonicDist, GetLeftDirection(car.directionFacing));
    MarkWall(backUltrasonicDist, GetBackDirection(car.directionFacing));
}

void SetCar(Car *car, int xPos, int yPos, Direction direction)
{
    car->xCoord = xPos;
    car->yCoord = yPos;
    car->directionFacing = direction;
}

void SetCarWallPointers()
{
    Node currNode = grid.gridArray[car.xCoord][car.yCoord];

    // set car forward, right, left and back pointers to the node's wall
    switch (car.directionFacing)
        {
        case North:
            car.forward = currNode.northIsWall;
            car.right = currNode.eastIsWall;
            car.left = currNode.westIsWall;
            car.back = currNode.southIsWall;
            car.directionRight = East;
            car.directionLeft = West;
            car.directionBack = South;
            break;
        case East:
            car.forward = currNode.eastIsWall;
            car.right = currNode.southIsWall;
            car.left = currNode.northIsWall;
            car.back = currNode.westIsWall;
            car.directionRight = South;
            car.directionLeft = North;
            car.directionBack = West;
            break;
        case West:
            car.forward = currNode.westIsWall;
            car.right = currNode.northIsWall;
            car.left = currNode.southIsWall;
            car.back = currNode.eastIsWall;
            car.directionRight = North;
            car.directionLeft = South;
            car.directionBack = East;
            break;
        case South:
            car.forward = currNode.southIsWall;
            car.right = currNode.westIsWall;
            car.left = currNode.eastIsWall;
            car.back = currNode.northIsWall;
            car.directionRight = West;
            car.directionLeft = East;
            car.directionBack = North;
            break;
        default:
            break;
        }
}

// add the node at a direction of car to stack
void AddDirectionToStack(Direction directionToCheck)
{
    Node nodePtr;
    int nodeX;
    int nodeY;

    // get the node at back of car
    switch (directionToCheck)
    {
    case North:
        nodeX = car.xCoord - 1;
        nodeY = car.yCoord;
        nodePtr = grid.gridArray[nodeX][nodeY];
        break;
    case South:
        nodeX = car.xCoord + 1;
        nodeY = car.yCoord;
        nodePtr = grid.gridArray[nodeX][nodeY];
        break;
    case East:
        nodeX = car.xCoord;
        nodeY = car.yCoord + 1;
        nodePtr = grid.gridArray[nodeX][nodeY];
        break;
    case West:
        nodeX = car.xCoord;
        nodeY = car.yCoord - 1;
        nodePtr = grid.gridArray[nodeX][nodeY];
        break;
    }

    // if node has not been visited
    if (nodePtr.isVisited == 0)
        AddStackIfNotExist(nodeX, nodeY); // add to stack
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