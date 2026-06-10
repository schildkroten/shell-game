#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "../lib/player.h"
#include "../lib/map.h"
#include "../lib/io.h"

typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT
} Direction;

typedef struct {
  Player *player;
  Map *map;
} GameManager;

GameManager *new_game_manager(int map_cols, int map_rows) {
  GameManager *new_gm;
  if ((new_gm = malloc(sizeof(GameManager))) == NULL) {
    return NULL;
  }
  
  if ((new_gm->player = new_player()) == NULL) {
    return NULL;
  }

  if ((new_gm->map = new_map(map_cols, map_rows)) == NULL) {
    return NULL;
  }

  return new_gm;
}

int free_game_manager(GameManager *gm) {
  if (gm == NULL) {
    errno = EINVAL;
    return -1;
  }

  free_player(gm->player);
  free_map(gm->map);
  free(gm);

  return 0;
}

int draw_map(GameManager *gm, ScreenBuffer *screen_buffer, int start_x, int start_y) {
  if (gm == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (screen_buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  for (int y = 0; y < gm->map->rows; y++) {
    for (int x = 0; x < gm->map->cols; x++) {
      if (x == gm->player->x && y == gm->player->y) {
        screen_buffer->buffer[y + start_y][x + start_x] = '@';
      } else {
        screen_buffer->buffer[y + start_y][x + start_x] = gm->map->map[y][x];
      }
    }
  }

  return 0;
}

int move_player(GameManager *gm, Direction direction) {
  if (gm == NULL) {
    errno = EINVAL;
    return -1;
  }

  switch (direction) {
    case UP:
      if (gm->player->y <= 0) break;
      gm->player->y--;
      break;

    case DOWN:
      if (gm->player->y >= gm->map->rows - 1) break;
      gm->player->y++;
      break;

    case LEFT:
      if (gm->player->x <= 0) break;
      gm->player->x--;
      break;

    case RIGHT:
      if (gm->player->x >= gm->map->cols - 1) break;
      gm->player->x++;
      break;

    default:
      break;
  }

  return 0;
}
