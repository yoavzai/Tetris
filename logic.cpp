#include <time.h> // time
#include <string.h> // memcpy
#include <windows.h>
#include <stdlib.h> //rand, srand

#include "logic.h"
#include "graphics.h"
#include "init.h"

Game_t Game;

bool IsValidLocation(const Piece_t * Piece)
{
    for (int i = 0, x = Piece->Location.Row; i < PIECE_GRID_ROWS; ++i, ++x)
    {
        for (int j = 0, y = Piece->Location.Col; j < PIECE_GRID_COLS; ++j, ++y)
        {
            if (Piece->Grid[i][j] && Game.GameGrid[x][y])
            {
                return false;
            }
        }
    } 

    return true;
}

bool MoveLeft(Piece_t * Piece)
{
    Piece->Location.Col -= 1;
    if (!IsValidLocation(Piece))
    {
        Piece->Location.Col += 1;
        return false;
    }
    return true;
}

bool MoveRight(Piece_t * Piece)
{
    Piece->Location.Col += 1;
    if (!IsValidLocation(Piece))
    {
        Piece->Location.Col -= 1;
        return false;
    }
    return true;
}

bool PushDown(Piece_t * Piece)
{
    Piece->Location.Row += 1;
    if (!IsValidLocation(Piece))
    {
        Piece->Location.Row -= 1;
        return false;
    }
    return true;
}

static void SwitchUint8(uint8_t *A, uint8_t *B)
{
    uint8_t temp = *A;
    *A = *B;
    *B = temp;
}

bool Z_Rotation(Piece_t * Piece)
{
    SwitchUint8(&Piece->Grid[2][1], &Piece->Grid[2][3]); 
    SwitchUint8(&Piece->Grid[1][3], &Piece->Grid[3][3]);
    if (!IsValidLocation(Piece))
    {
        Z_Rotation(Piece);
        return false;
    }
    return true;
}

bool S_Rotation(Piece_t * Piece)
{
    SwitchUint8(&Piece->Grid[1][2], &Piece->Grid[3][2]); 
    SwitchUint8(&Piece->Grid[3][1], &Piece->Grid[3][3]);
    if (!IsValidLocation(Piece))
    {
        S_Rotation(Piece);
        return false;
    }
    return true;
}

bool O_Rotation(Piece_t * Piece)
{
    return true;
}

bool I_Rotation(Piece_t * Piece)
{
    SwitchUint8(&Piece->Grid[2][0], &Piece->Grid[0][2]); 
    SwitchUint8(&Piece->Grid[2][1], &Piece->Grid[1][2]); 
    SwitchUint8(&Piece->Grid[2][3], &Piece->Grid[3][2]);
    if (!IsValidLocation(Piece))
    {
        I_Rotation(Piece);
        return false;
    }
    return true;
}

bool DefaultClockRot(Piece_t * Piece)
{
    SwitchUint8(&Piece->Grid[1][2], &Piece->Grid[3][2]); 
    SwitchUint8(&Piece->Grid[1][2], &Piece->Grid[2][1]); 
    SwitchUint8(&Piece->Grid[2][3], &Piece->Grid[3][2]);
    SwitchUint8(&Piece->Grid[1][1], &Piece->Grid[3][3]); 
    SwitchUint8(&Piece->Grid[1][1], &Piece->Grid[3][1]); 
    SwitchUint8(&Piece->Grid[1][3], &Piece->Grid[3][3]);
    if (!IsValidLocation(Piece))
    {
        DefaultCounterClockRot(Piece);
        return false;
    }
    return true;
} 

bool DefaultCounterClockRot(Piece_t * Piece)
{
    SwitchUint8(&Piece->Grid[2][1], &Piece->Grid[2][3]); 
    SwitchUint8(&Piece->Grid[1][2], &Piece->Grid[2][1]); 
    SwitchUint8(&Piece->Grid[2][3], &Piece->Grid[3][2]);
    SwitchUint8(&Piece->Grid[1][3], &Piece->Grid[3][1]); 
    SwitchUint8(&Piece->Grid[1][1], &Piece->Grid[3][1]); 
    SwitchUint8(&Piece->Grid[1][3], &Piece->Grid[3][3]);
    if (!IsValidLocation(Piece))
    {
        DefaultClockRot(Piece);
        return false;
    }
    return true;
}

Piece_t GetRandomPiece()
{
    static bool FirstEntrance = true;
    if (FirstEntrance)
    {
        srand(time(NULL));
        FirstEntrance = false;
    } 
    int idx = rand() % NUM_OF_PIECES;
    return (Game.Pieces[idx]);
}

void AddPieceToGameGrid(const Piece_t Piece)
{
    for (int i = 0, x = Piece.Location.Row; i < PIECE_GRID_ROWS; ++i, ++x)
    {
        for (int j = 0, y = Piece.Location.Col; j < PIECE_GRID_COLS; ++j, ++y)
        {
            if (Piece.Grid[i][j])
            {
               Game.GameGrid[x][y] = Piece.Grid[i][j];
            }        
        }
    }
}

static void CopyRow(int DestRow, const int SrcRow)
{
    for (int Col = GAME_GRID_PAD; Col <GAME_GRID_COLS+GAME_GRID_PAD; ++Col)
    {
        Game.GameGrid[DestRow][Col] = Game.GameGrid[SrcRow][Col];
    } 
}

static bool RowOcupied(int Row)
{
    for (int Col = GAME_GRID_PAD; Col < GAME_GRID_COLS+GAME_GRID_PAD; ++Col)
    {
        if (Game.GameGrid[Row][Col])
        {
            return true;
        }
    }
    return false;
}

static void UpdateScore(uint8_t NumOfFullRows)
{
    Game.Score += ((Game.Level + 2) * NumOfFullRows * NumOfFullRows);
}

static void UpdateLevel(uint8_t NumOfFullRows)
{
    static uint32_t CurLevelLineCount = 0;
    CurLevelLineCount += NumOfFullRows;
    if ((Game.Level < NUM_OF_LEVELS-1) && (CurLevelLineCount >= LINES_PER_LEVEL))
    {
        CurLevelLineCount -= LINES_PER_LEVEL;
        Game.Level += 1;           
    }
}

static void UpdateGameStats(uint8_t NumOfFullRows)
{
    UpdateScore(NumOfFullRows);
    UpdateLevel(NumOfFullRows);
}

static void DropRows(int DestRow)
{
    int AboveRow = DestRow - 1;
    while (RowOcupied(AboveRow))
    {
        CopyRow(DestRow, AboveRow);
        --DestRow;
        --AboveRow;
    }
    CopyRow(DestRow, AboveRow);
}

static void ClearRow(int Row)
{
    for (int Col = GAME_GRID_PAD; Col < GAME_GRID_COLS+GAME_GRID_PAD; ++Col)
    {
        Game.GameGrid[Row][Col] = 0;
    }
}

static bool RowIsFull(int Row)
{
    int NumOfOcupiedSquares = 0;
    for (int Col = GAME_GRID_PAD; Col <GAME_GRID_COLS+GAME_GRID_PAD; ++Col)
    {
        if (Game.GameGrid[Row][Col])
        {
            ++NumOfOcupiedSquares;
        }
    }

    return (NumOfOcupiedSquares == GAME_GRID_COLS);
}

void ClearFullRows()
{
    uint8_t NumOfFullRows = 0;
    for (int Row = GAME_GRID_PAD; Row < GAME_GRID_ROWS+GAME_GRID_PAD; ++Row)
    {
        if (RowIsFull(Row))
        {
            NumOfFullRows += 1;
            ClearRow(Row);
            DropRows(Row);
        }
    }
    UpdateGameStats(NumOfFullRows);
}

void EndOfTurnRoutine()
{
    if (!IsValidLocation(&Game.CurPiece))
    {
        Game.Over = true;
    }

    else if (!Game.CurPiece.PushDown(&Game.CurPiece))
    {
        AddPieceToGameGrid(Game.CurPiece);
        ClearFullRows();
        Game.CurPiece = Game.NextPiece;
        Game.NextPiece = GetRandomPiece();
    }
}

void NewGameRoutine()
{
    InitNewGame();
    DisplayGame();
    DisplayControlsBox();
    // TODO: add option to pick starting level
}

void GameOverRoutine()
{
    if (Game.Score > Game.BestScore)
    {
        Game.BestScore = Game.Score;
        DisplayMenu();
        DisplayNewBestBox();
    }
    else
    {
        DisplayMenu();
        DisplayGameOverBox();
    }
}

void ResetTurnTime()
{
    Game.EndOfTurnTime = GetTickCount() + Game.LevelTimeTable[Game.Level];
}

bool TurnOver()
{
    return (GetTickCount() >= Game.EndOfTurnTime);
}