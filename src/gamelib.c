#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "../lib/engine.h"

typedef enum {
  WOOD,
  STONE
} ItemId;

typedef struct Item {
  ItemId id;
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

  gm->map.width = gm->win_width * 2/3;
  gm->map.height = gm->win_height;
  gm->map.map = malloc(gm->map.width * gm->map.height * sizeof(char));

  if (gm->map.map == NULL) { return -1; }

  memset(gm->map.map, ',', gm->map.width * gm->map.height * sizeof(char));

  if (track_object(tracker, gm->map.map) == -1) { return -1; } 

  return 0;
}

int add_item_to_inventory(GameManager *gm, ItemId id, void *data, char symbol, ObjectTracker **tracker) {
  if (gm == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (tracker == NULL) {
    errno = EINVAL;
    return -1;
  }

  struct Item *new_item = (struct Item *)malloc(sizeof(struct Item));

  if (new_item == NULL) { return -1; }

  if (track_object(tracker, new_item) == -1) { return -1; }

  new_item->id = id;
  new_item->data = data;
  new_item->symbol = symbol;
  new_item->next = NULL;

  if (gm->player.inventory == NULL) {
    gm->player.inventory = new_item;
    return 0;
  }

  struct Item *p = gm->player.inventory;
  while (p->next != NULL) { p = p->next; }

  p->next = new_item;

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

  struct Item *p = gm.player.inventory;
  unsigned int i = 0;
  while (p != NULL && i < buf_size - 1) {
    buf[i] = p->symbol;
    p = p->next;
    i++;
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
