#pragma once

typedef enum GAME_STATE {
    MAIN_MENU,
    GAME,
    STOP
} GAME_STATE;

/*----------------*\
| Canvas variables |
\*----------------*/
#define NB_SCENE 2
extern int CANVAS_HEIGHT;
extern int CANVAS_WIDTH;

/*----------------*\
| Menu variables   |
\*----------------*/
extern int MENU_OPTION_H_SIZE;
extern int MENU_OPTION_H_MARGIN; 

/*----------------*\
| Grid variables   |
\*----------------*/
extern int GRID_HEIGHT;
extern int GRID_WIDTH;
extern int SHIFT_WIDTH;
extern int SHIFT_HEIGHT;
