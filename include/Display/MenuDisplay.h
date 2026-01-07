#pragma once

#include "Utils/String.h"

#include "globals.h"

#define mainTitleMaxSize 600

#define NB_MENU_OPTION 3

#define MAIN_TITLE_HEIGHT 4
#define MAIN_TITLE_WIDTH 42

typedef struct MenuOption {
    char *name;
    char *color;
} MenuOption ;  


void clear_input(String *buffer);
void display_menu(String *buffer, GAME_STATE state);
void display_main_title(String *buffer, int x, int y);
void display_menu_options(int cursor, String *buffer, int menuIdx);

