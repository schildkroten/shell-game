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
  perror(s);
  exit(1);
}

int main() {
  enable_raw_mode();
  reset_screen();

  atexit(cleanup);

  GameManager gm = GAME_MANAGER_BASE;
  if (init_game_manager(&gm, &tracker) == -1) { die("init_game_manager"); }

  Menu inventory = MENU_BASE;
  if (init_menu(&inventory, gm.win_width - gm.map.width - 1, gm.win_height, "", 1, &tracker) == -1) {
    die("init_menu");
  }

  char buf[INVENTORY_SIZE];

  while (1) {
    FrameBuffer frame_buffer = FRAME_BUFFER_BASE;
    if (init_frame_buffer(&frame_buffer, gm.win_width, gm.win_height) == -1) {
      die("init_frame_buffer");
    }

    if (inventory_to_str(gm, buf, sizeof(buf)) == -1) { die("inventory_to_str"); }

    if (update_menu_content(&inventory, buf, strlen(buf), &tracker) == -1) { die("update_menu_content"); }
    
    write(STDOUT_FILENO, "\x1b[?25l", 6);
    write(STDOUT_FILENO, "\x1b[H", 3);

    if (draw_map(gm, 0, 0, &frame_buffer) == -1) { die("draw_map"); }
    if (draw_menu(inventory, gm.map.width, 0, &frame_buffer) == -1) { die("draw_menu"); }

    if (write_frame_buffer(frame_buffer) == -1) { die("write_frame_buffer"); }

    free(frame_buffer.buffer);

    move_cursor(&gm, gm.player.x, gm.player.y);

    write(STDOUT_FILENO, "\x1b[?25h", 6);

    int key = get_keypress();
    if (key == -1) { die("get_keypress"); }

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
        exit(0);
        break;

      default:
        break;
    }
  }

  return 0;
}
