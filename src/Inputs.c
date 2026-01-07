#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>

#include "Game/Player.h"
#include "Menu/MainMenu.h"
#include "Multiplayer/network.h"

#define _XOPEN_SOURCE 500
#include "Inputs.h"
#include "globals.h"

#define NB_INPUTS 6
#define NB_MENU_OPTION 4
#define NB_MENU_MULTI 4

#define INPUT_THREASHOLD 7500000

#define MAX3(a, b, c) ((a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c))

static inputAssignment
    inputList[NB_SCENE][MAX3(NB_MENU_OPTION, NB_MENU_MULTI, NB_INPUTS)] = {
        {
            { .inputValue = 'z', .function = &menu_input_up },
            { .inputValue = 's', .function = &menu_input_down },
            { .inputValue = 'e', .function = &menu_input_enter },
            { .inputValue = 'a', .function = &menu_input_return },
        },
        {
            { .inputValue = 'z', .function = &multi_input_up },
            { .inputValue = 's', .function = &multi_input_down },
            { .inputValue = 'e', .function = &multi_input_enter },
            { .inputValue = 'a', .function = &multi_input_return },
        },
        {
            { .inputValue = 'p', .function = &input_quit },
            { .inputValue = 'd', .function = &input_move_right },
            { .inputValue = 'q', .function = &input_move_left },
            { .inputValue = 'z', .function = &input_move_up },
            { .inputValue = 's', .function = &input_move_down },
            { .inputValue = 'o', .function = &input_DEBUG },
        }
    };

void *input_Handler(void *raw_args)
{
    struct timespec ts;
    // 75ms
    ts.tv_sec = 0;
    ts.tv_nsec = 75000000;

    Inputs_args *arg = raw_args;
    while (!*arg->stop)
    {
        char input = ' ';
        scanf(" %c", &input);
        if (input >= 'a' && input <= 'z')
        {
            if (G_IS_CLIENT)
            {
                send_message(IN, G_SERVER_FD, "%d\n", input);
            }
            else
            {
                pthread_mutex_lock(arg->input_mutex);
                if (*arg->read_char == '\0')
                    *arg->read_char = input;
                else
                    *arg->next_char = input;
                pthread_mutex_unlock(arg->input_mutex);
            }
        }
        if (input == QUIT_INPUT)
        {
            *arg->stop = 1;
        }
        else
        {
            nanosleep(&ts, NULL);
        }
    }
    return NULL;
}

/*-------------------*\
|  Game relataed code |
\*-------------------*/

void apply_game_input(Player *player, char input, int *stoped)
{
    for (int i = 0; i < NB_INPUTS; i++)
    {
        if (inputList[GAME][i].inputValue == input)
        {
            inputList[GAME][i].function(stoped, player);
        }
    }
}

void apply_client_input(int *server_fd, char input, int *stoped)
{
    for (int i = 0; i < NB_INPUTS; i++)
    {
        if (inputList[GAME][i].inputValue == input)
        {
            inputList[GAME][i].function(stoped, server_fd);
        }
    }
}

/*-------------------*\
|  Inputs functions   |
\*-------------------*/

void input_quit(int *stoped, void *args)
{
    (void)args;
    *stoped = 1;
}

void input_move_left(int *stoped, void *args)
{
    (void)stoped;
    Player *player = args;
    if (player->dir != LEFT && player->dir != RIGHT && !player->colision)
    {
        player->dir = LEFT;
    }
}
void input_move_right(int *stoped, void *args)
{
    (void)stoped;
    Player *player = args;
    if (player->dir != LEFT && player->dir != RIGHT && !player->colision)
    {
        player->dir = RIGHT;
    }
}
void input_move_up(int *stoped, void *args)
{
    (void)stoped;
    Player *player = args;
    if (player->dir != UP && player->dir != DOWN && !player->colision)
    {
        player->dir = UP;
    }
}
void input_move_down(int *stoped, void *args)
{
    (void)stoped;
    Player *player = args;
    if (player->dir != UP && player->dir != DOWN && !player->colision)
    {
        player->dir = DOWN;
    }
}

void input_DEBUG(int *stoped, void *args)
{
    (void)stoped;
    (void)args;
}

/*-------------------*\
|  menu relataed code |
\*-------------------*/

void apply_menu_input(Menu_config *menuConf, char input, int *stoped)
{
    struct timespec temp_time;
    clock_gettime(CLOCK_REALTIME, &temp_time);
    if ((temp_time.tv_nsec - menuConf->last_input.tv_nsec < INPUT_THREASHOLD)
        && temp_time.tv_sec == menuConf->last_input.tv_sec)
    {
        return;
    }

    menuConf->last_input.tv_sec = temp_time.tv_sec;
    menuConf->last_input.tv_nsec = temp_time.tv_nsec;

    if (*menuConf->state != MULTI_MENU && *menuConf->state != MAIN_MENU)
        return;

    for (int i = 0; i < NB_MENU_OPTION; i++)
    {
        if (inputList[*menuConf->state][i].inputValue == input)
        {
            inputList[*menuConf->state][i].function(stoped, menuConf);
            break;
        }
    }
}

/*------------------------*\
|  Menu Inputs functions   |
\*------------------------*/

void menu_input_down(int *stoped, void *args)
{
    (void)stoped;
    Menu_config *menuConf = args;
    menuConf->cursor_position =
        (menuConf->cursor_position + 1) % menuConf->cursor_max_value;
}

void menu_input_up(int *stoped, void *args)
{
    (void)stoped;
    Menu_config *menuConf = args;
    menuConf->cursor_position =
        (menuConf->cursor_max_value + menuConf->cursor_position - 1)
        % menuConf->cursor_max_value;
}

void menu_input_enter(int *stoped, void *args)
{
    (void)stoped;
    Menu_config *menuConf = args;
    switch (menuConf->cursor_position)
    {
    case 0:
        *menuConf->state = GAME;
        break;
    case 1:
        *menuConf->state = MULTI_MENU;
        break;
    case 2:
        *menuConf->state = STOP;
        break;
    default:
        break;
    }
}

void menu_input_return(int *stoped, void *args)
{
    // return ?
    (void)args;
    *stoped = 2;
}

/*------------------------*\
|  Multi Inputs functions  |
\*------------------------*/

void multi_input_up(int *stoped, void *args)
{
    (void)stoped;
    Menu_config *menuConf = args;
    menuConf->cursor_position =
        (menuConf->cursor_max_value + menuConf->cursor_position - 1)
        % menuConf->cursor_max_value;
}

void multi_input_down(int *stoped, void *args)
{
    (void)stoped;
    Menu_config *menuConf = args;
    menuConf->cursor_position =
        (menuConf->cursor_position + 1) % menuConf->cursor_max_value;
}

void multi_input_enter(int *stoped, void *args)
{
    (void)stoped;
    Menu_config *menuConf = args;
    switch (menuConf->cursor_position)
    {
    case 0:
        // server
        *menuConf->state = SERVER;
        break;
    case 1:
        // client
        *menuConf->state = CLIENT;
        break;
    case 2:
        *menuConf->state = MAIN_MENU;
        break;
    default:
        break;
    }
}
void multi_input_return(int *stoped, void *args)
{
    (void)stoped;
    Menu_config *menuConf = args;
    *menuConf->state = MAIN_MENU;
}
