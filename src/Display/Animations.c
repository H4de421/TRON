#include "Display/Animations.h"

#include <bits/pthreadtypes.h>
#include <stdio.h>

#include "Display/Board.h"
#include "Display/Colors.h"

void draw_colision_anim(Player *player, String *buffer, pthread_mutex_t *mutex,
                        int duration)
{
    Dir dir = player->dir;
    int grid_x = player->grid_x + (dir == RIGHT ? 1 : (dir == LEFT ? -1 : 0));
    int grid_y = player->grid_y + (dir == DOWN ? 1 : (dir == UP ? -1 : 0));
    if (grid_x < 0 || grid_x >= GRID_WIDTH)
        grid_x -= (dir == RIGHT ? 1 : (dir == LEFT ? -1 : 0));
    if (grid_y < 0 || grid_y >= GRID_HEIGHT)
        grid_y -= (dir == DOWN ? 1 : (dir == UP ? -1 : 0));

    int shift_w = SHIFT_WIDTH + WIDTH_ID_TO_DISPLAY_ID((grid_x + 1));
    int shift_h = SHIFT_HEIGHT + HEIGHT_ID_TO_DISPLAY_ID((grid_y + 1));

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 200000000;

    int nb_draw = duration / 0.2;
    char temp[60];

    for (int i = 0; i < nb_draw; i++)
    {
        sprintf(temp, "\e[%d;%dH%s■\e[0m", shift_h, shift_w,
                (i % 2 == 0 ? PLAYER_COLOR : PLAYER_EFFECT_COLOR));
        pthread_mutex_lock(mutex);
        addToString(buffer, temp, 50);
        pthread_mutex_unlock(mutex);

        nanosleep(&ts, NULL);
    }
}
