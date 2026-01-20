#pragma once
#include "Inputs.h"

void client_loop(int server_fd, BoardContent *args_board);

void client_init(BoardContent *raw_args_board);

