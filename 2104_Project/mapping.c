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


//array used by navigation code
int navigationArray[9][11];

// global so that its values are 0 at initialisation
Grid grid;  // The Map 
Car car;    // Our Car
int numNodeVisited = 0; // counter for number of nodes visited
int stackTop = -1;  // to track the top element of stack array

// Stack Stores a max of 20 as there are 20 nodes
Stack dfsStack[NUMBER_OF_NODES]; // Store Node to of where the stack can travel to but has not visited
int carPrevX = CAR_START_X;
int carPrevY = CAR_START_Y;

// declare functions
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
    //SetCar(&car, 2, 0, North);    // for testing

    while (1)
    {
        CheckCurrentNode();
        if (numNodeVisited < 20)
        {
            MoveCarToStackPos();
            SavePrevXYToCurrentNode();
        }
        else
            break;
    }
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
        if (*car.back == 1)   // if back isGap
        {
            // add node at back to stack
            AddDirectionToStack(car.directionBack);
        }
        if (*car.left == 1)   // if left isGap
        {
            // add node at left to stack
            AddDirectionToStack(car.directionLeft);
        }
        if (*car.right == 1)   // if right isGap
        {
            // add node at right to stack
            AddDirectionToStack(car.directionRight);
        }
        if (*car.forward == 1)   // if front isGap
        {
            // add node at front to stack
            AddDirectionToStack(car.directionFacing);
        }
    }
}

void MoveCarToStackPos()
{
    // Save Car Current Node Position
    carPrevX = car.xCoord;
    carPrevY = car.yCoord;

    // Check for Last Stack Position in dfsStack
    // Checking North
    if (carPrevX - 1 == Peek().x && carPrevY == Peek().y)
    {
        //Node tempNode = grid.gridArray[carPrevX - 1][carPrevY];
        if (grid.gridArray[carPrevX][carPrevY].northIsWall == 1) // Checking if North Wall is a Gap
        {
            ChangeCarDirection(North, carPrevX - 1, carPrevY);
            // Pop from dfsStack[]
            Pop();
        }
        else
            Backtrack();
    }
    // Checking South
    else if (carPrevX + 1 == Peek().x && carPrevY == Peek().y)
    {
        //Node tempNode = grid.gridArray[carPrevX + 1][carPrevY];
        if(grid.gridArray[carPrevX][carPrevY].southIsWall == 1) // Checking if North Wall is a Gap
        {
            ChangeCarDirection(South, carPrevX + 1, carPrevY);
            // Pop dfsStack[]
            Pop();
        }
        else
            Backtrack();
    }
    // Checking East
    else if (carPrevX  == Peek().x && carPrevY + 1 == Peek().y)
    {
        //Node tempNode = grid.gridArray[carPrevX][carPrevY + 1];
        if (grid.gridArray[carPrevX][carPrevY].eastIsWall == 1) // Checking if North Wall is a Gap
        {
            // check current direction car facing
            ChangeCarDirection(East, carPrevX, carPrevY + 1);
            
            // Pop dfsStack[]
            Pop();
        }
        else
            Backtrack();
    }
    // Checking West
    else if (carPrevX == Peek().x && carPrevY - 1 == Peek().y)
    {
        //Node tempNode = grid.gridArray[carPrevX][carPrevY - 1];
        if (grid.gridArray[carPrevX][carPrevY].westIsWall == 1) // Checking if North Wall is a Gap
        {
            // check current direction car facing
            ChangeCarDirection(West, carPrevX, carPrevY - 1);
            
            // Pop dfsStack[]
            Pop();
        }
        else
            Backtrack();
    }
    else
        Backtrack();
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

void Backtrack()
{
    // BackTrack!
    // Check the node location Whether it is north, south, east, west of the car
    Direction directionOfNode = CheckDirectionOfNode(grid.gridArray[carPrevX][carPrevY].prevX, grid.gridArray[carPrevX][carPrevY].prevY);
    ChangeCarDirection(directionOfNode, grid.gridArray[carPrevX][carPrevY].prevX, grid.gridArray[carPrevX][carPrevY].prevY);
}

// push element to top of stack
void Push(int x, int y)
{
    if (stackTop == NUMBER_OF_NODES - 1)
        printf("Stack is full!\n");
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
        printf("Stack is empty!\n");
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

// void TempMarkWall(float ultrasonicDistance, Direction ultrasonicDirection)
// {
//     if (car.xCoord == 0 && car.yCoord == 0)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
//     }
//     else if (car.xCoord == 0 && car.yCoord == 1)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
//     }
//     else if (car.xCoord == 0 && car.yCoord == 2)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
//     }
//     else if (car.xCoord == 0 && car.yCoord == 3)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
//     }
//     else if (car.xCoord == 0 && car.yCoord == 4)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
//     }
//     else if (car.xCoord == 1 && car.yCoord == 0)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
//     }
//     else if (car.xCoord == 1 && car.yCoord == 1)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
//     }
//     else if (car.xCoord == 1 && car.yCoord == 2)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
//     }
//     else if (car.xCoord == 1 && car.yCoord == 3)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
//     }
//     else if (car.xCoord == 1 && car.yCoord == 4)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
//     }
//     else if (car.xCoord == 2 && car.yCoord == 0)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
//     }
//     else if (car.xCoord == 2 && car.yCoord == 1)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
//     }
//     else if (car.xCoord == 2 && car.yCoord == 2)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
//     }
//     else if (car.xCoord == 2 && car.yCoord == 3)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
//     }
//     else if (car.xCoord == 2 && car.yCoord == 4)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
//     }
//     else if (car.xCoord == 3 && car.yCoord == 0)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
//     }
//     else if (car.xCoord == 3 && car.yCoord == 1)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
//     }
//     else if (car.xCoord == 3 && car.yCoord == 2)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
//     }
//     else if (car.xCoord == 3 && car.yCoord == 3)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
//     }
//     else if (car.xCoord == 3 && car.yCoord == 4)
//     {
//         grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
//         grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
//         grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
//     }
// }

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

    printf("x:%d, y:%d\n", car->xCoord, car->yCoord);
}

void SetCarWallPointers()
{
    // set car forward, right, left and back pointers to the node's wall
    switch (car.directionFacing)
        {
        case North:
            car.forward = &grid.gridArray[car.xCoord][car.yCoord].northIsWall;
            car.right = &grid.gridArray[car.xCoord][car.yCoord].eastIsWall;
            car.left = &grid.gridArray[car.xCoord][car.yCoord].westIsWall;
            car.back = &grid.gridArray[car.xCoord][car.yCoord].southIsWall;
            car.directionRight = East;
            car.directionLeft = West;
            car.directionBack = South;
            break;
        case East:
            car.forward = &grid.gridArray[car.xCoord][car.yCoord].eastIsWall;
            car.right = &grid.gridArray[car.xCoord][car.yCoord].southIsWall;
            car.left = &grid.gridArray[car.xCoord][car.yCoord].northIsWall;
            car.back = &grid.gridArray[car.xCoord][car.yCoord].westIsWall;
            car.directionRight = South;
            car.directionLeft = North;
            car.directionBack = West;
            break;
        case West:
            car.forward = &grid.gridArray[car.xCoord][car.yCoord].westIsWall;
            car.right = &grid.gridArray[car.xCoord][car.yCoord].northIsWall;
            car.left = &grid.gridArray[car.xCoord][car.yCoord].southIsWall;
            car.back = &grid.gridArray[car.xCoord][car.yCoord].eastIsWall;
            car.directionRight = North;
            car.directionLeft = South;
            car.directionBack = East;
            break;
        case South:
            car.forward = &grid.gridArray[car.xCoord][car.yCoord].southIsWall;
            car.right = &grid.gridArray[car.xCoord][car.yCoord].westIsWall;
            car.left = &grid.gridArray[car.xCoord][car.yCoord].eastIsWall;
            car.back = &grid.gridArray[car.xCoord][car.yCoord].northIsWall;
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
    int nodeX = 0;
    int nodeY = 0;

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

Direction CheckDirectionOfNode(int nodeXPos, int nodeYPos)
{
    // Save Car Current Node Position
    int carX = car.xCoord;
    int carY = car.yCoord;

    if (carX - 1 == nodeXPos)
        return North;
    else if (carX + 1 == nodeXPos)
        return South;
    else if (carY + 1 == nodeYPos)
        return East;
    else if (carY - 1 == nodeYPos)
        return West;
}

void ChangeCarDirection(Direction directionToGo, int xPosToGo, int yPosToGo)
{
    if (directionToGo == car.directionFacing)
    {
        // Travel Forward
        SetCar(&car, xPosToGo, yPosToGo, car.directionFacing);
    }
    else if (GetBackDirection(directionToGo) == car.directionFacing)
    {
        // Reverse
        SetCar(&car, xPosToGo, yPosToGo, car.directionFacing);
    }
    else if (directionToGo == GetRightDirection(car.directionFacing))
    {
        // Turn Right
        SetCar(&car, xPosToGo, yPosToGo, GetRightDirection(car.directionFacing));
    }
    else if (directionToGo == GetLeftDirection(car.directionFacing))
    {
        // Turn Left
        SetCar(&car, xPosToGo, yPosToGo, GetLeftDirection(car.directionFacing));
    }
}

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