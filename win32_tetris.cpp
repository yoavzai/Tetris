#include <windows.h>
#include <stdint.h>

#include "logic.h"
#include "graphics.h"
#include "init.h"


LRESULT CALLBACK Win32MainWindowCallback(
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
            HDC DeviceContext = BeginPaint(Window, &Paint);
            BeginPaint(Window, &Paint);
            DrawWindow();
            EndPaint(Window, &Paint);
            break;
        } 

        case WM_KEYDOWN:
        {
            uint32_t VKCode = WParam;
            SHORT state = GetAsyncKeyState(VKCode);
            
            /*
            3 things I wish to fix but proved to be hard and more time consuming than im willing to give.
            1) "state & 0x0001" means only single keystrokes. better was to allow movement also on holding
                for a long time.
            2) allow holding a movement key, simultaniously apply a rotation key when pressed,
                and then continue to apply the movement key if its still pressed.
            3) on high level speed, pressing a key slows down the falling of the piece.
                ive tried multithreading which worked, but the graphics looked worse.
            */
            switch (VKCode)
            {
                case ('X'):
                {
                    if(state & 0x0001)
                    {
                        if (Game.CurPiece.RotateClockwise(&Game.CurPiece))
                        {
                            DrawWindow();
                        }
                    }
                    break;
                }
                case ('Z'):
                {
                    if(state & 0x0001)
                    {
                        if (Game.CurPiece.RotateCounterClockwise(&Game.CurPiece))
                        {
                            DrawWindow();
                        }
                    }
                    break;
                }
                case (VK_DOWN):
                {
                    if (Game.CurPiece.PushDown(&Game.CurPiece))
                    {
                        Game.EndOfTurnTime = GetTickCount() + Game.LevelTimeTable[Game.Level];
                        DrawWindow();
                    }
                    break;
                }
                case (VK_LEFT):
                {
                    if(state & 0x0001)
                    {              
                        if (Game.CurPiece.MoveLeft(&Game.CurPiece))
                        {
                            DrawWindow();
                        }
                    }                        
                    break;
                }
                case (VK_RIGHT):
                {
                    if(state & 0x0001)
                    {                         
                        if (Game.CurPiece.MoveRight(&Game.CurPiece))
                        {
                            DrawWindow();
                        }
                    }                        
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
    InitGame(ProcInstance, Win32MainWindowCallback);

    MSG Message;
    while (Game.Running)
    {
        DrawWindow();
        Game.EndOfTurnTime = GetTickCount() + Game.LevelTimeTable[Game.Level];
        while (GetTickCount() < Game.EndOfTurnTime)
        {
            PeekMessage(&Message, 0, 0, 0, PM_REMOVE);
            if (Message.message == WM_QUIT)
            {
                Game.Running = false;
                break;
            }
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        EndOfTurnRoutine();
    }

    return 0;
}