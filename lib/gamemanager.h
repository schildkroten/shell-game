#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "player.h"
#include "map.h"

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

GameManager *new_gamemanager(size_t rows, size_t cols);
int free_gamemanager(GameManager *gm);
int draw_map(GameManager *gm);
int move_player(GameManager *gm, Direction direction);

#endif
