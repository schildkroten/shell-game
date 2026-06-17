#include <string.h>
#include <errno.h>

#include "../lib/engine.h"

typedef struct {
  unsigned int x, y;
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

int init_game_manager(GameManager *gm) {
  if (gm == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (gm->map.map != NULL) {
    errno = EINVAL;
    return -1;
  }

  if (get_window_size(&gm->win_width, &gm->win_height) == -1) { return -1; }

  if (gm->win_width < 80 || gm->win_height < 24) {
    errno = ENOMSG;
    return -1;
  }

  gm->map.width = gm->win_width * 2/3;
  gm->map.height = gm->win_height * 2/3;
  gm->map.map = malloc(gm->map.width * gm->map.height * sizeof(char));

  if (gm->map.map == NULL) { return -1; }

  memset(gm->map.map, ',', gm->map.width * gm->map.height * sizeof(char));

  if (get_cursor_pos(&gm->cursor_x, &gm->cursor_y) == -1) { return -1; }

  return 0;
}

int draw_map(GameManager gm, FrameBuffer *frame_buffer) {
  if (frame_buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  for (size_t y = 0; y < gm.map.height; y++) {
    for (size_t x = 0; x < gm.map.width; x++) {
      if (x == gm.player.x && y == gm.player.y) {
        if (insert_into_frame(frame_buffer, '@', x, y) == -1) {
          return -1;
        }
      } else {
        if (insert_into_frame(frame_buffer, gm.map.map[y * gm.map.width + x], x, y) == -1) {
          return -1;
        }
      }
    }
  }

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
