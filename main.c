#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lib/gamemanager.h"
#include "lib/io.h"

#define MAP_WIDTH 20
#define MAP_HEIGHT 10

void reset_screen() {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
}

void die(const char *s) {
  reset_screen();
  perror(s);
  exit(1);
}

int main(int argc, char **argv) {
  enable_raw_mode();

  GameManager *gm;
  if ((gm = new_game_manager(MAP_WIDTH, MAP_HEIGHT)) == NULL) {
    die("new_game_manager");
  }

  char *content = "Inventory\nstone: 10\nwood: 15\nmeat: 3";
  Menu *menu;
  if ((menu = new_menu(MAP_WIDTH, MAP_HEIGHT, content, strlen(content))) == NULL) {
    free_game_manager(gm);
    die("new_menu");
  }

  reset_screen();

  char s[32];
  int slen;
  while (1) {
    ScreenBuffer *screen_buffer;
    if ((screen_buffer = new_screen_buffer(85, 21)) == NULL) {
      free_game_manager(gm);
      die("new_screen_buffer");
    }

    write(STDOUT_FILENO, "\x1b[?25l", 6);

    reset_screen();

    if (draw_map(gm, screen_buffer, 0, 0) == -1) {
      free_game_manager(gm);
      free_screen_buffer(screen_buffer);
      die("draw_map");
    }

    if (draw_menu(menu, screen_buffer, MAP_WIDTH, 0) == -1) {
      free_game_manager(gm);
      free_screen_buffer(screen_buffer);
      die("draw_menu");
    }

    if (write_screen_buffer(screen_buffer) == -1) {
      free_game_manager(gm);
      free_screen_buffer(screen_buffer);
      die("write_screen_buffer"); 
    }

    slen = snprintf(s, 32, "\x1b[%d;%dH", gm->player->y + 1, gm->player->x + 1);
    write(STDOUT_FILENO, s, slen);

    write(STDOUT_FILENO, "\x1b[?25h", 6);

    if (free_screen_buffer(screen_buffer) == -1) {
      free_game_manager(gm);
      die("free_screen_buffer");
    }

    switch (get_keypress()) {
      case ARROW_LEFT:
      case 'h':
        move_player(gm, LEFT);
        break;

      case ARROW_DOWN:
      case 'j':
        move_player(gm, DOWN);
        break;

      case ARROW_UP:
      case 'k':
        move_player(gm, UP);
        break;

      case ARROW_RIGHT:
      case 'l':
        move_player(gm, RIGHT);
        break;

      case 'e':
        if (menu->open == 0) {
          menu->open = 1;
        } else {
          menu->open = 0;
        }

        break;

      case CTRL_KEY('q'):
        reset_screen();
        free_game_manager(gm);
        exit(0);

      default:
        break;
    }
  }

  return 0;
}
