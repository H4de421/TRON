#pragma once

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "Game/Player.h"
#include "Menu/MainMenu.h"

#define QUIT_INPUT 'p'

typedef struct inputsArgs {
    pthread_mutex_t *input_mutex;
    char *read_char;
    char *next_char;
    int *stop;
} Inputs_args;

typedef struct inputAssignment {
    char inputValue;
    void (*function)(int *stoped, void *args);
} inputAssignment;




void apply_game_input(Player *player, char input, int *stoped);

void apply_menu_input(Menu_config *menuConf, char input, int *stoped);

void *input_Handler(void *raw_args);

void input_quit(int *stoped, void *args);


void input_move_left(int *stoped, void *args);
void input_move_right(int *stoped, void *args);
void input_move_up(int *stoped, void *args);
void input_move_down(int *stoped, void *args);
void input_DEBUG(int *stoped, void *args);

void menu_input_up(int *stoped, void *args);
void menu_input_down(int *stoped, void *args);
void menu_input_enter(int *stoped, void *args);
void menu_input_return(int *stoped, void *args);


void client_move_right(int *stoped, void *args);
void client_move_left(int *stoped, void *args);
void client_move_down(int *stoped, void *args);
void client_move_up(int *stoped, void *args);
