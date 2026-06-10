#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "player.h"
#include "map.h"
#include "io.h"

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

GameManager *new_game_manager(int rows, int cols);
int free_game_manager(GameManager *gm);

int draw_map(GameManager *gm, ScreenBuffer *screen_buffer, int start_x, int start_y);
int move_player(GameManager *gm, Direction direction);

#endif
