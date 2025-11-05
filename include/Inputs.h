#pragma once

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "Game/Player.h"

#define QUIT_INPUT 'p'

struct inputsArgs {
    pthread_mutex_t *input_mutex;
    char *read_char;
    char *next_char;
    int *stop;
};

struct inputAssignment {
    char inputValue;
    void (*function)(Player *player, int *stoped);
};

void apply_game_input(Player *player, char input, int *stoped);


void *input_Handler(void *raw_args);


void input_quit(Player *player, int *stoped);


void input_move_left(Player *player, int *stoped);
void input_move_right(Player *player, int *stoped);
void input_move_up(Player *player, int *stoped);
void input_move_down(Player *player, int *stoped);
void input_DEBUG(Player *player, int *stoped);

