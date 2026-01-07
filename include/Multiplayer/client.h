#pragma once
#include "Inputs.h"

void client_loop(int server_fd, int *stoped, BoardContent *args_board);

void client_init(BoardContent *raw_args_board, int *stoped);

