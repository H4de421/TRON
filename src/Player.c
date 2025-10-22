#define _POSIX_C_SOURCE 200809L

#include "Player.h"

Player *create_player(int x, int y, char color[]) {
  Player *res = malloc(sizeof(Player));
  res->grid_x = x;
  res->grid_y = y;
  res->color = color;
  res->dir = RIGHT;
  res->old_dir = RIGHT;
  res->num = 1;
  return res;
}

void destroy_player(Player *player) {
  free(player);
}

int move_player(Player *player, String *buffer, char *grid) {
  Dir dir = player->dir;
  // compute new value
  int grid_x = player->grid_x + (dir == RIGHT ? 1 : (dir == LEFT ? -1 : 0));
  int grid_y = player->grid_y + (dir == DOWN ? 1 : (dir == UP ? -1 : 0));
  if (grid_x < 0 || grid_x >= GRID_WIDTH || grid_y < 0 || grid_y >= GRID_HEIGHT)
    return 1;
  // draw trail before all
  draw_trail(player, buffer);

  if (grid[GRID_COORDINATE(grid_x, grid_y)] != 0)
    return 1;
  // place new spot
  player->grid_x = grid_x;
  player->grid_y = grid_y;
  draw_player(player, buffer, grid);
  player->old_dir = player->dir;
  return 0;
}

static void get_trail_curve(Dir dir, Dir old_dir, char buffer[5]) {
  if (dir == old_dir) {
    strcpy(buffer, (dir > 1 ? "─" : "│"));
    return;
  }
  int score = 10 * dir + old_dir;
  switch (score) {
  case 2:
  case 31:
    strcpy(buffer, "└");
    break;
  case 3:
  case 21:
    strcpy(buffer, "┘");
    break;
  case 13:
  case 20:
    strcpy(buffer, "┐");
    break;
  case 12:
  case 30:
    strcpy(buffer, "┌");
    break;
  default:
    break;
  }
}

void draw_trail(Player *player, String *buffer) {
  int shift_w = SHIFT_WIDTH + WIDTH_ID_TO_DISPLAY_ID((player->grid_x + 1));
  int shift_h = SHIFT_HEIGHT + HEIGHT_ID_TO_DISPLAY_ID((player->grid_y + 1));
  int nb_draw = (player->dir > 1 ? 5 : 2);
  int start = 0;

  if (player->dir != player->old_dir) {
    char trail[5];
    get_trail_curve(player->dir, player->old_dir, trail);
    char temp[50];
    sprintf(temp, "\e[%d;%dH%s%s\e[0m", shift_h, shift_w, player->color, trail);
    addToString(buffer, temp, 50);
    start = 1;
    shift_h += (player->dir > 1 ? 0 : 1) * (player->dir % 2 ? 1 : -1);
    shift_w += (player->dir > 1 ? 1 : 0) * (player->dir % 2 ? 1 : -1);
  }

  for (int i = start; i < nb_draw; i++) {
    char *trail = (player->dir > 1 ? "─" : "│");
    char temp[50];
    sprintf(temp, "\e[%d;%dH%s%s\e[0m", shift_h, shift_w, player->color, trail);
    addToString(buffer, temp, 50);
    shift_h += (player->dir > 1 ? 0 : 1) * (player->dir % 2 ? 1 : -1);
    shift_w += (player->dir > 1 ? 1 : 0) * (player->dir % 2 ? 1 : -1);
  }
}

void draw_player(Player *player, String *buffer, char *grid) {
  int shift_w = SHIFT_WIDTH + WIDTH_ID_TO_DISPLAY_ID((player->grid_x + 1));
  int shift_h = SHIFT_HEIGHT + HEIGHT_ID_TO_DISPLAY_ID((player->grid_y + 1));
  char temp[50];
  sprintf(temp, "\e[%d;%dH%s■\e[0m", shift_h, shift_w, player->color);
  addToString(buffer, temp, 50);
  grid[GRID_COORDINATE(player->grid_x, player->grid_y)] = player->num;
}
