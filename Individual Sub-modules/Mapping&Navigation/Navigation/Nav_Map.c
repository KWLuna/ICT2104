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
    
} Node;

enum Direction
{
    North,East,South,West

};

typedef struct
{
    int row;
    int col;
}Coordinate;


//variables
Node visitGrid[4][5];
int navigationArray[9][11];
bool visitedArray[9][11];
Coordinate movementList[90];
bool found=false;
int carRow=0;
int carCol=0;
int destRow;
int destCol;
//number of total movements taken by algo
int backfill=0;


//functions

//function call sequence
//receive coordinate from signal team
//-> call setCoord with current car coord and dest row/col with ref to [9][11]
//-> call conversionConstructor with a [4][5] array from mapping to create navigation array
//-> call navigateTo with the navigation array, visited array
//-> print out movement list
//-> TODO: replace movement list print instructions with movement calls to car movement
//-> TODO: send signal once destination reach to signal team to keep polling for more inputs
//-> TODO: send a signal if no route possible to reach target dest 

void setCoord(int carrow, int carcol, int destrow, int destcol)
{   
    carRow = carrow;
    carCol = carcol;
    destRow = destrow;
    destCol = destcol;

}

void conversionConstructor(Node gridArray[4][5])
{
    //initialization node centers as well as visited bool map
    //0 = space, 1 = wall, 100 = node
    
    for(int x=0; x<9;x++)
    {
        for(int y=0; y<11; y++)
        {
            navigationArray[x][y]=0;
            visitedArray[x][y]=false;
        }
    }


    //constructing navigation array map
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<5; j++)
        {
            int refRow = (i*2)+1;
            int refCol = (j*2)+1;

            navigationArray[refRow][refCol] = 0;

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
               navigationArray[refRow][refCol+1]=1;
            }
            if(gridArray[i][j].westIsWall == 0)
            {
               navigationArray[refRow][refCol-1]=1;
            }
        }
    }

    //visualization of navmap
    for(int x=0; x<9;x++)
    {
        printf("\n");
        for(int y=0; y<11; y++)
        {
            printf(" %d", navigationArray[x][y]);
        }
    }
    
}


bool validMove(int navigationArray[9][11],bool visitedArray[9][11],int newRow, int newCol)
{
    //doesnt exceed top/bottom boundaries
    if (newRow < 0 || newRow >= 9)       
        return false;
    //doesnt exceed left/right boundaries
    if (newCol < 0 || newCol >= 11)
        return false;
    //is not a wall
    if (navigationArray[newRow][newCol]==1)
        return false;
    //has not been visited
    if (visitedArray[newRow][newCol]==true)
        return false;

    return true;
}


//recursion code to search for path
bool navigateTo(int navigationArray[9][11],bool visitedArray[9][11], int currRow, int currCol)
{
    bool foundExit;
    

    if((currRow==destRow) && (currCol==destCol))
        {   
            movementList[backfill].row=currRow;
            movementList[backfill].col=currCol;
            backfill+=1;
            visitedArray[currRow][currCol]=true;
            found = true;
            return true;
        }

    visitedArray[currRow][currCol]=true;


    //recursion up(-1,0),down(+1,0),left(0,-1),right(0,+1)
    //up
    if(validMove(navigationArray,visitedArray,(currRow-1),(currCol)))
    {   
        foundExit = navigateTo(navigationArray,visitedArray,(currRow-1),(currCol));
    }
    //down
    if(!foundExit && (validMove(navigationArray,visitedArray,(currRow+1),(currCol))))
    {
        foundExit = navigateTo(navigationArray,visitedArray,(currRow+1),(currCol));
    }
    //left
    if(!foundExit && (validMove(navigationArray,visitedArray,(currRow),(currCol-1))))
    {   
        foundExit = navigateTo(navigationArray,visitedArray,(currRow),(currCol-1));
    }
    //right
    if(!foundExit && (validMove(navigationArray,visitedArray,(currRow),(currCol+1))))
    {
        foundExit = navigateTo(navigationArray,visitedArray,(currRow),(currCol+1));
    }

    if(foundExit)
    {
        movementList[backfill].row=currRow;
        movementList[backfill].col=currCol;
        backfill+=1;
        
        return true;
    }
    return false;

}


//main
int main()
{
    
    //multidimensional array is defined row X column / vert X hori
    //in this case access goes from 0,0 to 9,8
    //NOTE: convert array coordinate to grid based because coordinate is x,y while array is y,x
    //there is also absolute and relative coordinates to consider with reference to google sheets

    //uncomment this line and comment away the preconstructed array below when mapping is confirm a-go
    //conversionConstructor(visitGrid);

   int navigationArray[9][11]=
    {
        //sample map, go from 1,1 to 1,9
        1,1,1,1,1,1,1,1,1,1,1,
        1,0,0,0,0,0,0,0,0,0,1,
        1,0,1,0,1,0,1,1,1,0,1,
        1,0,1,0,1,0,0,0,1,0,1,
        1,0,1,0,1,1,1,1,1,0,1,
        1,0,1,0,0,0,0,0,0,0,1,
        1,0,1,1,1,1,1,1,1,0,1,
        1,0,0,0,0,0,0,0,0,0,1,
        1,1,1,1,1,1,1,1,1,1,1
    };

    //visualization of navmap
    for(int x=0; x<9;x++)
    {
        printf("\n");
        for(int y=0; y<11; y++)
        {
            printf(" %d", navigationArray[x][y]);
        }
    }

    //allow custom coordinate here once we fix with other teams
    destRow=1;
    destCol=9;

    //allow custom coordinate here once we fix with other teams
    carRow=1;
    carCol=1;

    navigateTo(navigationArray,visitedArray,carRow,carCol);

    printf("\n");
    //printing travelled bool map
    for(int x=0; x<9;x++)
    {
        printf("\n");
        for(int y=0; y<11; y++)
        {
            if(visitedArray[x][y]==true)
                printf(" T");
            if(visitedArray[x][y]==false)
                printf(" F");
        }
    }

    if(found)
    {
        printf("\nEXIT FOUND!");
        printf("\nTOTAL STEPS: %d",backfill-1);
        for(int i=0;i<backfill;i++)
        {
            if(i==0)
            {
                printf("\nStarting location: X:%d, Y:%d", movementList[backfill-1].col,movementList[backfill-1].row);
                printf("\nTarget Location: X:%d, Y:%d",destCol,destRow);
                continue;

            }
            
            //this means that the row increased by one, aka the coordinate moved down
            if(movementList[backfill-i-1].row == movementList[backfill-i].row + 1)
            {
                //move car IRL southwards half a grid worth of distance
                printf("\nDOWN");
            }
            //this means that the row decreased by one, aka the coordinate moved up
            else if(movementList[backfill-i-1].row == movementList[backfill-i].row - 1)
            {
                //move car IRL northwards half a grid worth of distance
                printf("\nUP");
            }
            //this means that the col increased by one, aka the coordinate moved right
            else if(movementList[backfill-i-1].col == movementList[backfill-i].col + 1)
            {
                //move car IRL eastwards half a grid worth of distance
                printf("\nRIGHT");
            }
            //this means that the col decreased by one, aka the coordinate moved left
            else if(movementList[backfill-i-1].col == movementList[backfill-i].col - 1)
            {
                //move car IRL westwards half a grid worth of distance
                printf("\nLEFT");
            }    
            
            
        }    
    }
    else
        printf("\nNO ROUTE POSSIBLE!");

    

   
    
    
}

