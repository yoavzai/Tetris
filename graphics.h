#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "logic.h"

// number of pixels per block edge
// for transformation from small uint8_t game grid and side grid, to windows window
#define GAME_GRID_BLOCK_SIZE 30
#define SIDE_GRID_BLOCK_SIZE 15

void DrawWindow();

#endif // __GRAPHICS_H__