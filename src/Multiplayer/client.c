#include "Multiplayer/client.h"

#include <arpa/inet.h>
#include <err.h>
#include <globals.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Multiplayer/network.h"
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int prepare_socket_client(char *ip, const char *port)
{
    char sname[INET6_ADDRSTRLEN];

    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo; // will point to the results

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me
    hints.ai_protocol = IPPROTO_TCP;

    if ((status = getaddrinfo(ip, port, &hints, &servinfo)) != 0)
    {
        printf("[client] GetAddrInfo error");
        return -1;
    }

    struct addrinfo *addrinfos = servinfo;

    int socket_f = -1;

    while (addrinfos != NULL)
    {
        int s = socket(addrinfos->ai_family, addrinfos->ai_socktype,
                       addrinfos->ai_protocol);
        if (s != -1)
        {
            int optval = 1;
            setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

            inet_ntop(servinfo->ai_family,
                      get_in_addr((struct sockaddr *)servinfo->ai_addr), sname,
                      sizeof sname);
            int error = connect(s, servinfo->ai_addr, servinfo->ai_addrlen);
            if (error == -1)
            {
                close(s);
                addrinfos = addrinfos->ai_next;
                continue;
            }
            socket_f = s;
            break;
        }
        addrinfos = addrinfos->ai_next;
    }

    freeaddrinfo(servinfo);

    return socket_f;
}

void client_init(BoardContent *args_board, int *stoped)
{
    (void)args_board;
    struct timespec ts;
    // 125ms
    ts.tv_sec = 0;
    ts.tv_nsec = 125000000;

    // server connection
    printf("[client] waiting for server to respond\n");
    fflush(stdout);
    int server_fd = prepare_socket_client(G_IP, G_PORT);
    int nb_try = 20;
    while (nb_try && server_fd == -1)
    {
        server_fd = prepare_socket_client(G_IP, G_PORT);
        printf("[client] connection failed retrying\n");
        nb_try--;
        nanosleep(&ts, NULL);
    }
    if (nb_try == 0)
    {
        printf("client error, can't connected to server");
        *stoped = 1;
    }

    G_IS_CLIENT = 1;
    G_SERVER_FD = server_fd;

    printf("[client] connected with socket %d\n", server_fd);
    // main loop
    while (!*stoped)
    {
        // wait for server notif
        int size = 64;
        char *buffer = malloc(size * sizeof(char));
        int size_msg = receve_data(&buffer, server_fd, &size);
        if (size_msg != -1)
            printf("[client] message recieved (len=%d) [%s]\n", size_msg,
                   buffer);
        free(buffer);
        if (size_msg == -1)
        {
            printf("server lost\n");
            *stoped = 1;
        }
        nanosleep(&ts, NULL);
    }
}
