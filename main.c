/* Shell survival game by Angus Bonney 2026 */

/* This is the main file it is where all the actual game
 * logic is run everything else is just librarys. */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "lib/engine.h"
#include "lib/gamelib.h"

/* Create global variables. */
ObjectTracker *tracker = NULL;

unsigned int num_enemys = 0;
unsigned int enemys_move = 0;

GameManager gm = GAME_MANAGER_BASE;

Menu inventory_menu = MENU_BASE;
Menu stats_menu = MENU_BASE;

/* The cleanup function gets called on exit
 * to disable raw mode and free the objects. */
void cleanup() {
  disable_raw_mode();
  reset_screen();
  free_tracked_objects(&tracker);
}

/* The die function just exits and prints a
 * error message. */
void die(const char *s) {
  perror(s);
  exit(1);
}

/* The setup function gets run once right as the
 * program starts. */
void setup() {
  srand(time(NULL));

  enable_raw_mode();
  reset_screen();

  atexit(cleanup);

  /* Initialize the global objects. */
  if (init_game_manager(&gm, &tracker) == -1) { die("init_game_manager"); }

  char inventory_buff[INVENTORY_SIZE];
  char stats_buff[256];

  if (inventory_to_str(&gm, inventory_buff, INVENTORY_SIZE) == -1) { die("inventory_to_str"); }
  if (stats_to_str(&gm, stats_buff, 256) == -1) { die("stats_to_str"); }

  if (init_menu(&inventory_menu, 30, 12, inventory_buff, INVENTORY_SIZE, &tracker) == -1) { die("init_menu"); }
  if (init_menu(&stats_menu, 30, 12, stats_buff, strlen(stats_buff), &tracker) == -1) { die("init_menu"); }
}

/* Update gets run every frame. */
void update() {
  /* Handle keypresses. */
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

  /* If there are no enemys spawn some. */
  if (num_enemys == 0) {
    num_enemys = rand() % 4 + 1;
    for (unsigned int i = 0; i < num_enemys; i++) {
      Enemy *new_enemy = create_enemy(rand() % gm.map.width , rand() % gm.map.height, 10, 0, NULL);

      if (new_enemy == NULL) { die("create_enemy"); }

      if (track_object(&tracker, new_enemy) == -1) { die("track_object"); }

      gm.enemys[i] = new_enemy;
    }
  } else {
    /* Else handle the movement of the enemys. */
    if (enemys_move) {
      for (unsigned int i = 0; i < MAX_ENEMYS - 1; i++) {
        if (gm.enemys[i] == NULL) { continue; }

        int8_t x_diff = gm.player.x - gm.enemys[i]->x;
        int8_t y_diff = gm.player.y - gm.enemys[i]->y;

        if ((x_diff & 0x7F) >= 1.5 * (y_diff & 0x7F)) {
          if (x_diff >= 0) {
            if (gm.enemys[i]->x + 1 == gm.player.x) {
              gm.player.health--;
            } else {
              gm.enemys[i]->x++;
            }
          } else {
            if (gm.enemys[i]->x - 1 == gm.player.x) {
              gm.player.health--;
            } else {
              gm.enemys[i]->x--;
            }
          }
        } else {
          if (y_diff >= 0) {
            if (gm.enemys[i]->y + 1 == gm.player.y) {
              gm.player.health--;
            } else {
              gm.enemys[i]->y++;
            }
          } else {
            if (gm.enemys[i]->y - 1 == gm.player.y) {
              gm.player.health--;
            } else {
              gm.enemys[i]->y--;
            }
          }
        }
      }
      enemys_move = 0;
    } else {
      enemys_move = 1;
    }
  }

  /* Update menus content */
  char inventory_buff[INVENTORY_SIZE];
  char stats_buff[256];

  if (inventory_to_str(&gm, inventory_buff, INVENTORY_SIZE) == -1) { die("inventory_to_str"); }
  if (update_menu_content(&inventory_menu, inventory_buff, INVENTORY_SIZE, &tracker) == -1) { die("update_menu_content_1"); }

  if (stats_to_str(&gm, stats_buff, 256) == -1) { die("stats_to_str"); }
  if (update_menu_content(&stats_menu, stats_buff, strlen(stats_buff), &tracker) == -1) { die("update_menu_content_2"); }
}

/* The draw function gets run every frame after the update
 * function. */
void draw() {
  FrameBuffer frame_buffer = FRAME_BUFFER_BASE;
  if (init_frame_buffer(&frame_buffer, gm.win_width, gm.win_height) == -1) {
    die("init_frame_buffer");
  }

  if (draw_map(&gm, 0, 0, &frame_buffer) == -1) { die("draw_map"); }
  if (draw_menu(&stats_menu, 60, 0, &frame_buffer) == -1) { die("draw_menu"); }
  if (draw_menu(&inventory_menu, 60, 12, &frame_buffer) == -1) { die("draw_menu"); }

  if (write_frame(frame_buffer) == -1) { die("write_frame_buffer"); }

  free(frame_buffer.buffer);

  move_cursor(&gm, gm.player.x, gm.player.y);
}

int main() {
  setup();
  draw();

  while (1) {
    update();
    draw();

    /* Exit if the players dead. */
    if (gm.player.health <= 0) {
      exit(0);
    }
  }

  return 0;
}
