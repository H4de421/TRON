#include "Game/Game.h"

#include "Display/Animations.h"
#include "Display/Board.h"
#include "Display/Colors.h"
#include "Game/Player.h"
#include "Inputs.h"

void start_game(BoardContent *raw_args_board, Inputs_args *raw_args_input)
{
    String *buffer = raw_args_board->buffer;

    draw_basic_grid(raw_args_board->buffer, G_GRID_WIDTH, G_GRID_HEIGHT);
    int player_x = G_GRID_WIDTH / 8 + 1;
    int player_y = G_GRID_HEIGHT / 2 + 1;
    Player *player = create_player(player_x, player_y, PLAYER_COLOR);
    draw_player(player, buffer, raw_args_board->grid);

    struct timespec ts;
    // 125ms
    ts.tv_sec = 0;
    ts.tv_nsec = 125000000;

    // main loop
    int err = 0;
    while (!err && !STOPED)
    {
        err = move_player(player, buffer, raw_args_board->grid);
        // input managment
        pthread_mutex_lock(raw_args_input->input_mutex);
        apply_game_input(player, *raw_args_input->read_char);
        *raw_args_input->read_char = *raw_args_input->next_char;
        *raw_args_input->next_char = '\0';
        pthread_mutex_unlock(raw_args_input->input_mutex);

        nanosleep(&ts, NULL);
    }
    if (err)
        draw_colision_anim(player, buffer, raw_args_input->input_mutex, 2);

    STOPED = 2;
    destroy_player(player);
}
