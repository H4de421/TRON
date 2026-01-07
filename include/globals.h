#pragma once

#include <pthread.h>

typedef enum GAME_STATE {
    MAIN_MENU,
    MULTI_MENU,
    GAME,
    SERVER,
    CLIENT,
    STOP
} GAME_STATE;

/*----------------*\
| Canvas variables |
\*----------------*/
#define NB_SCENE 3
extern int G_CANVAS_HEIGHT;
extern int G_CANVAS_WIDTH;

/*----------------*\
| Menu variables   |
\*----------------*/
extern int G_MENU_OPTION_H_SIZE;
extern int G_MENU_OPTION_H_MARGIN; 

/*----------------*\
| Grid variables   |
\*----------------*/
extern int G_GRID_HEIGHT;
extern int G_GRID_WIDTH;
extern int G_SHIFT_WIDTH;
extern int G_SHIFT_HEIGHT;

/*----------------*\
| server variables |
\*----------------*/
extern char G_IP[20];
extern char G_PORT[5];
extern char G_IS_CLIENT;
extern char G_SERVER_FD;
extern int G_SOCKET;
extern char G_SERVER_LOGGING[20];
extern char G_CLIENT_LOGGING[20];
extern int G_PLAYER_ID;



