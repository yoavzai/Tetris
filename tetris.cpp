#include <windows.h>
#include <stdint.h>

#include "logic.h"
#include "graphics.h"
#include "init.h"


LRESULT CALLBACK MainWindowCallback(
    HWND   Window,
    UINT   Message,
    WPARAM WParam,
    LPARAM LParam)
{
    LRESULT result;

    switch (Message)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT Paint = {};
            BeginPaint(Window, &Paint);
            if (Game.Over)
            {
                DisplayMenu();
            }
            else
            {
                DisplayGame();
            }
            EndPaint(Window, &Paint);
            break;
        } 

        case WM_KEYDOWN:
        {
            uint32_t VKCode = WParam;
            bool SingleKeystroke = GetAsyncKeyState(VKCode) & 0x0001;
            
            /*
            3 things I wish to fix but proved to be hard:
            1) instead of only applying SingleKeystroke, better was to allow movement also on holding
                for a long time.
            2) allow holding a movement key, simultaniously apply a rotation key when pressed,
                and then continue to apply the movement key if its still pressed.
            3) on high level speed, pressing a key slows down the falling of the piece.
                thats because key stroke messages are dispatched inside the turn time loop, 
                and while they are being processed the turn time is over, and only when they return
                the loop is ended.
                ive tried multithreading which worked, but the graphics looked worse.
            */
           
            switch (VKCode)
            {
                case ('P'):
                {
                    if(SingleKeystroke)
                    {
                        DisplayMenu();
                        DisplayPauseBox();  
                    }
                    break;
                }

                case ('X'):
                {
                    if (SingleKeystroke)
                    {
                        Game.CurPiece.RotateClockwise(&Game.CurPiece);
                        DisplayGame();
                    }
                    break;
                }
                case ('Z'):
                {
                    if(SingleKeystroke)
                    {
                        Game.CurPiece.RotateCounterClockwise(&Game.CurPiece);
                        DisplayGame();
                    }
                    break;
                }
                case (VK_UP):
                {
                    if(SingleKeystroke)
                    {
                        Game.CurPiece.RotateCounterClockwise(&Game.CurPiece);
                        DisplayGame();
                    }
                    break;
                }
                case (VK_DOWN):
                {
                    if (Game.CurPiece.PushDown(&Game.CurPiece))
                    {
                        Game.EndOfTurnTime = GetTickCount() + Game.LevelTimeTable[Game.Level];
                        DisplayGame();
                    }
                    break;
                }
                case (VK_LEFT):
                {
                    if(SingleKeystroke)
                    {        
                        Game.CurPiece.MoveLeft(&Game.CurPiece);      
                        DisplayGame();
                    }                        
                    break;
                }
                case (VK_RIGHT):
                {
                    if(SingleKeystroke)
                    {
                        Game.CurPiece.MoveRight(&Game.CurPiece);
                        DisplayGame();
                    }                        
                    break;
                }
                case VK_ESCAPE:
                {
                    PostQuitMessage(0);
                    break;
                }
            }
            break;
        }

        case WM_DESTROY:
        {PostQuitMessage(0);break;}

        case WM_CLOSE:
        {PostQuitMessage(0);break;}

        default:
        {result = DefWindowProc(Window, Message, WParam, LParam);break;} 
    }

  return result;
}

int CALLBACK WinMain(
    HINSTANCE ProcInstance,
    HINSTANCE PrevInstance,
    LPSTR     CmdLine,
    int       ShowCmd)
{
    InitGame(ProcInstance, MainWindowCallback);

    MSG Message;
    while (Game.Running)
    {
        NewGameRoutine();
        while (!Game.Over)
        {
            DisplayGame();
            Game.EndOfTurnTime = GetTickCount() + Game.LevelTimeTable[Game.Level];
            while (GetTickCount() < Game.EndOfTurnTime)
            {
                PeekMessage(&Message, 0, 0, 0, PM_REMOVE);
                if (Message.message == WM_QUIT)
                {
                    Game.Over = true;
                    Game.Running = false;
                    break;
                }
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }
            EndOfTurnRoutine();
        }
        if (!(Message.message == WM_QUIT))
        {
            GameOverRoutine();
        }
    }

    return 0;
}