#define _GNU_SOURCE
#include "Multiplayer/network.h"

#include <err.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int is_alive(int sock)
{
    const char *ping = "PING\n";
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
        *buffer_size += last_read;
        *buffer = realloc(*buffer, *buffer_size);
        last_read = recv(socket, (*buffer) + read, 64, MSG_NOSIGNAL);
        if (last_read == -1)
        {
            printf("can't read\n");
            return -1;
        }
        read += last_read;
    }
    if (!last_read)
    {
        return -1;
    }

    *buffer = realloc(*buffer, *buffer_size + 1);
    (*buffer)[read] = '\0';

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
