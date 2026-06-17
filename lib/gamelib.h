#ifndef GAMELIB_H
#define GAMELIB_H

#include "engine.h"

typedef struct {
  int x, y;
} Player;

typedef struct {
  int width, height;
  char *map;
} Map;

#define GAME_MANAGER_BASE {{0, 0}, {0, 0, NULL}}

typedef struct {
  Player player;
  Map map;
} GameManager;

int init_game_manager(GameManager *gm, int map_width, int map_height);

int draw_map(GameManager gm, FrameBuffer *frame_buffer, int start_x, int start_y);

typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT
} Direction;

int move_player(GameManager *gm, Direction direction);

#endif
