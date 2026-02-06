#include "Multiplayer/client.h"

#include <arpa/inet.h>
#include <err.h>
#include <globals.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "Display/Board.h"
#include "Display/Colors.h"
#include "Game/Dir.h"
#include "Game/Player.h"
#include "Multiplayer/network.h"
#include "Utils/pretty_printer.h"
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
        fprintf(stderr, "[client] GetAddrInfo error");
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

static void handle_message(char *buffer, int size_msg, Player **p1, Player **p2,
                           BoardContent *args_board)
{
    if (buffer == NULL)
        return;
    size_msg++;
    char *strsave = NULL;
    enum message_type type = parse_method(strtok_r(buffer, ";", &strsave));
    fprintf(stderr, "[client%d] message identified as %d\n", G_PLAYER_ID, type);
    switch (type)
    {
    case INIT: {
        // recieve the player id + players pos
        fprintf(stderr, "[client] recived INIT\n");
        init_enum *init = parse_INIT(strsave);
        G_PLAYER_ID = init->player_id;
        G_GRID_HEIGHT = init->max_l;
        G_GRID_WIDTH = init->max_c;

        print_init_struct(init, stderr);

        *p1 = create_player(init->p1_x, init->p1_y,
                            (init->player_id == 1 ? PLAYER_COLOR : AI_COLOR));
        *p2 = create_player(init->p2_x, init->p2_y,
                            (init->player_id == 2 ? PLAYER_COLOR : AI_COLOR));
        // draw map + players

        free(args_board->grid);
        args_board->grid = create_grid(G_GRID_HEIGHT, G_GRID_HEIGHT);
        draw_basic_grid(args_board->buffer, G_GRID_WIDTH, G_GRID_HEIGHT);
        draw_player(*p1, args_board->buffer);
        draw_player(*p2, args_board->buffer);
        free(init);
        break;
    }
    case SIZE: {
        // compute max map size
        fprintf(stderr, "[client] recived size\n");
        fprintf(stderr, "[client] computed size = %d, %d\n\n\n", G_GRID_WIDTH,
                G_GRID_HEIGHT);
        send_message(SIZE, G_SERVER_FD, "%d;%d", G_GRID_WIDTH, G_GRID_HEIGHT);
        break;
    }
    case TICK: {
        // update map of players
        tick_enum *tick = parse_TICK(strsave);
        fprintf(stderr, "[client] recived TICK dir p1 = %d, dir p2 = %d\n",
                tick->p1_d, tick->p2_d);
        (*p1)->old_dir = (*p1)->dir;
        (*p1)->dir = tick->p1_d;
        (*p2)->old_dir = (*p2)->dir;
        (*p2)->dir = tick->p2_d;
        fprintf(stderr, "pos before %d %d\n", (*p1)->grid_x, (*p1)->grid_y);
        // TODO FIX THIS no modif on P1 ??? why ....
        MULTI_STOPPED = move_player((*p1), args_board->buffer, args_board->grid)
            || move_player((*p2), args_board->buffer, args_board->grid);
        fprintf(stderr, "pos after  %d %d\n", (*p1)->grid_x, (*p1)->grid_y);
        free(tick);
        break;
    case START:
        fprintf(stderr, "[client] STARTING GAME !!!\n");
    }
    default:
        break;
    }
}

void client_loop(int server_fd, BoardContent *args_board)
{
    struct timespec ts;
    // 125ms
    ts.tv_sec = 0;
    ts.tv_nsec = 100000000;

    // create p1 and p2
    Player *p1 = NULL;
    Player *p2 = NULL;
    // draw players

    while (!MULTI_STOPPED)
    {
        // handle message
        //
        int size = 64;
        char *buffer = malloc(size * sizeof(char));
        // wait for server message
        int size_msg = receve_data(&buffer, server_fd, &size);
        fprintf(stderr, "message read \n");
        if (size_msg != -1)
            handle_message(buffer, size_msg, &p1, &p2, args_board);
        free(buffer);

        // connection lost
        if (size_msg == -1)
        {
            MULTI_STOPPED = 1;
        }
        nanosleep(&ts, NULL);
    }
    fprintf(stderr, "client EXITED : %d %d\n", MULTI_STOPPED, STOPPED);
    destroy_player(p1);
    destroy_player(p2);
}

void client_init(BoardContent *args_board)
{
    (void)args_board;
    struct timespec ts;
    // 125ms
    ts.tv_sec = 0;
    ts.tv_nsec = 125000000;

    // server connection
    fprintf(stderr, "[client] waiting for server to respond\n");
    int server_fd = prepare_socket_client(G_IP, G_PORT);
    int nb_try = 50;
    while (nb_try && server_fd == -1)
    {
        server_fd = prepare_socket_client(G_IP, G_PORT);
        fprintf(stderr, "[client] connection failed retrying\n");
        nb_try--;
        nanosleep(&ts, NULL);
    }
    if (nb_try == 0)
    {
        fprintf(stderr, "[client] error, can't connected to server");
        MULTI_STOPPED = 1;
    }

    G_SERVER_FD = server_fd;

    fprintf(stderr, "[client] connected with socket %d\n", server_fd);
    // main loop
    client_loop(server_fd, args_board);

    // close connection with server
    close(server_fd);
    // clear grid
    clear_grid(HEIGHT_ID_TO_DISPLAY_ID(G_GRID_HEIGHT));
    draw_borders(G_CANVAS_WIDTH, G_CANVAS_HEIGHT);
}
