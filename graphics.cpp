#include <windows.h>
#include <stdint.h>

#include "graphics.h"
#include "logic.h"
#include "init.h"

typedef uint8_t NumGrid_t[5][3];

static uint8_t GameGridCopy[GAME_GRID_ROWS + 2*GAME_GRID_PAD][GAME_GRID_COLS + 2*GAME_GRID_PAD];

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
        for (int j = 0, Col = 7; j < PIECE_GRID_COLS; ++j, Col+=2)
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
    for (int col = 0; col < GAME_GRID_COLS+2*GAME_GRID_PAD; col++)
    {
        GameGridCopy[GAME_GRID_PAD-1][col] = 1;
    }

}

static void AddCurPieceToGameGridCopy()
{
    for (int i = 0, x = Game.CurPiece.Location.Row; i < PIECE_GRID_ROWS; ++i, ++x)
    {
        for (int j = 0, y = Game.CurPiece.Location.Col; j < PIECE_GRID_COLS; ++j, ++y)
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
    memcpy(GameGridCopy, Game.GameGrid,
        (GAME_GRID_ROWS + 2*GAME_GRID_PAD)*(GAME_GRID_COLS + 2*GAME_GRID_PAD));
    AddCurPieceToGameGridCopy();
    AddUpperBorderToGameGridCopy();

    for (int Row = GAME_GRID_PAD-1, WindowRow = 0; Row < GAME_GRID_ROWS+GAME_GRID_PAD+1; ++Row, ++WindowRow)
    {
        int top = WindowRow * GAME_GRID_BLOCK_SIZE;
        int bottom = top + GAME_GRID_BLOCK_SIZE;
        for (int Col = GAME_GRID_PAD-1, WindowCol = 0; Col <GAME_GRID_COLS+GAME_GRID_PAD+1; ++Col, ++WindowCol)
        {
            int left = WindowCol * GAME_GRID_BLOCK_SIZE;
            int right = left + GAME_GRID_BLOCK_SIZE;
            RECT Block = {left, top, right, bottom};
            FillRect(Game.WindowDC, &Block, Game.BrushTable[GameGridCopy[Row][Col]]);
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
            FillRect(Game.WindowDC, &Block, Game.BrushTable[Game.SideGrid[Row][Col]]);
        }
    }
}

static void AddSideGridBestText()
{
    uint8_t BestText[5][21] = {
    {0,0,0,9,9,9,0,9,9,9,0,9,9,9,0,9,9,9,0,0,0},
    {0,0,0,9,0,9,0,9,0,0,0,9,0,0,0,0,9,0,0,0,0},
    {0,0,0,9,9,0,0,9,9,9,0,9,9,9,0,0,9,0,0,0,0},
    {0,0,0,9,0,9,0,9,0,0,0,0,0,9,0,0,9,0,0,0,0},
    {0,0,0,9,9,9,0,9,9,9,0,9,9,9,0,0,9,0,0,0,0}};

    for (int i = 0, Row = 27; i < 5; ++i, ++Row)
    {
        for (int j = 0, Col = 0; j < 21; ++j, ++Col)
        {
            Game.SideGrid[Row][Col] = BestText[i][j];
        } 
    }
}

static void AddSideGridNextText()
{
    uint8_t NextText[5][21] = {
    {0,0,9,9,0,0,9,0,9,9,9,0,9,0,9,0,9,9,9,0,0},
    {0,0,9,9,0,0,9,0,9,0,0,0,9,0,9,0,0,9,0,0,0},
    {0,0,9,0,9,0,9,0,9,9,9,0,0,9,0,0,0,9,0,0,0},
    {0,0,9,0,0,9,9,0,9,0,0,0,9,0,9,0,0,9,0,0,0},
    {0,0,9,0,0,9,9,0,9,9,9,0,9,0,9,0,0,9,0,0,0}};

    for (int i = 0, Row = 27; i < 5; ++i, ++Row)
    {
        for (int j = 0, Col = 0; j < 21; ++j, ++Col)
        {
            Game.SideGrid[Row][Col] = NextText[i][j];
        } 
    }
}

static void AddBestScoreToSideGrid()
{
    NumGrid_t * Ones = Numbers[Game.BestScore % 10];
    NumGrid_t * Tens = Numbers[(Game.BestScore/10) % 10];
    NumGrid_t * Hundreds = Numbers[(Game.BestScore/100) % 10];

    for (int i = 0, Row = 35; i < 5; ++i, ++Row)
    {
        for (int j = 0, Col = 4; j < 3; ++j, ++Col)
        {
            Game.SideGrid[Row][Col] = (*Hundreds)[i][j];
            Game.SideGrid[Row][Col+5] = (*Tens)[i][j];
            Game.SideGrid[Row][Col+10] = (*Ones)[i][j];   
        }
         
    }
};

static void ClearLowerSideGrid()
{
    for (int Row = 27; Row < SIDE_GRID_ROWS-2; ++Row)
    {
        for (int Col = 0; Col < SIDE_GRID_COLS-2; ++Col)
        {
            Game.SideGrid[Row][Col] = 0;
        } 
    }
}

void DisplayMenu()
{
    AddScoreToSideGrid();
    ClearLowerSideGrid();
    AddSideGridBestText();
    AddBestScoreToSideGrid();

    PrintGameArea();
    PrintSideGrid();
}

void DisplayGame()
{
    AddScoreToSideGrid();
    ClearLowerSideGrid();
    AddSideGridNextText();
    AddNextPieceToSideGrid();
  
    PrintGameArea();
    PrintSideGrid();
}

void DisplayControlsBox()
{
    MessageBoxW(Game.Window, L"\
    \u2190  -  MOVE LEFT\n\
    \u2192  -  MOVE RIGHT\n\
    \u2193  -  PUSH DOWN\n\
    \u2191  -  COUNTER CLOCKWISE ROTATION\n\
    Z  -  COUNTER CLOCKWISE ROTATION\n\
    X  -  CLOCKWISE ROTATION\n\n\
    P  -  Pause\n\
    Esc  -  Exit",
    L"Controls", MB_OK);
}

void DisplayGameOverBox()
{
    MessageBox(Game.Window, "Good job!", "GAME OVER", MB_OK);
}

void DisplayNewBestBox()
{
    MessageBox(Game.Window, "YOU'VE SET A NEW RECORD!!!", "Congratulations!", MB_OK);
}

void DisplayPauseBox()
{
    MessageBox(Game.Window, "Press ENTER to continue", "Paused", MB_OK);
}