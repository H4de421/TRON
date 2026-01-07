#include "Menu/MenuLoop.h"

#include <pthread.h>
#include <unistd.h>

#include "Display/MenuDisplay.h"
#include "Inputs.h"
#include "Menu/MainMenu.h"
#include "Utils/String.h"
#include "globals.h"

void menu_loop(String *buffer, GAME_STATE *state, Inputs_args *inputsArgs,
               int *stoped)
{
    struct timespec ts;
    // 125ms
    ts.tv_sec = 0;
    ts.tv_nsec = 125000000;

    display_menu(buffer, *state);

    Menu_config *menuConf = create_menu_config(state);

    while ((*state == MAIN_MENU || *state == MULTI_MENU) && !*stoped)
    {
        pthread_mutex_lock(inputsArgs->input_mutex);
        apply_menu_input(menuConf, *inputsArgs->read_char, stoped);
        *inputsArgs->read_char = *inputsArgs->next_char;
        *inputsArgs->next_char = '\0';
        pthread_mutex_unlock(inputsArgs->input_mutex);
        clear_input(buffer);
        display_menu_options(menuConf->cursor_position, buffer,
                             (*state == MULTI_MENU));

        nanosleep(&ts, NULL);
    }

    free(menuConf);
}
