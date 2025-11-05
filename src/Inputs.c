#include <stdio.h>
#include "Player.h"
#define _XOPEN_SOURCE 500
#include "Inputs.h"

#define NB_INPUTS 6

static struct inputAssignment inputList[NB_INPUTS] = {
    { .inputValue = 'p', .function = &input_quit },
    { .inputValue = 'd', .function = &input_move_right },
    { .inputValue = 'q', .function = &input_move_left },
    { .inputValue = 'z', .function = &input_move_up },
    { .inputValue = 's', .function = &input_move_down },
    { .inputValue = 'o', .function = &input_DEBUG },
};

void apply_input(Player *player, char input, int *stoped)
{
    for (int i = 0; i < NB_INPUTS; i++)
    {
        if (inputList[i].inputValue == input)
        {
            inputList[i].function(player, stoped);
        }
    }
}

void *input_Handler(void *raw_args)
{
    struct timespec ts;
    // 75ms
    ts.tv_sec = 0;
    ts.tv_nsec = 75000000;

    struct inputsArgs *arg = raw_args;
    while (!*arg->stop)
    {
        char input = ' ';
        scanf(" %c", &input);
        if (input >= 'a' && input <= 'z')
        {
            pthread_mutex_lock(arg->input_mutex);
            if (*arg->read_char == '\0')
                *arg->read_char = input;
            else
                *arg->next_char = input;
            pthread_mutex_unlock(arg->input_mutex);
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
    // busy wait looking for inputs...
}

/*-------------------*\
|  Inputs functions   |
\*-------------------*/

void input_quit(Player *player, int *stoped)
{
    (void)player;
    *stoped = 1;
}

void input_move_left(Player *player, int *stoped)
{
    (void)stoped;
    if (player->dir != LEFT && player->dir != RIGHT
        && !player->colision)
    {
        player->dir = LEFT;
    }
}
void input_move_right(Player *player, int *stoped)
{
    (void)stoped;
    if (player->dir != LEFT && player->dir != RIGHT
        && !player->colision)
    {
        player->dir = RIGHT;
    }
}
void input_move_up(Player *player, int *stoped)
{
    (void)stoped;
    if (player->dir != UP && player->dir != DOWN
        && !player->colision)
    {
        player->dir = UP;
    }
}
void input_move_down(Player *player, int *stoped)
{
    (void)stoped;
    if (player->dir != UP && player->dir != DOWN
        && !player->colision)
    {
        player->dir = DOWN;
    }
}

void input_DEBUG(Player *player, int *stoped)
{
    (void)stoped;
    (void)player;
}
