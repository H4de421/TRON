#pragma once

typedef enum Dir {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Dir;


struct Dir_links {
    Dir dir;
    char *string;
};
