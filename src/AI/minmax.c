#include "AI/minmax.h"

#include <stdlib.h>
#include <string.h>

#include "Display/Colors.h"
#include "Player.h"
#include "globals.h"
#include "limits.h"

static const int AI = 2;
static const int PLAYER = 1;

static int oposite(int dir1, int dir2)
{
    return dir1 == ((dir2 + 1) % 2) + 2 * (dir2 > 1 ? 1 : 0);
}

static int get_grid(int *grid, int x, int y)
{
    return grid[GRID_WIDTH * y + x];
}

static void set_grid(int *grid, int x, int y, int value)
{
    grid[GRID_WIDTH * y + x] = value;
}

static int *duplicate_grid(int *grid)
{
    int *res = malloc(GRID_WIDTH * GRID_HEIGHT);
    res = memcpy(res, grid, GRID_WIDTH * GRID_HEIGHT);
    return res;
}

// WARNING recursion
int compute_score(int *grid, Pair ai, Pair player, Pair pos)
{
    if (pos.first < 0 || pos.first > GRID_WIDTH || pos.second < 0
        || pos.second > GRID_HEIGHT)
        return 0;
    int controled = (get_grid(grid, pos.first, pos.second) == 0)
        && ABS(ai.first - pos.first) + ABS(ai.second - pos.second)
            < ABS(player.first - pos.first) + ABS(player.second - pos.second);
    Pair p1 = { .first = pos.first + 1, .second = pos.second };
    Pair p2 = { .first = pos.first - 1, .second = pos.second };
    Pair p3 = { .first = pos.first, .second = pos.second + 1 };
    Pair p4 = { .first = pos.first, .second = pos.second - 1 };
    return controled + compute_score(grid, ai, player, p1)
        + compute_score(grid, ai, player, p2)
        + compute_score(grid, ai, player, p3)
        + compute_score(grid, ai, player, p4);
}

/*int compute_score(int *grid, Pair ai, Pair player, Pair pos)
{
    return _compute_score(grid,GRID_WIDTH/2,GRID_HEIGHT/2,AI) -
_compute_score(grid, GRID_WIDTH/2, GRID_HEIGHT/2, PLAYER);
}*/

// return {choise, score}
Pair minmax_cpu(int *grid, Entity ai, Entity player, int deep)
{
    if (deep == 0)
    {
        Pair p_ai = { .first = ai.x, .second = ai.y };
        Pair p_player = { .first = player.x, .second = player.y };
        Pair res = { .first = -1,
                     .second = compute_score(grid, p_ai, p_ai, p_ai) };
        return res;
    }
    Pair best_score = { .first = -1, .second = INT_MIN };
    // compute all possible way score
    for (int ai_d = 0; ai_d < 4; ai_d++)
    {
        // avoid impossible moves
        if (oposite(ai_d, ai_d))
            continue;
        Pair curr_score = { .first = ai_d, .second = 0 };
        // compute new pos for AI
        Entity new_ai = {
            .x = ai.x + (ai_d == RIGHT ? 1 : (ai_d == LEFT ? -1 : 0)),
            .y = ai.y + (ai_d == DOWN ? 1 : (ai_d == UP ? -1 : 0)),
            .dir = ai_d,
        };
        // compute auu possible player moves
        for (int plyr_d = 0; plyr_d < 4; plyr_d++)
        {
            int *sub_grid = duplicate_grid(grid);

            Entity new_player = {
                .x = player.x + (plyr_d == RIGHT ? 1 : (ai_d == LEFT ? -1 : 0)),
                .y = player.y + (plyr_d == DOWN ? 1 : (plyr_d == UP ? -1 : 0)),
                .dir = plyr_d,
            };
            set_grid(sub_grid, new_ai.x, new_ai.y, AI);
            set_grid(sub_grid, new_player.x, new_player.y, PLAYER);

            // keeping the best score
            Pair score = minmax_cpu(grid, new_ai, new_player, deep - 1);
            curr_score.second += score.second;
            free(sub_grid);
        }
        if (best_score.second < curr_score.second)
        {
            best_score.first = curr_score.first;
            best_score.second = best_score.second;
        }
    }
    return best_score;
}
