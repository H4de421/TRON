#pragma once

#define _POSIX_C_SOURCE 200809L
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

int is_alive(int sock);

int send_data(char *buffer, int socket, int size);

int receve_data(char **buffer, int socket, int *buffer_size);

int create_and_bind(struct addrinfo *addrinfo);

int prepare_socket(const char *ip, const char *port);

int accept_client(int socket);


