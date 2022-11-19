#include <stdio.h>
#include <stdbool.h>

//typedefs
typedef struct
{
    bool isVisited;

    //-1: unknown, 0: wall, 1:gap, 2:travelledgap 
    int northIsWall;
    int eastIsWall;
    int southIsWall;
    int westIsWall;

    //count how many of the sides have been explored; 4 means all sides are accounted for
    //use 0000 to represent (NESW), e.g 1000 means north wall accounted for
    int allClear;
    

} Node;

enum Direction
{
    North,East,South,West

};

//variables
int completedNodes = 0;
enum Direction directionFacing;
int currentRow=4;
int currentCol=4;
Node visitGrid[10][9];


//functions

//used in the mapping function
void CheckNode()
{
    //check if current node is visited
    if(!visitGrid[currentRow][currentCol].isVisited)
    {
        visitGrid[currentRow][currentCol].isVisited = true;
        //check each wall here
        //if ultrasonic distance more than X, not wall
        //if ultrasonic distance less than X, is wall
            //then set the wall bool to true based on the direction
        //for now dummy here
        //visitGrid[currentRow][currentCol].allClear = 1111;
    }

}

//testing reporting car's current location and state of map at the moment
void ReportCar()
{
    printf("The car is currently located at %d, %d of the grid", currentRow, currentCol);
    //map printing for visualisation
    for(int i = 0 ; i < 10 ; i++)
    {
        printf("\n");
        for(int j = 0; j < 9; j++)
        {
            if(i == currentRow && j == currentCol)
                printf(" C ");
            else if(!visitGrid[i][j].isVisited)
                printf(" o ");
            else if(visitGrid[i][j].allClear == 1111)
                printf(" Y ");
            else 
                printf(" x ");
        }
        
    }
    printf("\n");
}


//main
int main()
{
    //multidimensional array is defined row X column / vert X hori
    //in this case access goes from 0,0 to 9,8
    //NOTE: convert array coordinate to grid based because coordinate is x,y while array is y,x
    //there is also absolute and relative coordinates to consider with reference to google sheets

    //grid instantiation on startup
    for(int i = 0 ; i < 10 ; i++)
        for(int j = 0; j < 9; j++)
        {
            visitGrid[i][j].isVisited = false;
            visitGrid[i][j].northIsWall = -1;
            visitGrid[i][j].eastIsWall = -1;
            visitGrid[i][j].southIsWall = -1;
            visitGrid[i][j].westIsWall = -1;
            visitGrid[i][j].allClear = 0000;
        }
    
    //car status initialisation
    directionFacing = North;

    currentRow=4; //move north: -1, move south: +1; never more than 9 or less than 0
    currentCol=4; //move west: -1 , move east: +1, never more than 8 or less than 0
    CheckNode();
    ReportCar();

    //testing out car movement
    //this is replaced when combined with the movement module from the other team
    currentRow+=1; //car goes south
    CheckNode();
    ReportCar();

    currentCol-=1; //car goes west
    CheckNode();
    ReportCar();

    // while(completedNodes < 20)
    // {
    //     CheckNode();
    //     completedNodes+=1;
    // }

    
    

}

