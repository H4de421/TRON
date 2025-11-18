#include "Menu/MainMenu.h"

#include <pthread.h>
#include "Display/MenuDisplay.h"
#include "globals.h"

Menu_config *create_menu_config(GAME_STATE *state)
{
    Menu_config *res = calloc(1, sizeof(Menu_config));
    res->cursor_position = 0;
    res->cursor_max_value = NB_MENU_OPTION;
    res->state = state;
    clock_gettime(CLOCK_REALTIME,&res->last_input);
    return res;
}

