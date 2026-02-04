#include "Utils/pretty_printer.h"

void print_init_struct(init_enum *init, FILE *fd)
{
    fprintf(fd,
            "\ninit_struct\n{\n\tplayer_id->%d\n\tmax_c->%d\n\tmax_l->%d\n\tp1_"
            "x->%d\n\tp1_y->%"
            "d\n\tp2_x->%d\n\tp2_y->%d\n}\n",
            init->player_id, init->max_c, init->max_l, init->p1_x, init->p1_y,
            init->p2_x, init->p2_y);
}
