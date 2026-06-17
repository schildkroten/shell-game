#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lib/engine.h"
#include "lib/gamelib.h"

ObjectTracker *tracker = NULL;

void cleanup() {
  disable_raw_mode();
  reset_screen();
  free_tracked_objects(&tracker);
}

void die(const char *s) {
  reset_screen();
  free_tracked_objects(&tracker);
  perror(s);
  exit(1);
}

int main() {
  enable_raw_mode();
  reset_screen();

  atexit(cleanup);

  GameManager gm = GAME_MANAGER_BASE;
  if (init_game_manager(&gm) == -1) {
    die("init_game_manager");
  }

  if (track_object(&tracker, gm.map.map) == -1) { die("track_object"); }

  while (1) {
    reset_screen();

    write(STDOUT_FILENO, "\x1b[?25l", 6);

    FrameBuffer frame_buffer = FRAME_BUFFER_BASE;
    if (init_frame_buffer(&frame_buffer, gm.win_width, gm.win_height) == -1) {
      die("init_frame_buffer");
    }

    if (draw_map(gm, &frame_buffer, 0, 0) == -1) { die("draw_map"); }

    if (write_frame_buffer(frame_buffer) == -1) { die("write_frame_buffer"); }

    move_cursor_to(gm.player.x, gm.player.y);
    gm.cursor_x = gm.player.x;
    gm.cursor_y = gm.player.y;

    write(STDOUT_FILENO, "\x1b[?25h", 6);

    int key = get_keypress();

    if (key == -1) {
      die("get_keypress");
    }

    switch (key) {
      case ARROW_UP:
        if (move_player(&gm, UP) == -1) { die("move_player"); }
        break;

      case ARROW_DOWN:
        if (move_player(&gm, DOWN) == -1) { die("move_player"); }
        break;

      case ARROW_LEFT:
        if (move_player(&gm, LEFT) == -1) { die("move_player"); }
        break;

      case ARROW_RIGHT:
        if (move_player(&gm, RIGHT) == -1) { die("move_player"); }
        break;

      case CTRL_KEY('q'):
        free(frame_buffer.buffer);
        exit(0);

      default:
        break;
    }

    free(frame_buffer.buffer);
  }

  return 0;
}
