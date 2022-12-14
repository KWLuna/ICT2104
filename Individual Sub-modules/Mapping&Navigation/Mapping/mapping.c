#include "mapping.h"

// Mapping
Grid grid;                       // The Map
Car car;                         // Our Car
int numNodeVisited = 0;          // counter for number of nodes visited
int stackTop = -1;               // to track the top element of stack array
Stack dfsStack[NUMBER_OF_NODES]; // Store Node to of where the stack can travel to but has not visited
int carPrevX = CAR_START_X;
int carPrevY = CAR_START_Y;
Node finalMapArray[4][5];

// Navigation
int carRow;
int carCol;
int destRow;
int destCol;
int backfill;
int navigationArray[9][11];
int visitedArray[9][11];
int exitFound;
Coordinate movementList[90];

// Comms
int dataAvailable = 0; // data is read from buffer
int coords[2];          // store 2 bytes from buffer
int x, y;               // separate coords array

int main()
{
    // Initialise Nodes prevX and prevY to 100
    for (int i = 0; i < MAPPING_GRID_SIZE_X; i++)
    {
        for (int p = 0; p < MAPPING_GRID_SIZE_Y; p++)
        {
            grid.gridArray[i][p].prevX = 100;
            grid.gridArray[i][p].prevY = 100;
        }
    }

    // Set Car Starting Position and Direction
    SetCar(&car, CAR_START_X, CAR_START_Y, North);

    while (1)
    {
        CheckCurrentNode();
        if (numNodeVisited < 20)
        {
            MoveCarToStackPos();
            SavePrevXYToCurrentNode();
        }
        else if (numNodeVisited == 20)
            break;
    }

    // converts map to 4x5 at the end of mapping
    ConvertMappedGrid();

    // to check the contents of 4x5 finalMapArray
    // for (int x = 0; x < 4; x++)
    // {
    //     for (int y = 0; y < 5; y++)
    //     {
    //         printf("%d%d%d%d\n", finalMapArray[x][y].northIsWall, finalMapArray[x][y].eastIsWall, finalMapArray[x][y].southIsWall, finalMapArray[x][y].westIsWall);
    //     }
    // }

    // convert mapping array to navigation array
    conversionConstructor(finalMapArray);

    // to check the contents of 4x5 finalMapArray
    // for (int x = 0; x < 9; x++)
    // {
    //     for (int y = 0; y < 11; y++)
    //     {
    //         printf("%d\n", navigationArray[x][y]);
    //     }
    // }

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

        SetCarWallPointers(); // set car's current node's wall pointers
        // check if surrounding nodes of car has nodes that have not been visited
        if (*car.back == 1) // if back isGap
        {
            // add node at back to stack
            AddDirectionToStack(car.directionBack);
        }
        if (*car.left == 1) // if left isGap
        {
            // add node at left to stack
            AddDirectionToStack(car.directionLeft);
        }
        if (*car.right == 1) // if right isGap
        {
            // add node at right to stack
            AddDirectionToStack(car.directionRight);
        }
        if (*car.forward == 1) // if front isGap
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
        // Node tempNode = grid.gridArray[carPrevX - 1][carPrevY];
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
        // Node tempNode = grid.gridArray[carPrevX + 1][carPrevY];
        if (grid.gridArray[carPrevX][carPrevY].southIsWall == 1) // Checking if North Wall is a Gap
        {
            ChangeCarDirection(South, carPrevX + 1, carPrevY);
            // Pop dfsStack[]
            Pop();
        }
        else
            Backtrack();
    }
    // Checking East
    else if (carPrevX == Peek().x && carPrevY + 1 == Peek().y)
    {
        // Node tempNode = grid.gridArray[carPrevX][carPrevY + 1];
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
        // Node tempNode = grid.gridArray[carPrevX][carPrevY - 1];
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
        return dfsStack[stackTop - 1];
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

// Generates fake map in [4][5]
void TempMarkWall()
{
    if (car.xCoord == 0 && car.yCoord == 0)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
    }
    else if (car.xCoord == 0 && car.yCoord == 1)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
    }
    else if (car.xCoord == 0 && car.yCoord == 2)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
    }
    else if (car.xCoord == 0 && car.yCoord == 3)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
    }
    else if (car.xCoord == 0 && car.yCoord == 4)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
    }
    else if (car.xCoord == 1 && car.yCoord == 0)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
    }
    else if (car.xCoord == 1 && car.yCoord == 1)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
    }
    else if (car.xCoord == 1 && car.yCoord == 2)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
    }
    else if (car.xCoord == 1 && car.yCoord == 3)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
    }
    else if (car.xCoord == 1 && car.yCoord == 4)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
    }
    else if (car.xCoord == 2 && car.yCoord == 0)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
    }
    else if (car.xCoord == 2 && car.yCoord == 1)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
    }
    else if (car.xCoord == 2 && car.yCoord == 2)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
    }
    else if (car.xCoord == 2 && car.yCoord == 3)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
    }
    else if (car.xCoord == 2 && car.yCoord == 4)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
    }
    else if (car.xCoord == 3 && car.yCoord == 0)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 0;
    }
    else if (car.xCoord == 3 && car.yCoord == 1)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
    }
    else if (car.xCoord == 3 && car.yCoord == 2)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
    }
    else if (car.xCoord == 3 && car.yCoord == 3)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
    }
    else if (car.xCoord == 3 && car.yCoord == 4)
    {
        grid.gridArray[car.xCoord][car.yCoord].northIsWall = 1;
        grid.gridArray[car.xCoord][car.yCoord].eastIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].southIsWall = 0;
        grid.gridArray[car.xCoord][car.yCoord].westIsWall = 1;
    }
}

void MarkWall(float ultrasonicDistance, Direction ultrasonicDirection)
{
    // if ultrasonic value is more than distance, it is not a wall
    if (ultrasonicDistance > 13.5f) // check how many cm is wall from car (one grid is 27cm)
    {
        // mark ultrasonic direction as gap
        switch (ultrasonicDirection)
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
    else // it is a wall
    {
        // mark ultrasonic direction as wall
        switch (ultrasonicDirection)
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
    float frontUltrasonicDist = 0.0f; // call function to get front ultrasonic distance <to be done by ultrasonic>
    float rightUltrasonicDist = 0.0f; // call function to get right ultrasonic distance <to be done by ultrasonic>
    float leftUltrasonicDist = 0.0f; // call function to get left ultrasonic distance <to be done by ultrasonic>
    float backUltrasonicDist = 0.0f; // call function to get back ultrasonic distance <to be done by ultrasonic>

    TempMarkWall(); // for testing
    // MarkWall(frontUltrasonicDist, car.directionFacing);
    // MarkWall(rightUltrasonicDist, GetRightDirection(car.directionFacing));
    // MarkWall(leftUltrasonicDist, GetLeftDirection(car.directionFacing));
    // MarkWall(backUltrasonicDist, GetBackDirection(car.directionFacing));
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
        // Travel Forward <to be done by motor>

        SetCar(&car, xPosToGo, yPosToGo, car.directionFacing);
    }
    else if (GetBackDirection(directionToGo) == car.directionFacing)
    {
        // Reverse <to be done by motor>

        SetCar(&car, xPosToGo, yPosToGo, car.directionFacing);
    }
    else if (directionToGo == GetRightDirection(car.directionFacing))
    {
        // Turn Right <to be done by motor>

        SetCar(&car, xPosToGo, yPosToGo, GetRightDirection(car.directionFacing));
    }
    else if (directionToGo == GetLeftDirection(car.directionFacing))
    {
        // Turn Left <to be done by motor>

        SetCar(&car, xPosToGo, yPosToGo, GetLeftDirection(car.directionFacing));
    }
}

Direction GetLeftDirection(Direction frontDirection)
{
    switch (frontDirection)
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
    switch (frontDirection)
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
    switch (frontDirection)
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

// converts 7x9 grid to 4x5 grid
void ConvertMappedGrid()
{
    // find x and y boundaries on 7x9 grid
    int startX, startY;
    // find far left node of car that is not visited
    for (int x = car.xCoord - 1; x >= -1; x--)
    {
        if (x == -1 || grid.gridArray[x][car.yCoord].isVisited == 0) // if grid not visited
        {
            startX = x + 1; // starting x is the previous x
            break;
        }
    }
    for (int y = car.yCoord - 1; y >= -1; y--)
    {
        if (y == -1 || grid.gridArray[car.xCoord][y].isVisited == 0) // if grid not visited
        {
            startY = y + 1; // starting y is the previous y
            break;
        }
    }

    // put 7x9 node content into 4x5 node content
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 5; y++)
            finalMapArray[x][y] = grid.gridArray[x + startX][y + startY]; // copy content into 4x5 small grid
    }

    // change car pos
    car.xCoord -= startX;
    car.yCoord -= startY;
}

// function call sequence for navigation
// receive coordinate from signal team
void receiveCoordinate()
{
    //uart_read_data();
    if (dataAvailable) // variables: x for x-coordinate, y for y-coordinate
    {
        //-> call setCoord with current car coord and dest row/col, function will convert into 9x11 usable coordinate
        //-> call conversionConstructor with the [4][5] array from mapping to create navigation array
        //-> call navigateTo with the navigation array, visited array, carRow, carCol
        //-> print out movement list
        //-> TODO-DONE: replace movement list print instructions with movement calls to car movement
        //-> TODO: send signal once destination reach to signal team to keep polling for more inputs
        //-> TODO: send a signal if no route possible to reach target dest
        setCoord(car.yCoord,car.xCoord,y,x);
        conversionConstructor(finalMapArray);
        navigateTo(navigationArray,visitedArray,destRow,destCol);
        targetLocator(exitFound, movementList,backfill);
    }
}

// convert 4x5 into 9x11
// conversion guide: (lengthx2) + 1
void conversionConstructor(Node gridArray[4][5])
{
    // reset all variables
    // initialization node centers
    // 0 = wall, 1 = space

    carRow = 0;
    carCol = 0;
    destRow = 0;
    destCol = 0;
    exitFound = 0;
    backfill = 0;

    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 11; y++)
        {
            navigationArray[x][y] = 0;
            visitedArray[x][y] = 0;
        }
    }

    for (int x = 0; x < 90; x++)
    {
        movementList[x].col = 0;
        movementList[x].row = 0;
    }

    // constructing navigation array map
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            int refRow = (i * 2) + 1;
            int refCol = (j * 2) + 1;

            // blocking out diagonals for each node
            navigationArray[refRow - 1][refCol - 1] = 1;
            navigationArray[refRow - 1][refCol + 1] = 1;
            navigationArray[refRow + 1][refCol - 1] = 1;
            navigationArray[refRow + 1][refCol + 1] = 1;

            // creating blocked off walls
            if (gridArray[i][j].northIsWall == 0)
            {
                navigationArray[refRow - 1][refCol] = 1;
            }
            if (gridArray[i][j].southIsWall == 0)
            {
                navigationArray[refRow + 1][refCol] = 1;
            }
            if (gridArray[i][j].eastIsWall == 0)
            {
                navigationArray[refRow][refCol - 1] = 1;
            }
            if (gridArray[i][j].westIsWall == 0)
            {
                navigationArray[refRow][refCol + 1] = 1;
            }
        }
    }
}

void setCoord(int carrow, int carcol, int destrow, int destcol)
{
    carRow = (carrow * 2) + 1;
    carCol = (carcol * 2) + 1;
    destRow = (destrow * 2) + 1;
    destCol = (destcol * 2) + 1;
}

int validMove(int navigationArray[9][11], int visitedArray[9][11], int newRow, int newCol)
{
    // doesnt exceed top/bottom boundaries
    if (newRow < 0 || newRow >= 9)
        return 0;
    // doesnt exceed left/right boundaries
    if (newCol < 0 || newCol >= 11)
        return 0;
    // is not a wall
    if (navigationArray[newRow][newCol] == 1)
        return 0;
    // has not been visited
    if (visitedArray[newRow][newCol] == 1)
        return 0;

    return 1;
}

// recursion code to search for path
int navigateTo(int navigationArray[9][11], int visitedArray[9][11], int currRow, int currCol)
{
    int foundExit;

    if ((currRow == destRow) && (currCol == destCol))
    {
        movementList[backfill].row = currRow;
        movementList[backfill].col = currCol;
        backfill += 1;
        visitedArray[currRow][currCol] = 1;
        exitFound = 1;
        return 1;
    }

    visitedArray[currRow][currCol] = 1;

    // recursion up(-1,0),down(+1,0),left(0,-1),right(0,+1)
    // up
    if (validMove(navigationArray, visitedArray, (currRow - 1), (currCol)))
    {
        foundExit = navigateTo(navigationArray, visitedArray, (currRow - 1), (currCol));
    }
    // down
    if (!foundExit && (validMove(navigationArray, visitedArray, (currRow + 1), (currCol))))
    {
        foundExit = navigateTo(navigationArray, visitedArray, (currRow + 1), (currCol));
    }
    // left
    if (!foundExit && (validMove(navigationArray, visitedArray, (currRow), (currCol - 1))))
    {
        foundExit = navigateTo(navigationArray, visitedArray, (currRow), (currCol - 1));
    }
    // right
    if (!foundExit && (validMove(navigationArray, visitedArray, (currRow), (currCol + 1))))
    {
        foundExit = navigateTo(navigationArray, visitedArray, (currRow), (currCol + 1));
    }

    if (foundExit)
    {
        movementList[backfill].row = currRow;
        movementList[backfill].col = currCol;
        backfill += 1;

        return 1;
    }
    return 0;
}

void targetLocator(int exitFound, Coordinate movementList[90], int backfill)
{
    if (exitFound)
    {
        printf("\nEXIT FOUND!");
        printf("\nTOTAL STEPS: %d", backfill - 1);
        for (int i = 0; i < backfill; i++)
        {
            if (i == 0)
            {
                printf("\nStarting location: X:%d, Y:%d", movementList[backfill - 1].col, movementList[backfill - 1].row);
                printf("\nTarget Location: X:%d, Y:%d", destCol, destRow);
                continue;
            }

            // this means that the row increased by one, aka the coordinate moved down
            if (movementList[backfill - i - 1].row == movementList[backfill - i].row + 1)
            {
                // move car IRL southwards half a grid worth of distance
                printf("\nDOWN");
                if (car.directionFacing == North)
                {
                    // turn back and move forward <to be done by motor>
                    
                }
                else if (car.directionFacing == South)
                {
                    // move forward <to be done by motor>
                    
                }
                else if (car.directionFacing == West)
                {
                    // turn left and move forward <to be done by motor>
                    
                }
                else if (car.directionFacing == East)
                {
                    // turn right and move forward <to be done by motor>
                    
                }
                car.directionFacing = South;
            }
            // this means that the row decreased by one, aka the coordinate moved up
            else if (movementList[backfill - i - 1].row == movementList[backfill - i].row - 1)
            {
                // move car IRL northwards half a grid worth of distance
                printf("\nUP");
                if (car.directionFacing == North)
                {
                    // move forward <to be done by motor>
                    
                }
                else if (car.directionFacing == South)
                {
                    // turn back and move forward <to be done by motor>
                    
                }
                else if (car.directionFacing == West)
                {
                    // turn right and move forward <to be done by motor>
                    
                }
                else if (car.directionFacing == East)
                {
                    // turn left and move forward <to be done by motor>
                    
                }
                car.directionFacing = North;
            }
            // this means that the col increased by one, aka the coordinate moved right
            else if (movementList[backfill - i - 1].col == movementList[backfill - i].col + 1)
            {
                // move car IRL eastwards half a grid worth of distance
                printf("\nRIGHT");
                if (car.directionFacing == North)
                {
                    // turn right and move forward <to be done by motor>
                    
                }
                else if (car.directionFacing == South)
                {
                    // turn left and move forward <to be done by motor>
                    
                }
                else if (car.directionFacing == West)
                {
                    // turn back and move forward <to be done by motor>
                    
                }
                else if (car.directionFacing == East)
                {
                    // move forward <to be done by motor>
                    
                }
                car.directionFacing = East;
            }
            // this means that the col decreased by one, aka the coordinate moved left
            else if (movementList[backfill - i - 1].col == movementList[backfill - i].col - 1)
            {
                // move car IRL westwards half a grid worth of distance
                printf("\nLEFT");
                if (car.directionFacing == North)
                {
                    // turn left and move forward <to be done by motor>
                    
                }
                else if (car.directionFacing == South)
                {
                    // turn right and move forward <to be done by motor>
                    
                }
                else if (car.directionFacing == West)
                {
                    // move forward <to be done by motor>
                    
                }
                else if (car.directionFacing == East)
                {
                    // turn back and move forward <to be done by motor>
                    
                }
                car.directionFacing = West;
            }
        }
    }
    else
        printf("\nNO ROUTE POSSIBLE!");
}