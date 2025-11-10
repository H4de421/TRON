#pragma once

#include "Utils/String.h"

#define mainTitleMaxSize 600

#define NB_MENU_OPTION 2

#define MAIN_TITLE_HEIGHT 4
#define MAIN_TITLE_WIDTH 42

typedef struct MenuOption {
    char name[35];
} MenuOption ;  


void display_menu(String *buffer);
void display_main_title(String *buffer, int x, int y);


