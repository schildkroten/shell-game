#ifndef GAMELIB_H
#define GAMELIB_H

#include "engine.h"

typedef struct {
  int x, y;
} Player;

typedef struct {
  size_t width, height;
  char *map;
} Map;

#define GAME_MANAGER_BASE {0, 0, {0, 0}, {0, 0, NULL}}

typedef struct {
  size_t win_width, win_height;

  Player player;
  Map map;
} GameManager;

int init_game_manager(GameManager *gm, size_t map_width, size_t map_height);

int draw_map(GameManager gm, FrameBuffer *frame_buffer, int start_x, int start_y);

typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT
} Direction;

int move_player(GameManager *gm, Direction direction);

#endif
