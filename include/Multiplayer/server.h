#pragma once
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include "Game/Player.h"

typedef struct server_listen_args {
    int socket_fd;
    int player_1;
    int player_2;
    Dir *dir_1;
    pthread_mutex_t *m_dir_1;
    Dir *dir_2;
    pthread_mutex_t *m_dir_2;
    int map_col_p1;
    int map_lin_p1;
    int map_col_p2;
    int map_lin_p2;
} Server_listen_args;


void *server_listen(void *raw_args);
void server_init(void);
void launch_multi(BoardContent *board_args);

