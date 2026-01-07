#pragma once

#define _POSIX_C_SOURCE 200809L
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "Game/Dir.h"

enum message_type {
    INIT,
    SIZE,
    TICK,
    START,
    IN,
    ERROR
};

typedef struct init_enum {
    int player_id;
    int max_c;
    int max_l;
    int p1_x;
    int p1_y;
    int p2_x;
    int p2_y;

} init_enum;

typedef struct tick_enum {
    int p1_d;
    int p2_d;
} tick_enum;

typedef struct in_enum {
    Dir dir;
} in_enum;

typedef struct size_enum {
    int nb_col;
    int nb_lin;
} size_enum;

enum message_type parse_method(char *methode);

void send_message(enum message_type method, int fd, char *format, ...);

init_enum *parse_INIT(char *content);
in_enum *parse_IN(char *content);
tick_enum *parse_TICK(char *content);
size_enum *parse_server_SIZE(char *content);

int is_alive(int sock);

int send_data(char *buffer, int socket, int size);

int receve_data(char **buffer, int socket, int *buffer_size);

int create_and_bind(struct addrinfo *addrinfo);

int prepare_socket(const char *ip, const char *port);

int accept_client(int socket);

void prepare_logging(char *name, int fd);

