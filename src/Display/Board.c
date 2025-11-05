#include "Display/Board.h"

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "Display/Colors.h"
#include "globals.h"

void clear_grid(int height)
{
    for (int i = 0; i < height; i++)
    {
        printf("\e[%d;0H\e[K", i);
    }
}

int adjust_grid_size(int *width, int *height)
{
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    int new_width = ws.ws_col;
    int new_height = ws.ws_row;
    clear_grid(new_height);
    int border_H_size = 3;
    int border_W_size = 1;

    int minimal_grid_height = 19; // 10 row at least
    int minimal_grid_width = 25;

    if (new_height < minimal_grid_height + border_H_size
        || new_width < minimal_grid_width + border_W_size)
    {
        printf("\e[%d;%dHNot enougth space!", new_height / 2,
               (new_width / 2) - 9);
        printf("\e[%d;0H", new_height - 2);
        sleep(2);
        return 1;
    }
    else
    {
        draw_borders(new_width, new_height);
        *width = (((new_width - border_W_size) -1 ) / 5);
        *height = (((new_height - border_H_size) - 1) / 2);
    }
    return 0;
}
char *create_grid(int width, int height)
{
    return calloc(width * height, sizeof(char));
}

void destroy_board(BoardContent *board)
{
    destroyString(board->buffer);
    free(board->grid);
    free(board);
}

void draw_borders(int width, int height)
{
    int max_lines = HEIGHT_ID_TO_DISPLAY_ID(height);
    int max_cols = width - 1;
    clear_grid(max_lines);
    printf("%s", BORDER_COLOR);
    // draw border of the screen
    for (int i = 0; i < max_lines; i++)
    {
        printf("\e[%d;0H", i + 1);
        if (i == 0)
        {
            printf("%s", "╭");
            for (int j = 0; j < max_cols - 1; j++)
            {
                printf("%s", "─");
            }
            printf("%s", "╮");
        }
        else if (i == max_lines - 1)
        {
            printf("%s", "╰");
            for (int j = 0; j < max_cols - 1; j++)
            {
                printf("%s", "─");
            }
            printf("%s", "╯");
        }
        else
        {
            printf("%s", "│");
            for (int j = 0; j < max_cols - 1; j++)
            {
                printf("%c", ' ');
            }
            printf("%s", "│");
        }
    }

    // draw more info
    printf("\e[0;5H┐%sTRON%s┌%s", TITLE_COLOR, BORDER_COLOR, NO_COLOR);
}

void draw_basic_grid(String *buffer, int width, int height)
{
    int max_lines = HEIGHT_ID_TO_DISPLAY_ID(height);
    int max_cols = width - 1;

    for (int i = 0; i < max_lines; i++)
    {
        char str[30];
        sprintf(str, "\e[%d;%dH", i + SHIFT_HEIGHT + 1, SHIFT_WIDTH + 1);
        printf("%s", GRID_COLOR);
        addToString(buffer, str, 10);
        if (i % 2 == 0)
        {
            if (i == 0)
            {
                addToString(buffer, "┌", 3);
                for (int j = 0; j < max_cols; j++)
                {
                    if (j != max_cols - 1)
                        addToString(buffer, "────┬", 15);
                    else
                        addToString(buffer, "────┐", 15);
                }
                addToString(buffer, "\n", 1);
            }
            else if (i == max_lines - 1)
            {
                addToString(buffer, "└", 3);
                for (int j = 0; j < max_cols; j++)
                {
                    (j != max_cols - 1 ? addToString(buffer, "────┴", 15)
                                       : NULL);
                    (j == max_cols - 1 ? addToString(buffer, "────┘", 15)
                                       : NULL);
                }
                addToString(buffer, "\n", 1);
            }
            else
            {
                addToString(buffer, "├", 3);
                for (int j = 0; j < max_cols; j++)
                {
                    (j != max_cols - 1 ? addToString(buffer, "────┼", 15)
                                       : NULL);
                    (j == max_cols - 1 ? addToString(buffer, "────┤", 15)
                                       : NULL);
                }
                addToString(buffer, "\n", 1);
            }
        }
        else
        {
            addToString(buffer, "│", 3);
            for (int j = 0; j < max_cols; j++)
            {
                addToString(buffer, "    │", 15);
            }
            addToString(buffer, "\n", 1);
        }
    }
    // printf("%s", NO_COLOR);
}

void *updateBoardLoop(void *raw_args)
{
    struct BoardContent *args = raw_args;

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000000000 / FPS;

    while (!*args->stoped)
    {
        // lock buffer mutex
        pthread_mutex_lock(args->bufferMutex);
        // Draw_updates
        if (args->buffer->size != 0)
        {
            printf("%s", args->buffer->content);
            fflush(stdout);
            clearString(args->buffer);
        }
        // close mutex
        pthread_mutex_unlock(args->bufferMutex);
        // sleep until next loop
        nanosleep(&ts, NULL);
    }
    // flush the buffer a last time before exiting
    pthread_mutex_lock(args->bufferMutex);
    if (args->buffer->size != 0)
    {
        printf("%s", args->buffer->content);
        fflush(stdout);
        clearString(args->buffer);
    }
    pthread_mutex_unlock(args->bufferMutex);

    return NULL;
}
