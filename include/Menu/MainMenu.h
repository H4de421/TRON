#pragma once

#include "globals.h"
#include "time.h"

typedef struct Menu_config {
    int cursor_position;
    int cursor_max_value;
    GAME_STATE *state;
    struct timespec last_input;
} Menu_config; 


Menu_config *create_menu_config(GAME_STATE *state);


