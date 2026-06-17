#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lib/engine.h"
#include "lib/gamelib.h"

#define MAP_WIDTH 20
#define MAP_HEIGHT 10

ObjectTracker *tracker = NULL;

void cleanup() {
  disable_raw_mode();
  reset_screen();
  free_tracked_objects(&tracker);
}

void die(const char *s) {
  reset_screen();
  perror(s);
  exit(1);
}

int main() {
  enable_raw_mode();
  reset_screen();

  atexit(cleanup);

  GameManager gm = GAME_MANAGER_BASE;
  if (init_game_manager(&gm, MAP_WIDTH, MAP_HEIGHT) == -1) {
    die("init_game_manager");
  }

  if (track_object(&tracker, gm.map.map) == -1) {
    die("track_object");
  }

  while (1) {
    reset_screen();

    FrameBuffer frame_buffer = FRAME_BUFFER_BASE;
    if (init_frame_buffer(&frame_buffer, 80, 40) == -1) {
      die("init_frame_buffer");
    }

    if (draw_map(gm, &frame_buffer, 0, 0) == -1) {
      die("draw_map");
    }

    write_frame_buffer(frame_buffer);
    write(STDOUT_FILENO, "\x1b[H", 3);

    int key = get_keypress();

    if (key == -1) {
      die("get_keypress");
    }

    switch (key) {
      case ARROW_UP:
        move_player(&gm, UP);
        break;

      case ARROW_DOWN:
        move_player(&gm, DOWN);
        break;

      case ARROW_LEFT:
        move_player(&gm, LEFT);
        break;

      case ARROW_RIGHT:
        move_player(&gm, RIGHT);
        break;

      case CTRL_KEY('q'):
        exit(0);

      default:
        break;
    }

    free(frame_buffer.buffer);
  }

  return 0;
}
