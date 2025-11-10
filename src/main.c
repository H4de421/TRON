#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "Display/Board.h"
#include "Game/Game.h"
#include "Inputs.h"
#include "globals.h"
#include "Display/Menu.h"

struct termios old_termios;

void reset_terminal(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_termios);
    printf("\e[?25h"); // Affiche le curseur
    // move cursor uneder grid
    clear_grid(HEIGHT_ID_TO_DISPLAY_ID(GRID_WIDTH));
    printf("\e[0;0H");

    fflush(stdout);
}

void handle_sigint(int sig)
{
    (void)sig;
    reset_terminal();
    exit(0);
}

int main(void)
{
    /*--------------------*\
    | grid size adjustment |
    \*--------------------*/

    int adjusted = adjust_grid_size(&GRID_WIDTH, &GRID_HEIGHT);
    if (adjusted)
    {
        return 2;
    }
    /*---------------------*\
    | variables definitions |
    \*---------------------*/

    int stoped = 0;
    char read_char = '\0';
    char next_char = '\0';
    pthread_mutex_t input_mutex;
    pthread_mutex_t dispaly_mutex;
    pthread_mutex_init(&input_mutex, NULL);
    pthread_mutex_init(&dispaly_mutex, NULL);

    /*--------------*\
    | terminal setup |
    \*--------------*/

    // save current terminal state
    struct termios new_termios;

    tcgetattr(STDIN_FILENO, &old_termios);

    atexit(reset_terminal);
    signal(SIGINT, handle_sigint);

    // disable "echo" and canonic mode (allow a simple input an not phrases)
    new_termios = old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios);

    // set stdin to non-blocking (may be redondant)
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    // hiding cursor
    printf("\e[?25l");
    fflush(stdout);

    /*--------------*\
    | display buffer |
    \*--------------*/

    String *buffer = createString(30, &dispaly_mutex);

    /*--------------*\
    | input thread   |
    \*--------------*/

    // input thread preparation
    // TODO RENAME THIS STRUCT
    struct inputsArgs *raw_args_input = malloc(sizeof(struct inputsArgs));
    raw_args_input->stop = &stoped;
    raw_args_input->read_char = &read_char;
    raw_args_input->next_char = &next_char;
    raw_args_input->input_mutex = &input_mutex;

    pthread_t input_thread;
    pthread_create(&input_thread, NULL, input_Handler, raw_args_input);

    /*--------------*\
    | display thread |
    \*--------------*/

    BoardContent *raw_args_board = malloc(sizeof(BoardContent));
    raw_args_board->width = GRID_WIDTH;
    raw_args_board->height = GRID_HEIGHT;
    raw_args_board->stoped = &stoped;
    raw_args_board->bufferMutex = &dispaly_mutex;
    raw_args_board->buffer = buffer;
    raw_args_board->grid = create_grid(GRID_WIDTH, GRID_HEIGHT);

    pthread_t display_thread;
    pthread_create(&display_thread, NULL, updateBoardLoop, raw_args_board);

    /*--------------------*\
    | Game section         |
    \*--------------------*/

    display_menu(buffer);
    //start_game(raw_args_board, raw_args_input, &stoped);

    /*--------------------*\
    | end Game section     |
    \*--------------------*/

    pthread_join(display_thread, NULL);
    pthread_join(input_thread, NULL);
    destroy_board(raw_args_board);

    free(raw_args_input);
    clear_grid(HEIGHT_ID_TO_DISPLAY_ID(GRID_WIDTH));
    printf("\e[0;0H");
    fflush(stdout);
    return 0;
}
