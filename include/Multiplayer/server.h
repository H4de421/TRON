#pragma once
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include "Game/Player.h"

struct server_listen_args {
    int *stoped;
    int socket_fd;
    int player_1;
    int player_2;
    Dir *dir_1;
    pthread_mutex_t *m_dir_1;
    Dir *dir_2;
    pthread_mutex_t *m_dir_2;
};


void *server_listen(void *raw_args);
void server_init(int *stoped);
void launch_multi(int *stoped, BoardContent *board_args);

