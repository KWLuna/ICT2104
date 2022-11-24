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

// Stack Stores a max of 20 as there are 20 nodes
Stack dfsStack[NUMBER_OF_NODES]; // Store Node to of where the stack can travel to but has not visited
int carPrevX = CAR_START_X;
int carPrevY = CAR_START_Y;

// declare functions
void CheckCurrentNode();
void MoveCarToStackPos();
void SavePrevXYToCurrentNode();
void Push(int x, int y);
Stack Pop();
Stack Peek();
void MarkWall(int x, int y, Node gridArray[][9], float ultrasonicDistance, Direction ultrasonicDirection);
void CheckNode(int x, int y, Node gridArray[][9], Direction directionFacing);
void SetCar(Car *car, int xPos, int yPos, Direction direction);
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

void CheckCurrentNode()
{

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
    if(directionToGo == car.directionFacing)
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

void MoveCarToStackPos()
{
    // Save Car Current Node Position
    carPrevX = car.xCoord;
    carPrevY = car.yCoord;

    // Check for Last Stack Position in dfsStack
    // Checking North
    if (carPrevX - 1 == Peek().x && carPrevY == Peek().y)
    {
        Node tempNode = grid.gridArray[carPrevX - 1][carPrevY];
        if(tempNode.northIsWall == 1) // Checking if North Wall is a Gap
        {
            ChangeCarDirection(North, carPrevX - 1, carPrevY);
            // Pop from dfsStack[]
            Pop();
        }
        else
        {
            // BackTrack!
            int nodePrevX = grid.gridArray[carPrevX][carPrevY].prevX;
            int nodePrevY = grid.gridArray[carPrevX][carPrevY].prevY;

            // Check the node location Whether it is north, south, east, west of the car

            Direction directionOfNode = CheckDirectionOfNode(nodePrevX, nodePrevY);
            ChangeCarDirection(directionOfNode, nodePrevX, nodePrevY);
        }
    }
    // Checking South
    else if (carPrevX + 1 == Peek().x && carPrevY == Peek().y)
    {
        Node tempNode = grid.gridArray[carPrevX + 1][carPrevY];
        if(tempNode.southIsWall == 1) // Checking if North Wall is a Gap
        {
            ChangeCarDirection(South, carPrevX + 1, carPrevY);
            // Pop dfsStack[]
            Pop();
        }
        else
        {
            // BackTrack!
            int nodePrevX = grid.gridArray[carPrevX][carPrevY].prevX;
            int nodePrevY = grid.gridArray[carPrevX][carPrevY].prevY;

            // Check the node location Whether it is north, south, east, west of the car
            Direction directionOfNode = CheckDirectionOfNode(nodePrevX, nodePrevY);
            ChangeCarDirection(directionOfNode, nodePrevX, nodePrevY);
        }
    }
    // Checking East
    else if (carPrevX  == Peek().x && carPrevY + 1 == Peek().y)
    {
        Node tempNode = grid.gridArray[carPrevX][carPrevY + 1];
        if(tempNode.eastIsWall == 1) // Checking if North Wall is a Gap
        {
            // check current direction car facing
            ChangeCarDirection(East, carPrevX, carPrevY + 1);
            
            // Pop dfsStack[]
            Pop();
        }
        else
        {
            // BackTrack!
            int nodePrevX = grid.gridArray[carPrevX][carPrevY].prevX;
            int nodePrevY = grid.gridArray[carPrevX][carPrevY].prevY;

            // Check the node location Whether it is north, south, east, west of the car
            Direction directionOfNode = CheckDirectionOfNode(nodePrevX, nodePrevY);
            ChangeCarDirection(directionOfNode, nodePrevX, nodePrevY);
        }
    }
    // Checking West
    else if (carPrevX  == Peek().x && carPrevY + 1 == Peek().y)
    {
        Node tempNode = grid.gridArray[carPrevX][carPrevY - 1];
        if(tempNode.westIsWall == 1) // Checking if West Wall is a Gap
        {
            ChangeCarDirection(West, carPrevX, carPrevY - 1);
            // Pop dfsStack[]
            Pop();
        }
        else
        {
            // BackTrack!
            int nodePrevX = grid.gridArray[carPrevX][carPrevY].prevX;
            int nodePrevY = grid.gridArray[carPrevX][carPrevY].prevY;

            // Check the node location Whether it is north, south, east, west of the car
            Direction directionOfNode = CheckDirectionOfNode(nodePrevX, nodePrevY);
            ChangeCarDirection(directionOfNode, nodePrevX, nodePrevY);
        }
    }
    // Checking West
    else if (carPrevX == Peek().x && carPrevY - 1 == Peek().y)
    {
        Node tempNode = grid.gridArray[carPrevX][carPrevY - 1];
        if(tempNode.westIsWall == 1) // Checking if North Wall is a Gap
        {
            // check current direction car facing
            Pop();
        }
        else
        {
            // BackTrack!
            int nodePrevX = grid.gridArray[carPrevX][carPrevY].prevX;
            int nodePrevY = grid.gridArray[carPrevX][carPrevY].prevY;

            // Check the node location Whether it is north, south, east, west of the car
            Direction directionOfNode = CheckDirectionOfNode(nodePrevX, nodePrevY);
            ChangeCarDirection(directionOfNode, nodePrevX, nodePrevY);
        }
    }
}

void SavePrevXYToCurrentNode()
{
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
        // add 1 to number of nodes visited
        numNodeVisited++;

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