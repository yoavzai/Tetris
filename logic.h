#ifndef __LOGIC_H__
#define __LOGIC_H__

#include <windows.h>
#include <stdint.h>

// game grid data
#define GAME_GRID_ROWS 20
#define GAME_GRID_COLS 10
#define GAME_GRID_PAD 4
#define PIECE_GRID_ROWS 4
#define PIECE_GRID_COLS 4
#define SIDE_GRID_COLS 23
#define SIDE_GRID_ROWS 44

// game data
#define NUM_OF_LEVELS 10
#define NUM_OF_PIECES 7
#define NUM_OF_COLORS 10
#define LINES_PER_LEVEL 20
#define PIECE_STARTING_LOCATION {2,7}

// Piece drop time table in millisec
#define LEVEL0_DROP_TIME 150 
#define LEVEL1_DROP_TIME 100 
#define LEVEL2_DROP_TIME 80 
#define LEVEL3_DROP_TIME 60 
#define LEVEL4_DROP_TIME 40 
#define LEVEL5_DROP_TIME 30 
#define LEVEL6_DROP_TIME 20 
#define LEVEL7_DROP_TIME 15 
#define LEVEL8_DROP_TIME 10
#define LEVEL9_DROP_TIME 5

// Colors in 0x00bbggrr
#define BACKGROUND_COLOR 0x00000000
#define BORDER_COLOR 0x00666666
#define Z_COLOR 0x000000D8
#define S_COLOR 0x00D80000
#define I_COLOR 0x0000D800
#define O_COLOR 0x0000D800
#define L_COLOR 0x000000D8
#define J_COLOR 0x00D80000
#define T_COLOR 0x0000D800
#define WHITE_COLOR 0x00FFFFFF

#define BACKGROUND_COLOR_IDX 0
#define BORDER_COLOR_IDX 1
#define Z_COLOR_IDX 2
#define S_COLOR_IDX 3
#define I_COLOR_IDX 4
#define O_COLOR_IDX 5
#define L_COLOR_IDX 6
#define J_COLOR_IDX 7
#define T_COLOR_IDX 8
#define WHITE_COLOR_IDX 9

// Pieces Index ID
#define Z_IDX 0
#define S_IDX 1
#define I_IDX 2
#define O_IDX 3
#define L_IDX 4
#define J_IDX 5
#define T_IDX 6


typedef uint8_t PieceGrid_t[PIECE_GRID_ROWS][PIECE_GRID_COLS];
typedef struct Piece Piece_t;
typedef struct Coord Coord_t;
typedef struct Game Game_t;

struct Coord
{
    int Row;
    int Col;
};

struct Piece
{
    Coord_t Location;
    PieceGrid_t Grid;
    /*
    all bellow functions update the piece only if it is legal.
    return value: true if action was legal and piece was updated to the new state,
    false if action was illegal and piece state didn't change
    */
    bool (*MoveLeft)(Piece_t*);
    bool (*MoveRight)(Piece_t*);
    bool (*PushDown)(Piece_t*);
    bool (*RotateClockwise)(Piece_t*);
    bool (*RotateCounterClockwise)(Piece_t*);
};

struct Game
{
    HWND Window;
    uint8_t GameGrid[GAME_GRID_ROWS + 2*GAME_GRID_PAD][GAME_GRID_COLS + 2*GAME_GRID_PAD];
    uint8_t SideGrid[SIDE_GRID_ROWS][SIDE_GRID_COLS];
    Piece_t Pieces[NUM_OF_PIECES];
    uint32_t LevelTimeTable[NUM_OF_LEVELS];
    uint32_t ColorTable[NUM_OF_COLORS];
    bool Running;
    bool Over;
    Piece_t CurPiece;
    Piece_t NextPiece;
    uint32_t EndOfTurnTime;
    uint8_t Level;
    uint16_t Score;
    uint16_t BestScore;
};

extern Game_t Game;

bool MoveLeft(Piece_t *Piece);
bool MoveRight(Piece_t *Piece);
bool PushDown(Piece_t *Piece);
bool Z_Rotation(Piece_t *Piece);
bool S_Rotation(Piece_t *Piece);
bool O_Rotation(Piece_t *Piece);
bool I_Rotation(Piece_t *Piece);
bool DefaultClockRot(Piece_t *Piece);
bool DefaultCounterClockRot(Piece_t *Piece);
bool IsValidLocation(const Piece_t *Piece);
void AddPieceToGameGrid(const Piece_t Piece);
Piece_t GetRandomPiece();
void ClearFullRows();
void NewGameRoutine();
void EndOfTurnRoutine();
void GameOverRoutine();

#endif // __LOGIC_H__