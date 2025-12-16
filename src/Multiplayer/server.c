#include "Multiplayer/server.h"

#include <err.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "Display/Board.h"
#include "Game/Player.h"
#include "Multiplayer/client.h"
#include "Multiplayer/network.h"
#include "globals.h"

#define EPOLL_QUEUE_LEN 2
#define MAX_EPOLL_EVENTS_PER_RUN 6
#define EPOLL_RUN_TIMEOUT -1

void *server_listen(void *raw_args)
{
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 25000000;

    struct server_listen_args *args = raw_args;
    int socket_fd = args->socket_fd;
    int player_1 = args->player_1;
    int player_2 = args->player_2;

    int epfd = epoll_create(EPOLL_QUEUE_LEN);

    static struct epoll_event ev_p1;
    ev_p1.events = EPOLLIN | EPOLLERR;
    ev_p1.data.fd = player_1;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, player_1, &ev_p1) != 0)
    {
        close(player_1);
        close(player_2);
        close(socket_fd);
        *args->stoped = 1;
        printf("error while adding p1 to epoll\n");
    }

    static struct epoll_event ev_p2;
    ev_p2.events = EPOLLIN | EPOLLERR;
    ev_p2.data.fd = player_2;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, player_2, &ev_p2) != 0)
    {
        close(player_1);
        close(player_2);
        close(socket_fd);
        *args->stoped = 1;
        printf("error while adding p2 to epoll\n");
    }

    static struct epoll_event events[6];

    // listen loop
    while (!*args->stoped)
    {
        int nfds = epoll_wait(epfd, events, MAX_EPOLL_EVENTS_PER_RUN, 0);
        if (nfds < 0)
        {
            *args->stoped = 1;
            printf("Error in epoll_wait!\n");
        }

        for (int i = 0; i < nfds; i++)
        {
            int fd = events[i].data.fd;

            if (events[i].events & EPOLLERR)
            {
                epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &events[i]);
                printf("[serv] lost p%d\n", i + 1);
                *args->stoped = 1;
            }
            if (events[i].events & EPOLLIN)
            {
                // TODO LOGS
                char *buffer = malloc(64);
                int size = 0;
                printf("trying to read from p%d\n", events[i].data.fd);
                int size_msg = receve_data(&buffer, events[i].data.fd, &size);
                free(buffer);
                if (size_msg != -1)
                    printf("[server]log receved from p%d\n", i + 1);
                else
                    *args->stoped = 1;
            }
        }
        nanosleep(&ts, NULL);
    }
    close(socket_fd);
    close(player_1);
    close(player_2);
    return NULL;
}

void waiting_2_players(int sockets[2], int socket_fd, int *stoped)
{
    // wait for 2 players to connect
    while (!*stoped)
    {
        int nb_connected_players = 0;

        while (nb_connected_players != 2)
        {
            printf("[serv] there is %d player in the room\n",
                   nb_connected_players);
            int socket_p = accept4(socket_fd, NULL, NULL, SOCK_NONBLOCK);
            if (socket_p == -1)
            {
                printf("[serv] accept gone wrong\n");
                continue;
            }

            sockets[nb_connected_players] = socket_p;
            nb_connected_players++;
            printf("[serv] new player accepted: pid %d\n", socket_p);
            sleep(1);
        }
        if (is_alive(sockets[0]) && is_alive(sockets[1]))
            break;
        close(sockets[0]);
        close(sockets[1]);
    }
}

void server_init(int *stoped)
{
    // launch socket
    int socket_fd = prepare_socket(G_IP, G_PORT);
    printf("server created with %s %s socket is %d\n", G_IP, G_PORT, socket_fd);
    int sockets[2] = { 0 };

    // waiting for 2 player
    waiting_2_players(sockets, socket_fd, stoped);

    printf("----------------------------------------------------------\n"
           "[serv] all players connected launching game\n"
           "----------------------------------------------------------\n");
    // preparing players data
    Dir p1;
    pthread_mutex_t m_dir_p1;
    pthread_mutex_init(&m_dir_p1, NULL);
    Dir p2;
    pthread_mutex_t m_dir_p2;
    pthread_mutex_init(&m_dir_p2, NULL);

    // preparing listen thread arguments
    struct server_listen_args *raw_args =
        malloc(sizeof(struct server_listen_args));
    raw_args->stoped = stoped;
    raw_args->socket_fd = socket_fd;
    raw_args->player_1 = sockets[0];
    raw_args->player_2 = sockets[1];
    raw_args->dir_1 = &p1;
    raw_args->m_dir_1 = &m_dir_p1;
    raw_args->dir_2 = &p2;
    raw_args->m_dir_1 = &m_dir_p1;

    // launch listening thread.
    pthread_t listen_thread;
    pthread_create(&listen_thread, NULL, server_listen, raw_args);

    // launch game computing thread;
    printf("[serv] sending data\n");
    send_data("u r p1\n", sockets[0], 7);
    send_data("u r p2\n", sockets[1], 7);

    while (!*stoped)
    {
        sleep(1);
    }

    pthread_join(listen_thread, NULL);
    free(raw_args);
}

void launch_multi(int *stoped, BoardContent *board_args)
{
    fflush(NULL);
    int pid = fork();

    switch (pid)
    {
    case -1:
        err(1, "Can't create server\n");
        break;
    case 0:
        // child
        printf("[serv] launching server_init\n");
        server_init(stoped);
        exit(0);
        break;
    default:
        // parent
        printf("[client] launching client_init\n");
        client_init(board_args, stoped);
        break;
    }
    waitpid(pid, NULL, 0);
    printf("server exited normaly");
    sleep(3);
}
