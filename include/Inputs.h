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
} Inputs_args;

typedef struct inputAssignment {
    char inputValue;
    void (*function)(void *args);
} inputAssignment;




void apply_game_input(Player *player, char input);

void apply_menu_input(Menu_config *menuConf, char input);

void *input_Handler(void *raw_args);

void input_quit(void *args);


void input_move_left(void *args);
void input_move_right(void *args);
void input_move_up(void *args);
void input_move_down(void *args);
void input_DEBUG(void *args);

void menu_input_up(void *args);
void menu_input_down(void *args);
void menu_input_enter(void *args);
void menu_input_return(void *args);

void multi_input_up(void *args);
void multi_input_down(void *args); 
void multi_input_enter(void *args); 
void multi_input_return(void *args); 

void client_move_right(void *args);
void client_move_left(void *args);
void client_move_down(void *args);
void client_move_up(void *args);
