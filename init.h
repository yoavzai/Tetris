#ifndef __INIT_H__
#define __INIT_H__

#include <windows.h>

void InitNewGame();
void InitGame(HINSTANCE ProcInstance, WNDPROC MessagesCallbackFunc);
void InitSideGrid();

#endif // __INIT_H__