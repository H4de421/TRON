#pragma once

#include "Utils/String.h"
#include "globals.h"
#include <pthread.h>
#include <time.h>
#include <stdio.h>

#define HEIGHT_ID_TO_DISPLAY_ID(ID) ((ID)*2-1)
#define WIDTH_ID_TO_DISPLAY_ID(ID) (((ID)-1)*5+1)
#define GRID_COORDINATE(x,y) ((y)*GRID_WIDTH+(x))

//#define GRID_WIDTH_default 25
//#define GRID_HEIGHT_default 15

#define FPS 60

typedef struct BoardContent {
    int width;
    int height;
    String *buffer;
    int *stoped;
    char *grid;
    pthread_mutex_t *bufferMutex;
} BoardContent ;

void clear_grid(int height);

char *create_grid(int width, int height);

void destroy_board(BoardContent *board);

void draw_borders(int width, int height);

void draw_basic_grid(String *buffer, int width, int height);

void restore_grid_tile(String *buffer, int x, int y);

void *updateBoardLoop(void *raw_args);

int adjust_grid_size(int *width, int *height);

