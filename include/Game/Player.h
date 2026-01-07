#pragma once 

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Game/Dir.h"

#include "Utils/String.h"
#include "Display/Board.h"


typedef struct TrailNode {
    int grid_x;
    int grid_y;
    struct TrailNode *next;
} TrailNode;

typedef struct player {
    int grid_x;
    int grid_y;
    char *color;
    Dir dir;
    Dir old_dir;
    int num;
    int colision;
} Player;


Player *create_player(int x, int y, char str[]);

void destroy_player(Player *player);

int move_player(Player *player, String *buffer, char *grid);

void draw_player(Player *player, String *buffer, char *grid);

void draw_trail(Player *player, String *buffer);

