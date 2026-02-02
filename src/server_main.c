
#include <stdio.h>
#include <stdlib.h>

#include "Multiplayer/server.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "[server] wrong arguments number %d\n", argc);
        exit(42);
    }

    server_init(argv[1], argv[2]);
}
