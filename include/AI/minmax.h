#pragma once

#define MIN_X(v1,v2,v3) ((v1) < (v2) ? ((v1) < (v3) ? (v1) : (v3)) : ((v2) < (v3) ? (v2) : (v3)))
#define POSSIBILITY 9
#define ABS(x) ((x) > 0 ? (x) : (x)*-1)

typedef struct pair{
    int first;
    int second;
} Pair;

typedef struct entity{
    int x;
    int y;
    int dir;
} Entity;

Pair minmax_cpu(int *grid, Entity AI, Entity player, int deep);

int compute_score(int *grid, Pair ai, Pair player, Pair pos);
