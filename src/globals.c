#include "globals.h"

// define ddefault value
// thoses will be overwrite at the start of the programs

/*----------------*\
| Canvas variables |
\*----------------*/
int G_CANVAS_HEIGHT = 25;
int G_CANVAS_WIDTH = 50;

/*----------------*\
| Menu variables   |
\*----------------*/
int G_MENU_OPTION_H_SIZE = 1;
int G_MENU_OPTION_H_MARGIN = 5;

/*----------------*\
| Grid variables   |
\*----------------*/
int G_GRID_HEIGHT = 10;
int G_GRID_WIDTH = 5;
int G_SHIFT_WIDTH = 3;
int G_SHIFT_HEIGHT = 3;

/*------------------*\
| Server variables   |
\*------------------*/
char G_IP[20] = "localhost";
char G_PORT[5] = "4214";
char G_IS_CLIENT = 0;
char G_SERVER_FD = 0;
