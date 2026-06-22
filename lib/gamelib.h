#ifndef GAMELIB_H
#define GAMELIB_H

#include "engine.h"

typedef enum {
  TOOL,
  RESOURCE
} ItemType;

typedef enum {
  PICKAXE,
  AXE
} ToolId;

typedef struct {
  ToolId id;
  unsigned int damage;
  unsigned int durability;
  unsigned int breaking_power;
} ToolData;

typedef enum {
  WOOD,
  STONE
} ResourceId;

typedef struct {
  ResourceId id;
} ResourceData;

typedef struct {
  ItemType type;
  char symbol;
  void *data;
} Item;

Item *create_item(ItemType type, char symbol, void *data);

typedef struct {
  Item *item;
} Inventory;

#define INVENTORY_SIZE 50

typedef struct {
  unsigned int x, y;
  Inventory inventory[INVENTORY_SIZE];
} Player;

typedef struct {
  size_t width, height;
  char *map;
} Map;

#define GAME_MANAGER_BASE {0, 0, 0, 0, {0, 0, {[0 ... INVENTORY_SIZE - 1] = {NULL}}}, {0, 0, NULL}}

typedef struct {
  size_t win_width, win_height;
  unsigned int cursor_x, cursor_y;

  Player player;
  Map map;
} GameManager;

int init_game_manager(GameManager *gm, ObjectTracker **tracker);

int add_to_inventory(GameManager *gm, Item *item);
int remove_from_inventory(GameManager *gm, Item *item);
int inventory_to_str(GameManager gm, char *buf, size_t buf_size);

int draw_map(GameManager gm, unsigned int start_x, unsigned int start_y, FrameBuffer *frame_buffer);

typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT
} Direction;

int move_player(GameManager *gm, Direction direction);

int move_cursor(GameManager *gm, unsigned int x, unsigned int y);

#endif
