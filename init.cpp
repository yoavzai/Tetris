#include <windows.h>
#include "logic.h"
#include "graphics.h"

static void InitGameGrid()
{
    int i = 0;
    int j = 0;
    for (i = GAME_GRID_PAD; i < GAME_GRID_ROWS + GAME_GRID_PAD; ++i)
    {
        for (j = 0; j < GAME_GRID_COLUMNS + 2*GAME_GRID_PAD; ++j)
        {
            if (j < GAME_GRID_PAD || j >= GAME_GRID_COLUMNS + GAME_GRID_PAD)
            {
                Game.GameGrid[i][j] = 1;
            }
        }
    }
    for (i; i < GAME_GRID_ROWS + 2*GAME_GRID_PAD; ++i)
    {
        for (j = 0; j < GAME_GRID_COLUMNS + 2*GAME_GRID_PAD; ++j)
        {
            Game.GameGrid[i][j] = 1;
        }
    }
}

static void InitWindow(HINSTANCE ProcInstance, WNDPROC MessagesCallbackFunc)
{
    WNDCLASS WindowClass = {};
    WindowClass.lpfnWndProc = MessagesCallbackFunc;
    WindowClass.hInstance = ProcInstance;
    WindowClass.lpszClassName = "TetrisWindowClass";
    if (0 == RegisterClass(&WindowClass))
    {
        OutputDebugString("Window Class not Registered in RegisterClass function call");
    }

    Game.Window =
    CreateWindowEx(
        0, // ExStyle
        WindowClass.lpszClassName, //ClassName
        "Tetris Window", // WindowName
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, // Style
        CW_USEDEFAULT, CW_USEDEFAULT, 720, 690, // Position
        0, // Parent
        0, // Menu
        ProcInstance, // ModuleInstance
        0); // Open param

    if (NULL == Game.Window)
    {
        // TODO: window not created log
        OutputDebugString("Window not created in CreateWindowEx function call");
        exit(1);
    }
}

static void InitPieces()
{
    Game.Pieces[Z_IDX] = {
        PIECE_STARTING_LOCATION,
        {{0,0,0,0},
        {0,0,0,0},
        {0,Z_COLOR_IDX,Z_COLOR_IDX,0},
        {0,0,Z_COLOR_IDX,Z_COLOR_IDX}},
        MoveLeft,
        MoveRight,
        PushDown,
        Z_Rotation,
        Z_Rotation
        };

    Game.Pieces[S_IDX] = {
        PIECE_STARTING_LOCATION,
        {{0,0,0,0},
        {0,0,0,0},
        {0,0,S_COLOR_IDX,S_COLOR_IDX},
        {0,S_COLOR_IDX,S_COLOR_IDX,0}},
        MoveLeft,
        MoveRight,
        PushDown,
        S_Rotation,
        S_Rotation
        };

    Game.Pieces[I_IDX] = {
        PIECE_STARTING_LOCATION,
        {{0,0,0,0},
        {0,0,0,0},
        {I_COLOR_IDX,I_COLOR_IDX,I_COLOR_IDX,I_COLOR_IDX},
        {0,0,0,0}},
        MoveLeft,
        MoveRight,
        PushDown,
        I_Rotation,
        I_Rotation
        };

    Game.Pieces[O_IDX] = {
        PIECE_STARTING_LOCATION,
        {{0,0,0,0},
        {0,0,0,0},
        {0,O_COLOR_IDX,O_COLOR_IDX,0},
        {0,O_COLOR_IDX,O_COLOR_IDX,0}},
        MoveLeft,
        MoveRight,
        PushDown,
        O_Rotation,
        O_Rotation
        };

    Game.Pieces[L_IDX] = {
        PIECE_STARTING_LOCATION,
        {{0,0,0,0},
        {0,0,0,0},
        {0,L_COLOR_IDX,L_COLOR_IDX,L_COLOR_IDX},
        {0,L_COLOR_IDX,0,0}},
        MoveLeft,
        MoveRight,
        PushDown,
        DefaultClockRot,
        DefaultCounterClockRot
        };

    Game.Pieces[J_IDX] = {
        PIECE_STARTING_LOCATION,
        {{0,0,0,0},
        {0,0,0,0},
        {0,J_COLOR_IDX,J_COLOR_IDX,J_COLOR_IDX},
        {0,0,0,J_COLOR_IDX}},
        MoveLeft,
        MoveRight,
        PushDown,
        DefaultClockRot,
        DefaultCounterClockRot
        };

    Game.Pieces[T_IDX] = {
        PIECE_STARTING_LOCATION,
        {{0,0,0,0},
        {0,0,0,0},
        {0,T_COLOR_IDX,T_COLOR_IDX,T_COLOR_IDX},
        {0,0,T_COLOR_IDX,0}},
        MoveLeft,
        MoveRight,
        PushDown,
        DefaultClockRot,
        DefaultCounterClockRot
        };
}

static void AddSideGridUpperBorder()
{
    for (int Row = 0; Row < 2; ++Row)
    {
        for (int Col = 0; Col < SIDE_GRID_COLS; ++Col)
        {
            Game.SideGrid[Row][Col] = 1;
        }
    }
}

static void AddSideGridMiddleBorder()
{
    for (int Row = 22; Row < 24; ++Row)
    {
        for (int Col = 0; Col < SIDE_GRID_COLS; ++Col)
        {
            Game.SideGrid[Row][Col] = 1;
        }
    }
}

static void AddSideGridLowerBorder()
{
    for (int Row = SIDE_GRID_ROWS-2; Row < SIDE_GRID_ROWS; ++Row)
    {
        for (int Col = 0; Col < SIDE_GRID_COLS; ++Col)
        {
            Game.SideGrid[Row][Col] = 1;
        }
    }
}

static void AddSideGridSideBorder()
{
    for (int Row = 2; Row < SIDE_GRID_ROWS-2; ++Row)
    {
        for (int Col = 21; Col < 23; ++Col)
        {
            Game.SideGrid[Row][Col] = 1;
        } 
    }
}

static void AddSideGridScoreText()
{
    uint8_t ScoreText[5][21] = {
    {0,9,9,9,0,9,9,9,0,9,9,9,0,9,9,9,0,9,9,9,0},
    {0,9,0,0,0,9,0,0,0,9,0,9,0,9,0,9,0,9,0,0,0},
    {0,9,9,9,0,9,0,0,0,9,0,9,0,9,9,9,0,9,9,9,0},
    {0,0,0,9,0,9,0,0,0,9,0,9,0,9,9,0,0,9,0,0,0},
    {0,9,9,9,0,9,9,9,0,9,9,9,0,9,0,9,0,9,9,9,0}};

    for (int i = 0, Row = 5; i < 5; ++i, ++Row)
    {
        for (int j = 0, Col = 0; j < 21; ++j, ++Col)
        {
            Game.SideGrid[Row][Col] = ScoreText[i][j];
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

static void InitSideGrid()
{
    AddSideGridUpperBorder();
    AddSideGridMiddleBorder();
    AddSideGridLowerBorder();
    AddSideGridSideBorder();
    AddSideGridScoreText();
    AddSideGridNextText();
}

static void InitLevelTimeTable()
{
    Game.LevelTimeTable[0] = LEVEL0_DROP_TIME;
    Game.LevelTimeTable[1] = LEVEL1_DROP_TIME;
    Game.LevelTimeTable[2] = LEVEL2_DROP_TIME;
    Game.LevelTimeTable[3] = LEVEL3_DROP_TIME;
    Game.LevelTimeTable[4] = LEVEL4_DROP_TIME;
    Game.LevelTimeTable[5] = LEVEL5_DROP_TIME;
    Game.LevelTimeTable[6] = LEVEL6_DROP_TIME;
    Game.LevelTimeTable[7] = LEVEL7_DROP_TIME;
    Game.LevelTimeTable[8] = LEVEL8_DROP_TIME;
    Game.LevelTimeTable[9] = LEVEL9_DROP_TIME;
}

static void InitColorTable()
{
    Game.ColorTable[BACKGROUND_COLOR_IDX] = BACKGROUND_COLOR;
    Game.ColorTable[BORDER_COLOR_IDX] = BORDER_COLOR;
    Game.ColorTable[Z_COLOR_IDX] = Z_COLOR;
    Game.ColorTable[S_COLOR_IDX] = S_COLOR;
    Game.ColorTable[I_COLOR_IDX] = I_COLOR;
    Game.ColorTable[O_COLOR_IDX] = O_COLOR;
    Game.ColorTable[L_COLOR_IDX] = L_COLOR;
    Game.ColorTable[J_COLOR_IDX] = J_COLOR;
    Game.ColorTable[T_COLOR_IDX] = T_COLOR;
    Game.ColorTable[WHITE_COLOR_IDX] = WHITE_COLOR;
}

void InitGame(HINSTANCE ProcInstance, WNDPROC MessagesCallbackFunc)
{
    InitWindow(ProcInstance, MessagesCallbackFunc);
    InitGameGrid();
    InitSideGrid();
    InitPieces();
    InitLevelTimeTable();
    InitColorTable();
    Game.Running = true;
    Game.CurPiece = GetRandomPiece();
    Game.NextPiece = GetRandomPiece();
    Game.Level = 0;
    Game.Score = 0;
}