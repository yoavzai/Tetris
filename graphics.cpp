#include <windows.h>
#include <stdint.h>

#include "graphics.h"
#include "logic.h"

typedef uint8_t NumGrid_t[5][3];

static char GameGridCopy[GAME_GRID_ROWS + 2*GAME_GRID_PAD][GAME_GRID_COLUMNS + 2*GAME_GRID_PAD];

static NumGrid_t Zero = {
    {9,9,9},
    {9,0,9},
    {9,0,9},
    {9,0,9},
    {9,9,9}};
static NumGrid_t One = {
    {0,9,9},
    {0,0,9},
    {0,0,9},
    {0,0,9},
    {0,0,9}};
static NumGrid_t Two = {
    {9,9,9},
    {0,0,9},
    {9,9,9},
    {9,0,0},
    {9,9,9}};
static NumGrid_t Three = {
    {9,9,9},
    {0,0,9},
    {9,9,9},
    {0,0,9},
    {9,9,9}};
static NumGrid_t Four = {
    {9,0,9},
    {9,0,9},
    {9,9,9},
    {0,0,9},
    {0,0,9}};
static NumGrid_t Five = {
    {9,9,9},
    {9,0,0},
    {9,9,9},
    {0,0,9},
    {9,9,9}};
static NumGrid_t Six = {
    {9,9,9},
    {9,0,0},
    {9,9,9},
    {9,0,9},
    {9,9,9}};
static NumGrid_t Seven = {
    {9,9,9},
    {0,0,9},
    {0,0,9},
    {0,0,9},
    {0,0,9}};
static NumGrid_t Eight = {
    {9,9,9},
    {9,0,9},
    {9,9,9},
    {9,0,9},
    {9,9,9}};
static NumGrid_t Nine = {
    {9,9,9},
    {9,0,9},
    {9,9,9},
    {0,0,9},
    {9,9,9}};

static NumGrid_t * Numbers[10] = {&Zero, &One, &Two, &Three, &Four, &Five, &Six, &Seven, &Eight, &Nine};


static void AddScoreToSideGrid()
{
    NumGrid_t * Ones = Numbers[Game.Score % 10];
    NumGrid_t * Tens = Numbers[(Game.Score/10) % 10];
    NumGrid_t * Hundreds = Numbers[(Game.Score/100) % 10];

    for (int i = 0, Row = 14; i < 5; ++i, ++Row)
    {
        for (int j = 0, Col = 4; j < 3; ++j, ++Col)
        {
            Game.SideGrid[Row][Col] = (*Hundreds)[i][j];
            Game.SideGrid[Row][Col+5] = (*Tens)[i][j];
            Game.SideGrid[Row][Col+10] = (*Ones)[i][j];   
        }
         
    }
};

static void AddNextPieceToSideGrid()
{
    for (int i = 0, Row = 32; i < PIECE_GRID_ROWS; ++i, Row+=2)
    {
        for (int j = 0, Col = 7; j < PIECE_GRID_COLUMNS; ++j, Col+=2)
        {
            uint8_t num = Game.NextPiece.Grid[i][j];
            Game.SideGrid[Row][Col] = num;
            Game.SideGrid[Row][Col+1] = num;
            Game.SideGrid[Row+1][Col] = num;
            Game.SideGrid[Row+1][Col+1] = num;
        } 
    } 
}

static void AddUpperBorderToGameGridCopy()
{
    for (int col = 0; col < GAME_GRID_COLUMNS+2*GAME_GRID_PAD; col++)
    {
        GameGridCopy[GAME_GRID_PAD-1][col] = 1;
    }

}

static void AddCurPieceToGameGridCopy()
{
    for (int i = 0, x = Game.CurPiece.Location.Row; i < PIECE_GRID_ROWS; ++i, ++x)
    {
        for (int j = 0, y = Game.CurPiece.Location.Col; j < PIECE_GRID_COLUMNS; ++j, ++y)
        {
            if (Game.CurPiece.Grid[i][j])
            {
                GameGridCopy[x][y] = Game.CurPiece.Grid[i][j];
            }
        }
    }
}

static void PrintGameArea()
{
    for (int Row = GAME_GRID_PAD-1, WindowRow = 0; Row < GAME_GRID_ROWS+GAME_GRID_PAD+1; ++Row, ++WindowRow)
    {
        int top = WindowRow * GAME_GRID_BLOCK_SIZE;
        int bottom = top + GAME_GRID_BLOCK_SIZE;
        for (int Col = GAME_GRID_PAD-1, WindowCol = 0; Col <GAME_GRID_COLUMNS+GAME_GRID_PAD+1; ++Col, ++WindowCol)
        {
            int left = WindowCol * GAME_GRID_BLOCK_SIZE;
            int right = left + GAME_GRID_BLOCK_SIZE;
            RECT Block = {left, top, right, bottom};
            HBRUSH Brush = CreateSolidBrush(Game.ColorTable[GameGridCopy[Row][Col]]);
            HDC dc =  GetDC(Game.Window);
            FillRect(dc, &Block, Brush);
            ReleaseDC(Game.Window, dc);
            DeleteObject(Brush);
        }
    }
}

static void PrintSideGrid()
{
    for (int Row = 0, WindowRow = 0; Row < SIDE_GRID_ROWS; ++Row, ++WindowRow)
    {
        int top = WindowRow * SIDE_GRID_BLOCK_SIZE;
        int bottom = top + SIDE_GRID_BLOCK_SIZE;        
        for (int Col = 0, WindowCol = 24; Col < SIDE_GRID_COLS; ++Col, ++WindowCol)
        {
            int left = WindowCol * SIDE_GRID_BLOCK_SIZE;
            int right = left + SIDE_GRID_BLOCK_SIZE;
            RECT Block = {left, top, right, bottom};
            HBRUSH Brush = CreateSolidBrush(Game.ColorTable[Game.SideGrid[Row][Col]]);
            HDC dc =  GetDC(Game.Window);
            FillRect(dc, &Block, Brush);
            ReleaseDC(Game.Window, dc);
            DeleteObject(Brush);
        }
    }
}

void DrawWindow()
{
    memcpy(GameGridCopy, Game.GameGrid,
            (GAME_GRID_ROWS + 2*GAME_GRID_PAD)*(GAME_GRID_COLUMNS + 2*GAME_GRID_PAD));
    AddCurPieceToGameGridCopy();
    AddUpperBorderToGameGridCopy();
    AddNextPieceToSideGrid();
    AddScoreToSideGrid();
  
    PrintGameArea();
    PrintSideGrid();
}