/****************************************/
/*Shane Bramley                         */
/*Project-01: Maze this .c file         */ 
/*will run the maze generation          */
/*and create fully built and solved maze*/
/****************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mazegen.h"
#include <time.h>
#include <string.h>

int Width = 0;
int Height = 0;
int WayPointX = 0;
int WayPointY = 0;
int WayPointAlleyLength = 0;
double WayPointDirectionPercent = 0.0;
double StraightProbability = 0.0;
int PrintAlgorithmSteps = FALSE;
int dirTest[4];
int **maze;
int dirSolveTest[3];
int possiblePaths[3];
int waysToGo;
int beginX,beginY;
int counter;
int beginCheck = FALSE;
int tempEnd,tempBegin;
int totalAmountOfSpaces;
int freeFlag;
_Bool isSolving;
int endx,endy;
int PIPELIST_INDEX = 15;
int isStart = FALSE;
int indexCheck;
int exitSolve = FALSE;
int maxWaysToCarve;
int wayPointNorthX,wayPointNorthY,wayPointSouthX,wayPointSouthY;
int wayPointEastX,wayPointEastY,wayPointWestY,wayPointWestX;
int findNorth = FALSE;
int findSouth = FALSE;
int noAlley = FALSE;
int threeByThree = FALSE;
int isNorth = FALSE;
int isSouth = FALSE;
int isWest = FALSE;
int isEast = FALSE;
int northStart = FALSE, southStart = FALSE, eastStart = FALSE, westStart = FALSE;
int wayX, wayY;
int firstMazeGen = TRUE;
//This is used for referncing the set up for the NSEW directions
/*
const int DIRECTION_LIST[] = {NORTH, EAST, SOUTH, WEST};
const int DIRECTION_DX[]   = {    0,    1,     0,   -1};
const int DIRECTION_DY[]   = {   -1,    0,     1,    0};

const unsigned char pipeList[] =
{
  219, 208, 198, 200, 210, 186, 201, 204, 
  181, 188, 205, 202, 187, 185, 203, 206
};
*/

void copyIntToAddress(int n, unsigned char bytes[])
{
  bytes[0] = n & 0xFF;
  bytes[1] = (n >>  8) & 0xFF;
  bytes[2] = (n >> 16) & 0xFF;
  bytes[3] = (n >> 24) & 0xFF;
}

void setRGB(unsigned char data[], int x, int y, int rowSize, 
            int pixelHeight,
            unsigned char r, unsigned char g, unsigned char b)
{
  y = (pixelHeight - y) - 1;
  int offset = (x * 3) + (y * rowSize);
  data[offset] = b;
  data[offset+1] = g;
  data[offset+2] = r;
}

//============MazePrint()=============
//This method will print out the maze
//====================================
void mazePrint()
{
    //Create the mask to test the locations
    int mask = GOAL + VISITED;
    int randEnd = rand() % Width;
    printf("\n\n");
    printf("========================\n");
    printf("Maze(%d x %d): (%d, %d)\n",
    Width, Height, WayPointX, WayPointY);
    printf("========================\n");

    //Check if the maze needs to be solved
    if(isSolving == TRUE)
    {
      for(int i = 0; i < Height; i++)
      {
        for(int j = 0; j < Width; j++)
        {
          //If this is the end or beginning turn it red
          if(j == beginX && i == beginY || j == endx && i == endy || j == WayPointX && i == WayPointY && threeByThree == FALSE)
          {
             textcolor(TEXTCOLOR_RED);
             if(maze[j][i] > 15)
             {
                maze[j][i] -= GOAL;
                if(maze[j][i] > 15)
                {
                   maze[j][i] -= GOAL;
                }
             }
             printf("%c", pipeList[maze[j][i]]);
             textcolor(TEXTCOLOR_WHITE);
          }
          else if(maze[j][i] > SPECIAL)
          {
             textcolor(TEXTCOLOR_RED);
             maze[j][i] -= SPECIAL;
             if(maze[j][i] > 32)
             {
                textcolor(TEXTCOLOR_RED);
                maze[j][i] -= VISITED;
                if(maze[j][i] > 15)
                {
                   textcolor(TEXTCOLOR_GREEN);
                   maze[j][i] -= GOAL;
                }
             }
             printf("%c", pipeList[maze[j][i]]);
             textcolor(TEXTCOLOR_WHITE);
          }
          else
          {
             if(maze[j][i] & GOAL)
             {
                textcolor(TEXTCOLOR_GREEN);
                maze[j][i] = maze[j][i] & 15;
             }
             else
             {
                textcolor(TEXTCOLOR_WHITE);
                maze[j][i] = maze[j][i] & 15;
             }
             printf("%c", pipeList[maze[j][i]]);
             textcolor(TEXTCOLOR_WHITE);
          }
        }
        printf("\n");
      }
      isSolving = FALSE;
    }
    else  //If it doesnt need to be solved then print out
    {
      for(int i = 0; i < Height; i++)
      {
        for(int j = 0; j < Width; j++)
        {
            if(maze[j][i] > SPECIAL)
            {
               maze[j][i] -= SPECIAL;
               if(PrintAlgorithmSteps == TRUE)
               {
                  textcolor(TEXTCOLOR_RED);
               }
               printf("%c", pipeList[maze[j][i]]);
               maze[j][i] += SPECIAL;
               textcolor(TEXTCOLOR_WHITE);
            }
            else if(j == WayPointX && i == WayPointY && PrintAlgorithmSteps == TRUE)
            {
               textcolor(TEXTCOLOR_RED);
               printf("%c", pipeList[maze[j][i]]);
               textcolor(TEXTCOLOR_WHITE);
            }
            else
            {	 
               printf("%c", pipeList[maze[j][i]]);
            }
        }
        printf("\n");
      }
    }
    isSolving = FALSE;
    
  int pixelWidth = Width * 8;
  int pixelHeight = Height * 8;
  int rowSize = pixelWidth * 3;
  int rowPadding = 0;
  rowPadding = (4 - (rowSize % 4)) % 4;
  rowSize += rowPadding;
  int pixelDataSize = rowSize*pixelHeight;
  int fileSize = 54 + pixelDataSize;

  unsigned char header[54] = 
  {
    'B','M',  // magic number
    0,0,0,0,  // size in bytes (set below)
    0,0,0,0,  // reserved
    54,0,0,0, // offset to start of pixel data
    40,0,0,0, // info hd size
    0,0,0,0,  // image width (set below)
    0,0,0,0,  // image heigth (set below)
    1,0,      // number color planes
    24,0,     // bits per pixel
    0,0,0,0,  // compression is none
    0,0,0,0,  // image bits size
    0x13,0x0B,0,0, // horz resoluition in pixel / m
    0x13,0x0B,0,0, // vert resolutions (0x03C3 = 96 dpi, 0x0B13 = 72 dpi)
    0,0,0,0,  // #colors in pallete
    0,0,0,0,  // #important colors
  };


  copyIntToAddress(fileSize, &header[2]);
  copyIntToAddress(pixelWidth, &header[18]);
  copyIntToAddress(pixelHeight, &header[22]);
  copyIntToAddress(pixelDataSize, &header[34]);

  unsigned char img[pixelDataSize];

  //Initilize all pixels to white. This also sets any row padding
  //  to 0FF, but that data is ignored by readers.
  memset(img,0xFF,sizeof(img));
  
  for(int i = 0; i < Height * 8; i++)
  {
     for(int j = 0; j < Width * 8; j++)
     {
        //setRGB(img, j, i, rowSize, pixelHeight, 255, 255, 255);
        //setRGB(img, j, i, rowSize, pixelHeight, 51, 51, 255);
	setRGB(img, j, i, rowSize, pixelHeight, 0, 180, 255);
     }
  }

  int counter = 0;
  for(int i = 0; i < Height; i++)
  {
     for(int j = 0; j < Width; j++)
     {
         if(maze[j][i] == 0) //Blank
         {
         }
         else if(maze[j][i] == 1) //South Dead End
         {
             int tempX = j * 8 + 4;
             int tempY = i * 8;
             
             for(int s = 0; s < 4; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX - 1, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempY += 1;
             }
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX - 1, tempY, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX + 1, tempY, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX - 2, tempY, rowSize, pixelHeight, 0, 0, 0);
         }
         else if(maze[j][i] == 2) //West Dead End
         {
             int tempX = j * 8;
             int tempY = i * 8 + 4;
             
             setRGB(img, tempX, tempY + 1, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY - 2, rowSize, pixelHeight, 0, 0, 0);
             for(int s = 0; s < 4; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
                tempX += 1;
             }
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
             tempX += 1;
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
             tempX += 1;
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
         }
         else if(maze[j][i] == 3) //North to East
         {
             int tempX = j * 8 + 7;
             int tempY = i * 8 + 3;
             int tempY2 = i * 8 + 4;
             
             for(int s = 0; s < 4; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX, tempY2, rowSize, pixelHeight, 0, 0, 0);
                tempX -= 1;
             }
             for(int s = 0; s < 4; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX + 1, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempY -= 1;
             }
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX + 1, tempY, rowSize, pixelHeight, 0, 0, 0);
         }
         else if(maze[j][i] == 4) //North Dead End
         {
             int tempX2 = j * 8 + 3;
             int tempX = j * 8 + 4;
             int tempY = i * 8;
             setRGB(img, tempX2 - 1, tempY, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX + 1, tempY, rowSize, pixelHeight, 0, 0, 0);
             for(int s = 0; s < 8; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX2, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempY += 1;
             }
         }
         else if(maze[j][i] == 5) //North/South 
         {
             int tempX2 = j * 8 + 3;
             int tempX = j * 8 + 4;
             int tempY = i * 8;
             
             for(int s = 0; s < 8; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX2, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempY += 1;
             }
         }
         else if(maze[j][i] == 6) //East to South
         {
             int tempX = j * 8 + 7;
             int tempY = i * 8 + 3;
             int tempY2 = i * 8 + 4;
             
             for(int s = 0; s < 4; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX, tempY2, rowSize, pixelHeight, 0, 0, 0);
                tempX -= 1;
             }
             for(int s = 0; s < 4; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX + 1, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempY += 1;
             }
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX + 1, tempY, rowSize, pixelHeight, 0, 0, 0);
         }
         else if(maze[j][i] == 7) //North/East/South
         {
             int tempX2 = j * 8 + 3;
             int tempX = j * 8 + 4;
             int tempY = i * 8;
             
             for(int s = 0; s < 8; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX2, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempY += 1;
             }
             int tempY2 = tempY - 5;
             tempY -= 4;
             for(int s = 0; s < 4; s++)
             {
                setRGB(img, tempX, tempY2, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempX += 1;
             }  
         }
         else if(maze[j][i] == 8) //East Dead End
         {
             int tempX = j * 8 + 7;
             int tempY = i * 8 + 3;
             int tempY2 = i * 8 + 4;
             setRGB(img, tempX, tempY2 + 1, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
             for(int s = 0; s < 8; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX, tempY2, rowSize, pixelHeight, 0, 0, 0);
                tempX -= 1;
             }
         }
         else if(maze[j][i] == 9) //North to West
         {
             int tempX2 = j * 8 + 3;
             int tempX = j * 8 + 4;
             int tempY = i * 8;
             
             for(int s = 0; s < 4; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX2, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempY += 1;
             }
             for(int s = 0; s < 4; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempX -= 1;
             }
             setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             tempX -= 1;
             setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                

         }
         else if(maze[j][i] == 10) //West to East
         {
             int tempX = j * 8 + 7;
             int tempY = i * 8 + 3;
             int tempY2 = i * 8 + 4;
          
             for(int s = 0; s < 8; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX, tempY2, rowSize, pixelHeight, 0, 0, 0);
                tempX -= 1;
             }
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY2, rowSize, pixelHeight, 0, 0, 0);
         }
         else if(maze[j][i] == 11) //North/West/East
         {
             int tempX2 = j * 8 + 3;
             int tempX = j * 8 + 4;
             int tempY = i * 8;
             
             for(int s = 0; s < 4; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX2, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempY += 1;
             }
        
             for(int s = 0; s < 4; s++)
             {
                setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempX += 1;
             }
             setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             for(int s = 0; s < 8; s++)
             {
                setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempX -= 1;
             }
             setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             tempX -= 1;
             setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
         }
         else if(maze[j][i] == 12) //West to South
         {
             int tempX = j * 8;
             int tempY = i * 8 + 4;
             
             for(int s = 0; s < 4; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX -1, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX -1, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempX += 1;
             }
             setRGB(img, tempX -1, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX -1, tempY, rowSize, pixelHeight, 0, 0, 0);
             for(int s = 0; s < 3; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX - 1, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempY += 1;
             }
             setRGB(img, tempX - 1, tempY, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
         }
         else if(maze[j][i] == 13) //North/West/South
         {
             int tempX2 = j * 8 + 3;
             int tempX = j * 8 + 4;
             int tempY = i * 8;
             
             for(int s = 0; s < 8; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX2, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempY += 1;
             }
             int tempY2 = tempY - 5;
             tempY -= 4;
             for(int s = 0; s < 4; s++)
             {
                setRGB(img, tempX, tempY2, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempX -= 1;
             }
             setRGB(img, tempX, tempY2, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             tempX -= 1;
             setRGB(img, tempX, tempY2, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             tempX -= 1;
             setRGB(img, tempX, tempY2, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);

         }
         else if(maze[j][i] == 14) //West/South/East
         {
             int tempX2 = j * 8 + 3;
             int tempX = j * 8 + 4;
             int tempY = i * 8 + 8;
             
             for(int s = 0; s < 4; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX2, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempY -= 1;
             }
        
             for(int s = 0; s < 4; s++)
             {
                setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempX += 1;
             }
             setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             for(int s = 0; s < 8; s++)
             {
                setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempX -= 1;
             }
             setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             tempX -= 1;
             setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
         }
         else if(maze[j][i] == 15) //North/South/East/West
         {
              int tempX2 = j * 8 + 3;
             int tempX = j * 8 + 4;
             int tempY = i * 8 + 8;
             
             for(int s = 0; s < 4; s++)
             {
                //Note: img, X, Y, rowSize, pixelHeight, color,color,color
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX2, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempY -= 1;
             }
        
             for(int s = 0; s < 4; s++)
             {
                setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempX += 1;
             }
             setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             for(int s = 0; s < 8; s++)
             {
                setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempX -= 1;
             }
             setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             tempX -= 1;
             setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX, tempY, rowSize, pixelHeight, 0, 0, 0);
             for(int s = 0; s < 4; s++)
             {
                setRGB(img, tempX + 5, tempY, rowSize, pixelHeight, 0, 0, 0);
                setRGB(img, tempX + 4, tempY, rowSize, pixelHeight, 0, 0, 0);
                tempY -= 1;
             }
             setRGB(img, tempX + 5, tempY, rowSize, pixelHeight, 0, 0, 0);
             setRGB(img, tempX + 4, tempY, rowSize, pixelHeight, 0, 0, 0);
         }
     }
  }
 
  FILE* myFile = fopen("Maze.bmp", "wb");
  fwrite(header, 1, sizeof(header), myFile);
  fwrite(img, 1, sizeof(img), myFile);
  fclose(myFile);

  printf("\nBITMAP Generated!\n");
/*
  if (0)
  { int i;
    for (i=0; i<54; i++)
    {
      printf("header[%2d]=%0X\n", i, header[i]);
    }
  }
*/
}

//================FlipDir()========================
//This method will change the direction of the char
//=================================================
int flipDir(int dir)
{
    //**This is needed because the directions need to be fliped towarfs
    //the previous location so it makes things easier
    int direction = DIRECTION_LIST[dir];
    int N = 0;
    int E = 1;
    int S = 2;
    int W = 3;
    
    if(direction == NORTH)
    {
      return S;
    }
    if(direction == WEST)
    {
      return E;
    }
    if(direction == SOUTH)
    {
      return N;
    }
    if(direction == EAST)
    {
      return W;
    }
}


//===================FindDirSolve===============
//This function will find the new directions 
//to go
//============================================== 
int findDirSolve(int currentX, int currentY)
{
   int i; 
   int wayToGo = 0;
   waysToGo = 0;
   int newX, newY;
  
  for(i = 0; i < TOTAL_DIRECTIONS; i++)
  {
     newX = currentX + DIRECTION_DX[i];
     newY = currentY + DIRECTION_DY[i];
    // Checks every direction around the location we are at.
    if(newX < Width && newX >= 0 && newY < Height && newY >= 0)
    {
      if((maze[currentX][currentY] & DIRECTION_LIST[i]) > 0 && 
         (maze[newX][newY] & VISITED) == 0)
      {
        possiblePaths[waysToGo] = i;
        waysToGo++;
      }
    }
  }
  // returns a random valid path
  if(waysToGo != 0)
  {
    wayToGo = rand() % waysToGo;
    return possiblePaths[wayToGo]; 
  }
  else
  {
    return 0;
  }
}

//================FindDir()================
//This function will find a valid direction
//=========================================
int findDir(int currentX, int currentY)
{
  int index = 0;

  if(isSolving == TRUE)
  {
     index = findDirSolve(currentX, currentY);	  
  }
  else
  {
     for(int i = 0; i <= 3; i++)
     {
     //Check if direction is out of bounds
        if(currentX + DIRECTION_DX[i] >= 0 && 
                     currentX + DIRECTION_DX[i] < Width && 
                     currentY + DIRECTION_DY[i] >= 0 && 
                     currentY + DIRECTION_DY[i] < Height)
        {
          //Check to see if the space is empty in the maze
          if(maze[currentX + DIRECTION_DX[i]][currentY + DIRECTION_DY[i]] == 0)
          {
             dirTest[index] = i;
             index++;    
          }
        }
     }
  }
  return index;
}

//=====================Carve_Passage================================
//This method will carve the maze and make it a real fully built one
//==================================================================
int carvemaze(int currentX, int currentY, int dir) 
{
  int newDir;
  while(totalAmountOfSpaces > 0)
  {
    int Index = 0;
    if(threeByThree == FALSE)
    {
    if(currentY == 0 || currentY == Height-1 || currentX == Width-1 || 
       currentX == 0)
    {
       if(findNorth == FALSE)
       {
          beginX = currentX;
          beginY = currentY;
          findNorth = TRUE;
          if(beginX == 0)
          {
             maze[currentX][currentY] = 10;
          }
          else if(beginX == Width - 1)
          {
             maze[currentX][currentY] = 10;
          }
          else if(beginY == 0)
          {
             maze[currentX][currentY] = 5;
          }
          else if(beginY == Height -1)
          {
             maze[currentX][currentY] = 5;
          }
          if(isNorth == TRUE)
          {
             isNorth = 2;
          }
          else if(isSouth == TRUE)
          {
             isSouth = 2;
          }
          else if(isEast == TRUE)
          {
             isEast = 2;
          }
          else if(isWest == TRUE)
          {
             isWest = 2;
          }
          maxWaysToCarve = 0;
       }
       else if(findSouth == FALSE)
       {
          endx = currentX;
          endy = currentY;
          findSouth = TRUE;
          if(endx == 0)
          {
             maze[currentX][currentY] = 10;
          }
          else if(endx == Width - 1)
          {
             maze[currentX][currentY] = 10;
          }
          else if(endy == 0)
          {
             maze[currentX][currentY] = 5;
          }
          else if(endy == Height -1)
          {
             maze[currentX][currentY] = 5;
          }
          if(isNorth == TRUE)
          {
             isNorth = 2;
          }
          else if(isSouth == TRUE)
          {
             isSouth = 2;
          }
          else if(isEast == TRUE)
          {
             isEast = 2;
          }
          else if(isWest == TRUE)
          {
             isWest = 2;
          }
          maxWaysToCarve = 0;
       }
    }
    if(maxWaysToCarve == 0)
    {
       maxWaysToCarve = (Width * Height) * WayPointDirectionPercent;
       if(isNorth == TRUE)
       {
          if(PrintAlgorithmSteps == TRUE)
          {
             printf("WayPoint: North Reset");
          }
          wayPointNorthX = currentX;
          wayPointNorthY = currentY;
          isNorth = FALSE;
       }
       else if(isEast == TRUE)
       {
          if(PrintAlgorithmSteps == TRUE)
          {
             printf("WayPoint: East Reset");
          }
          wayPointEastX = currentX;
          wayPointEastY = currentY;
          isEast = FALSE;
       }
       else if(isWest == TRUE)
       {
          if(PrintAlgorithmSteps == TRUE)
          {
             printf("WayPoint: West Reset");
          }
          wayPointWestX = currentX;
          wayPointWestY = currentY;
          isWest = FALSE;
       }
       else if(isSouth == TRUE)
       {
          if(PrintAlgorithmSteps == TRUE)
          {
             printf("WayPoint: South Reset");
          }
          wayPointSouthX = currentX;
          wayPointSouthY = currentY;
          isSouth = FALSE;
       }
       
       
       int randWay = (rand() % 4);
       if(randWay == 0)
       {
          if(isNorth != 2)
          {
            if(wayPointNorthX != beginX && wayPointNorthY != beginY 
             || wayPointNorthX != endx && wayPointNorthY != endy)
             {
                currentX = wayPointNorthX;
                currentY = wayPointNorthY;
             } 
          }
          else
          {
             if(wayPointSouthX != beginX && wayPointSouthY != beginY 
             || wayPointSouthX != endx && wayPointSouthY != endy)
             {
                currentX = wayPointSouthX;
                currentY = wayPointSouthY;
             }    
          }
       }
       else if(randWay == 1)
       {
          if(isEast != 2)
          {
            if(wayPointEastX != beginX && wayPointEastY != beginY 
             || wayPointEastX != endx && wayPointEastY != endy)
             {
                currentX = wayPointEastX;
                currentY = wayPointEastY;
             } 
          }
          else
          {
             if(wayPointWestX != beginX && wayPointWestY != beginY 
             || wayPointWestX != endx && wayPointWestY != endy)
             {
                currentX = wayPointWestX;
                currentY = wayPointWestY;
             }    
          }
       }
       else if(randWay == 2)
       {
          if(isSouth != 2)
          {
            if(wayPointSouthX != beginX && wayPointSouthY != beginY 
             || wayPointSouthX != endx && wayPointSouthY != endy)
             {
                currentX = wayPointSouthX;
                currentY = wayPointSouthY;
             } 
          }
          else
          {
             if(wayPointNorthX != beginX && wayPointNorthY != beginY 
             || wayPointNorthX != endx && wayPointNorthY != endy)
             {
                currentX = wayPointNorthX;
                currentY = wayPointNorthY;
             }    
          }
       }
       else if(randWay == 3)
       {
          if(isWest != 2)
          {
            if(wayPointWestX != beginX && wayPointWestY != beginY 
             || wayPointWestX != endx && wayPointWestY != endy)
             {
                currentX = wayPointWestX;
                currentY = wayPointWestY;
             } 
          }
          else
          {
             if(wayPointEastX != beginX && wayPointEastY != beginY 
             || wayPointEastX != endx && wayPointEastY != endy)
             {
                currentX = wayPointEastX;
                currentY = wayPointEastY;
             }    
          }
       }
    }
    }
      //Check is there are any directions to go 
      Index = findDir(currentX, currentY);
    
      if(Index != 0)
      { 
        int randomNumber;
        //Get random number
        randomNumber = rand() % Index;
       
        if(findNorth == FALSE && dirTest[randomNumber] == 2)
        {
           Index = findDir(currentX, currentY);
           randomNumber = rand() % Index;
        }
        //Get Random direction
        int randomDir = dirTest[randomNumber];
      
        newDir = flipDir(randomDir);
        //Print Random direction
        if(maze[currentX][currentY] == 0)
        {
          maze[currentX][currentY] = 
          DIRECTION_LIST[dir] | DIRECTION_LIST[randomDir];
        }
        //If the space has something in it add a new random durection
        else if(maze[currentX][currentY] != 0)
        {
          maze[currentX][currentY] += DIRECTION_LIST[randomDir];
        }
        //Get new current x and y
        currentX += DIRECTION_DX[dirTest[randomNumber]];
        currentY += DIRECTION_DY[dirTest[randomNumber]];
        //Decrament total spaces in the array checked
        totalAmountOfSpaces--;
        maxWaysToCarve--;
        //Call method to do recursion
        carvemaze(currentX, currentY, newDir);
      }
      //No more spaces left so return
      else
      {
        if(maze[currentX][currentY] == 0)
        {
           maze[currentX][currentY] = DIRECTION_LIST[dir];	
        }
        return 0;
      }
    //}  
  }  
}

//================MazeCarveAlley===================
//Create alleyways inside the maze before
//=================================================
int createAlleyWays(int x, int y)
{
   int tempWayPointX = x;
   int tempWayPointY = y;
   //printf("WayPoint is X: %d Y: %d", x, y);
   for(int i = 0; i < WayPointAlleyLength; i++)
   {
      if(x + DIRECTION_DX[0] > Width && y + DIRECTION_DY[0] > Height || i == WayPointAlleyLength - 1)
      {
         //maze[x][y] = 5;
         maze[x][y] = 4 + SPECIAL;
         wayPointNorthX = x;
         wayPointNorthY = y;
         //totalAmountOfSpaces--;
         break;	 
      }
      else if(x + DIRECTION_DX[0] < Width && y + DIRECTION_DY[0] < Height && i != WayPointAlleyLength -1)
      {
         x = x + DIRECTION_DX[0];
         y = y + DIRECTION_DY[0];
         //totalAmountOfSpaces--;
         maze[x][y] = 5 + SPECIAL;  
      }
   }
   x = tempWayPointX;
   y = tempWayPointY;
   for(int i = 0; i < WayPointAlleyLength; i++)
   {
      if(x + DIRECTION_DX[1] > Width && y + DIRECTION_DY[1] > Height || i == WayPointAlleyLength - 1)
      {
         maze[x][y] = 8 + SPECIAL;
         wayPointEastX = x;
         wayPointEastY = y;
         break;	 
      }
      else if(x + DIRECTION_DX[1] < Width && y + DIRECTION_DY[1] < Height && i != WayPointAlleyLength -1)
      {
         x = x + DIRECTION_DX[1];
         y = y + DIRECTION_DY[1];
         maze[x][y] = 10 + SPECIAL;  
      }
   }
   x = tempWayPointX;
   y = tempWayPointY;
   for(int i = 0; i < WayPointAlleyLength; i++)
   {
      if(x + DIRECTION_DX[2] > Width && y + DIRECTION_DY[2] > Height || i == WayPointAlleyLength - 1)
      {
         maze[x][y] = 1 + SPECIAL;
         wayPointSouthX = x;
         wayPointSouthY = y;
         break;	 
      }
      else if(x + DIRECTION_DX[2] < Width && y + DIRECTION_DY[2] < Height && i != WayPointAlleyLength -1)
      {
         x = x + DIRECTION_DX[2];
         y = y + DIRECTION_DY[2];
         maze[x][y] = 5 + SPECIAL;  
      }
   }
   x = tempWayPointX;
   y = tempWayPointY;
   for(int i = 0; i < WayPointAlleyLength; i++)
   {
      if(x + DIRECTION_DX[3] > Width && y + DIRECTION_DY[3] > Height || i == WayPointAlleyLength - 1)
      {
         maze[x][y] = 2 + SPECIAL;
         wayPointWestX = x;
         wayPointWestY = y;
         break;	 
      }
      else if(x + DIRECTION_DX[3] < Width && y + DIRECTION_DY[3] < Height && i != WayPointAlleyLength -1)
      {
         x = x + DIRECTION_DX[3];
         y = y + DIRECTION_DY[3];
         maze[x][y] = 10 + SPECIAL;  
      }
   }
}

//===========MazeGenerate==============
//This method will run the initial maze
//=====================================
int mazeGenerate(
    int width, int height,              // [3, 1000],  [3, 1000]
    int wayPointX, int wayPointY,       // [1, width],   [1, height]
    int wayPointAlleyLength,            // [0,  min(width, height)/2 ]
    
    double wayPointDirectionPercent,    // [0.0,  1.0]
    double straightProbability,         // [0.0,  1.0]
    int printAlgorithmSteps)            // [TRUE | FALSE]
{
   
   //Free the maze at the beginnig just incase
   mazeFree();
   isSolving = FALSE;
   if(width < 3 || height < 3)
   {
      printf("Error: Width or Height is less than 3\n");
      return TRUE;
   }
   
   //Pass in the varibles that are needed 
   Width = width;
   Height = height;
   WayPointX = wayPointX;
   WayPointY = wayPointY;
   WayPointAlleyLength = wayPointAlleyLength;
   WayPointDirectionPercent = wayPointDirectionPercent;
   StraightProbability = straightProbability;
   PrintAlgorithmSteps = printAlgorithmSteps;
   
   /* Allocate the maze array. */
   maze = malloc(width * sizeof(*maze));
   for(int i = 0; i < width; i++)
   {
      maze[i] = malloc(height * sizeof(int));
   }
   freeFlag = 1;
   //If the maze is null then quit
   if(maze == NULL) 
   {
     printf("error: not enough memory\n");
     return TRUE;
   }
   //Initialize the array using a nested for loop
   for(int x = 0; x < width; x++)
   {
     for(int y = 0; y < height; y++)
     {
       maze[x][y] = 0;
     }
   }

   if(WayPointX-1 < 0 || WayPointY-1 < 0 || WayPointX > Width || WayPointY > Height)
   {
      printf("Error: The WayPoint is out of bounds");
      return TRUE;
   }
   if(Width > 3 && Height > 3)
   {
      //Set the way point
      maze[WayPointX][WayPointY] = 15;
   }
   else
   {
       threeByThree = TRUE;
       //WayPointX -= 1;
       //wayPointY -= 1;
       //maze[WayPointX][WayPointY] = 15;
   }
   //Get the total number of spaces
   totalAmountOfSpaces = (Width * Height);
   if(threeByThree == FALSE)
   {
   if(WayPointAlleyLength != 0)
   {
      createAlleyWays(WayPointX, WayPointY); 
   }
   else
   {   
      noAlley = TRUE;
      maze[WayPointX][WayPointY + 1] = 1 + SPECIAL;
      wayPointNorthX = WayPointX;
      wayPointNorthY = WayPointY + 1;
      maze[WayPointX][WayPointY - 1] = 4 + SPECIAL;
      wayPointSouthX = WayPointX;
      wayPointSouthY = WayPointY - 1;
      maze[WayPointX - 1][WayPointY] = 2 + SPECIAL;
      wayPointWestX = WayPointX - 1;
      wayPointWestY = WayPointY;
      maze[WayPointX + 1][WayPointY] = 8 + SPECIAL;
      wayPointEastX = WayPointX + 1;
      wayPointEastY = WayPointY;
   }
   }
   //Maze2WayPoint(WayPointX, WayPointY, NORTH);
   maxWaysToCarve = (Width * Height) * WayPointDirectionPercent;
   //*********Old Maze Carve*******
   //Create Start position
   int randNum = (rand() % Width);
   //******Old*******
   beginX = randNum;
   beginY = 0;
   if(threeByThree == FALSE)
   {
   int directionStart;
   int randWay = (rand() % 4);
   if(randWay == 0)
   {
      directionStart = 0;
      isNorth = TRUE;
      wayX = wayPointNorthX;
      wayY = wayPointNorthY;
   }
   else if(randWay == 1)
   {
      directionStart = 1;
      isEast = TRUE;
      wayX = wayPointEastX;
      wayY = wayPointEastY;
   }
   else if(randWay == 2)
   {
      directionStart = 2;
      isSouth = TRUE;
      wayX = wayPointSouthX;
      wayY = wayPointSouthY;
   }
   else if(randWay == 3)
   {
      directionStart = 3;
      isWest = TRUE;
      wayX = wayPointWestX;
      wayY = wayPointWestY;
   }
   }
   if(threeByThree == FALSE)
   {
      carvemaze(wayX, wayY, 0);
   }
   else
   {
      carvemaze(randNum, 0, 0);
   //Create End Position
   int randNumEnd = (rand() % Width);
   maze[randNumEnd][Height-1] += SOUTH;
   //printf("End");
   endx = randNumEnd;
   endy = Height - 1;
   }
}
//============SolveMaze()============
//This method will solve the maze
//===================================
int SolveTheMaze(int currentx, int currenty, int dir)
{
  int nextDir;
  //If the program is still solving while keeps going
  while(isSolving == TRUE)
  {
     //Add the Goal plus the visited into the current location
     maze[currentx][currenty] |= GOAL + VISITED;
     //Get the next direction from the current
     nextDir = findDir(currentx,currenty);
          
     if(waysToGo == 0)
     {
        maze[currentx][currenty] -= GOAL;
        return 0;
     }
     if((currentx == endx) && (currenty == endy))
     {
        exitSolve = TRUE;
        isSolving = FALSE;
        return 0;
     }
     //Go to the new location 
     int currentX2 = currentx + DIRECTION_DX[nextDir];
     int currentY2 = currenty + DIRECTION_DY[nextDir];
     nextDir = flipDir(nextDir);
     //Do recurrsion and call the function
     SolveTheMaze(currentX2, currentY2, nextDir);
     if(exitSolve == FALSE && maze[currentx][currenty] & GOAL)
     {
        maze[currentx][currenty] -= GOAL;
     } 
  }
}

//================MazeSolve()===================
//This method will begin the solving of the maze
//==============================================
void mazeSolve()
{
  //printf("BeginX:%d BeginY: %d\n", beginX, beginY);
  //Save the beginning location
  int tempWay = maze[WayPointX][WayPointY];
  tempBegin = maze[beginX][beginY];
  //tempBegin += 2;
  //Save the ending location
  tempEnd = maze[endx][endy];
  isSolving = TRUE;
  //printf("BeginX:%d BeginY: %d\n", beginX, beginY);
  //Call my solveMaze method
  SolveTheMaze(beginX, beginY, 0);
  isSolving = TRUE;
  maze[beginX][beginY] = tempBegin;
  maze[endx][endy] = tempEnd;
  maze[WayPointX][WayPointY] = tempWay;
  beginCheck = FALSE;
}

//================MazeFree()======================
//This method will free the memory for the program
//================================================
void mazeFree()
{
  //**This flag is used so I can use free multiple times
  if(freeFlag == 0)
  {
    return;
  }
  else
  {
    for(int x = 0; x < Height; x++)
    {
      free(maze[x]);
    }
    free(maze);
    freeFlag = 0;
  }
}
