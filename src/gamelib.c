#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "../lib/engine.h"

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

Item *create_item(ItemType type, char symbol, void *data) {
  Item *new_item = malloc(sizeof(Item));
  if (new_item == NULL) { return NULL; }

  new_item->type = type;
  new_item->symbol = symbol;
  new_item->data = data;

  return new_item;
}

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

typedef struct {
  size_t win_width, win_height;
  unsigned int cursor_x, cursor_y;

  Player player;
  Map map;
} GameManager;

int init_game_manager(GameManager *gm, ObjectTracker **tracker) {
  if (gm == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (gm->map.map != NULL) {
    errno = EINVAL;
    return -1;
  }

  if (tracker == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (get_window_size(&gm->win_width, &gm->win_height) == -1) { return -1; }
  if (get_cursor_pos(&gm->cursor_x, &gm->cursor_y) == -1) { return -1; }

  if (gm->win_width < 80 || gm->win_height < 24) {
    errno = ENOMSG;
    return -1;
  }

  gm->win_width = 80;
  gm->win_height = 24;

  gm->map.width = gm->win_width * 2/3;
  gm->map.height = gm->win_height;
  gm->map.map = malloc(gm->map.width * gm->map.height * sizeof(char));

  if (gm->map.map == NULL) { return -1; }

  memset(gm->map.map, ',', gm->map.width * gm->map.height * sizeof(char));

  if (track_object(tracker, gm->map.map) == -1) { return -1; } 

  return 0;
}

int add_to_inventory(GameManager *gm, Item *item) {
  if (gm == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (item == NULL) {
    errno = EINVAL;
    return -1;
  }

  int i;
  for (i = 0; i < INVENTORY_SIZE - 1; i++) {
    if (gm->player.inventory[i].item == item) {
      errno = EINVAL;
      return -1;
    }

    if (gm->player.inventory[i].item == NULL) {
      gm->player.inventory[i].item = item;
      return 0;
    }
  }

  if (i == INVENTORY_SIZE - 1) {
    return 1;
  }

  return 0;
}

int remove_from_inventory(GameManager *gm, Item *item) {
  if (gm == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (item == NULL) {
    errno = EINVAL;
    return -1;
  }

  int i;
  for (i = 0; i < INVENTORY_SIZE - 1; i++) {
    if (gm->player.inventory[i].item == item) {
      gm->player.inventory[i].item = NULL;
      return 0;
    }
  }

  if (i == INVENTORY_SIZE - 1) {
    return 1;
  }

  return 0;
}

int inventory_to_str(GameManager gm, char *buf, size_t buf_size) {
  if (buf == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (buf_size < 1) {
    errno = EINVAL;
    return -1;
  }

  unsigned int i;
  for (i = 0; i < INVENTORY_SIZE - 1 && i < buf_size - 1; i++) {
    if (gm.player.inventory[i].item == NULL) {
      buf[i] = ' ';
      continue;
    }

    buf[i] = gm.player.inventory[i].item->symbol;
  }

  buf[i] = '\0';

  return 0;
}

int draw_map(GameManager gm, unsigned int start_x, unsigned int start_y, FrameBuffer *frame_buffer) {
  if (frame_buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  for (size_t y = start_y; y < gm.map.height + start_y; y++) {
    memcpy(&frame_buffer->buffer[y * frame_buffer->width + start_x], &gm.map.map[y * gm.map.width], gm.map.width);
  }

  frame_buffer->buffer[gm.player.y * frame_buffer->width + gm.player.x] = '@';

  return 0;
}

typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT
} Direction;

int move_player(GameManager *gm, Direction direction) {
  if (gm == NULL) {
    errno = EINVAL;
    return -1;
  }

  switch (direction) {
    case UP:
      if (gm->player.y > 0) { gm->player.y--; }
      break;

    case DOWN:
      if (gm->player.y < gm->map.height - 1) { gm->player.y++; }
      break;

    case LEFT:
      if (gm->player.x > 0) { gm->player.x--; }
      break;

    case RIGHT:
      if (gm->player.x < gm->map.width - 1) { gm->player.x++; }
      break;

    default:
      break;
  }

  return 0;
}

int move_cursor(GameManager *gm, unsigned int x, unsigned int y) {
  if (gm == NULL) {
    errno = EINVAL;
    return -1;
  }

  char buffer[32];
  snprintf(buffer, sizeof(buffer), "\x1b[%d;%dH", y + 1, x + 1);
  write(STDOUT_FILENO, buffer, strlen(buffer));

  gm->cursor_x = x;
  gm->cursor_y = y;

  return 0;
}
