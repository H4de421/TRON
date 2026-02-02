#include <time.h>
#define _GNU_SOURCE
#include <err.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "Multiplayer/network.h"
#include "globals.h"

#define NB_METHOD 6
#define MESSAGE_MAX_SIZE 4096

struct method_e_str
{
    char *str;
    enum message_type type;
};

struct method_e_str method_trad[NB_METHOD] = {
    { "INIT", INIT },   { "SIZE", SIZE }, { "TICK", TICK },
    { "START", START }, { "IN", IN },     { "ERROR", ERROR }
};
enum message_type parse_method(char *method)
{
    for (int i = 0; i < NB_METHOD; i++)
    {
        if (strcmp(method, method_trad[i].str) == 0)
            return method_trad[i].type;
    }
    return ERROR;
}

void send_message(enum message_type method, int fd, char *format, ...)
{
    char final_message[MESSAGE_MAX_SIZE] = { 0 };
    sprintf(final_message, "%s;", method_trad[method].str);
    memcpy(final_message, method_trad[method].str,
           strlen(method_trad[method].str));
    va_list args;
    size_t size = strlen(method_trad[method].str);
    final_message[size++] = ';';
    for (va_start(args, format); *format != '\0'; ++format)
    {
        if (*format == '%')
        {
            ++format;
            switch (*format)
            {
            case 's': {
                char *content = va_arg(args, char *);
                memcpy(final_message + (size++), content, (int)sizeof(content));
                size += strlen(content);
                break;
            }
            case 'd': {
                int i = va_arg(args, int);
                char content[1028];
                sprintf(content, "%d", i);
                sprintf(final_message + size, "%d;", i);
                size += strlen(content);
                break;
            }
            default:
                final_message[size++] = '%';
                final_message[size++] = *format;
                break;
            }
        }
        else
        {
            final_message[size++] = *format;
        }
    }
    final_message[size++] = '\n';
    fflush(stdout);
    printf("sending to %u : [%s]", fd, final_message);
    int res = send_data(final_message, fd, size);
    printf("send returned %d\n", res);
}

init_enum *parse_INIT(char *content)
{
    init_enum *res = calloc(1, sizeof(init_enum));
    if (content == NULL)
        return res;
    char *savestr = NULL;
    char *p_id = strtok_r(content, ";", &savestr);
    char *max_c = strtok_r(NULL, ";", &savestr);
    char *max_l = strtok_r(NULL, ";", &savestr);
    char *p1_x = strtok_r(NULL, ";", &savestr);
    char *p1_y = strtok_r(NULL, ";", &savestr);
    char *p2_x = strtok_r(NULL, ";", &savestr);
    char *p2_y = strtok_r(NULL, ";", &savestr);
    /*printf("\n%s | %s | %s | %s | %s | %s | %s\n", p_id, max_c, max_l, p1_x,
           p1_y, p2_x, p2_y);*/
    res->player_id = atoi(p_id);
    res->max_c = atoi(max_c);
    res->max_l = atoi(max_l);
    res->p1_x = atoi(p1_x);
    res->p1_y = atoi(p1_y);
    res->p2_x = atoi(p2_x);
    res->p2_y = atoi(p2_y);
    return res;
}

in_enum *parse_IN(char *content)
{
    in_enum *res = calloc(1, sizeof(in_enum));
    if (content == NULL)
        return res;
    char *savestr = NULL;
    char *dir = strtok_r(content, ";", &savestr);
    res->dir = atoi(dir);
    return res;
}

size_enum *parse_server_SIZE(char *content)
{
    size_enum *res = calloc(1, sizeof(size_enum));
    if (content == NULL)
        return res;
    char *savestr = NULL;
    char *max_c = strtok_r(content, ";", &savestr);
    char *max_l = strtok_r(NULL, ";", &savestr);
    res->nb_col = atoi(max_c);
    res->nb_lin = atoi(max_l);
    return res;
}

tick_enum *parse_TICK(char *content)
{
    tick_enum *res = calloc(1, sizeof(tick_enum));
    char *savestr;
    char *p1_d = strtok_r(content, ";", &savestr);
    char *p2_d = strtok_r(NULL, ";", &savestr);
    res->p1_d = atoi(p1_d);
    res->p2_d = atoi(p2_d);
    return res;
}

int is_alive(int sock)
{
    const char *ping = "PING;\n";
    int ret = send(sock, ping, strlen(ping), MSG_NOSIGNAL | MSG_DONTWAIT);
    if (ret == -1)
    {
        return 0; // socket fermée ou erreur
    }
    return 1; // socket semble ok
}

int create_and_bind(struct addrinfo *addrinfos)
{
    while (addrinfos != NULL)
    {
        int s = socket(addrinfos->ai_family, addrinfos->ai_socktype,
                       addrinfos->ai_protocol);
        if (s != -1)
        {
            int optval = 1;
            setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

            int error = bind(s, addrinfos->ai_addr, addrinfos->ai_addrlen);
            if (error == -1)
            {
                close(s);
                addrinfos = addrinfos->ai_next;
                continue;
            }
            return s;
        }
        addrinfos = addrinfos->ai_next;
    }
    errx(1, "Can't bind\n");
}

int prepare_socket(const char *ip, const char *port)
{
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
        errx(1, "GAI error");
    }

    int socket_id = create_and_bind(servinfo);

    freeaddrinfo(servinfo);

    int flags = fcntl(socket_id, F_GETFL);
    fcntl(socket_id, F_SETFL, flags | O_NONBLOCK);

    listen(socket_id, SOMAXCONN);
    return socket_id;
}

int receve_data(char **buffer, int socket, int *buffer_size)
{
    *buffer_size = 64;
    *buffer = realloc(*buffer, *buffer_size * sizeof(char));

    int last_read = recv(socket, *buffer, *buffer_size, MSG_NOSIGNAL);
    int read = last_read;
    while (last_read > 0 && (*buffer)[read - 1] != '\n')
    {
        // fprintf(stderr,
        //         "[server read] last read %d / read %d / size %d / current
        //         last " "char %c\n", last_read, read, *buffer_size,
        //         (*buffer)[read - 1]);
        *buffer_size += last_read;
        *buffer = realloc(*buffer, *buffer_size);
        last_read = recv(socket, (*buffer) + read, 64, MSG_NOSIGNAL);
        if (last_read == -1)
        {
            return -1;
        }
        read += last_read;
    }
    if (read <= 0)
    {
        return -1;
    }
    //    fprintf(stderr,
    //            "[server read] EN",
    //            *buffer_size + 1, read);

    *buffer = realloc(*buffer, *buffer_size + 1);
    (*buffer)[read] = '\0';

    // fprintf(stderr, "read : %s\n", *buffer);

    return read;
}

int send_data(char *buffer, int socket, int size)
{
    int last_send = send(socket, buffer, size, MSG_NOSIGNAL);
    int sended = last_send;
    while (sended < size)
    {
        last_send = send(socket, buffer + sended, size - sended, MSG_NOSIGNAL);
        if (sended == -1)
            return -1;
        sended += last_send;
    }
    /*if (last_send == -1)
    {
        return -1;
    }*/

    return sended;
}

void prepare_logging(char *name, int fd, char *str_PID)
{
    char filename[2048];
    strcat(filename, name);
    strcat(filename, str_PID);
    int file = open(filename, O_CREAT | O_WRONLY,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (!file)
    {
        fprintf(stderr, "can't open file");
        return;
    }
    int res = dup2(file, fd);
    if (res == -1)
    {
        fprintf(stderr, "can't dup2");
        return;
    }
}
