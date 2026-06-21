#ifndef GAMELIB_H
#define GAMELIB_H

#include "engine.h"

typedef enum {
  WOOD,
  STONE
} ItemId;

typedef struct Item {
  ItemId type;
  void *data;
  char symbol;
  struct Item *next;
} Inventory;

typedef struct {
  unsigned int x, y;
  Inventory *inventory;
} Player;

typedef struct {
  size_t width, height;
  char *map;
} Map;

#define GAME_MANAGER_BASE {0, 0, 0, 0, {0, 0, NULL}, {0, 0, NULL}}

typedef struct {
  size_t win_width, win_height;
  unsigned int cursor_x, cursor_y;

  Player player;
  Map map;
} GameManager;

int init_game_manager(GameManager *gm, ObjectTracker **tracker);

int add_item_to_inventory(GameManager *gm, ItemId id, void *data, char symbol, ObjectTracker **tracker);
int inventory_to_str(GameManager gm, char *buf, size_t buf_size);

int draw_map(GameManager gm, unsigned int start_x, unsigned int start_y, FrameBuffer *frame_buffer);

typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT
} Direction;

int move_player(GameManager *gm, Direction direction);

#endif
