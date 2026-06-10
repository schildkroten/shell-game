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

int main(int argc, char **argv) {
  enable_raw_mode();

  GameManager *gm = new_game_manager(MAP_WIDTH, MAP_HEIGHT);

  char *content = "Inventory\nstone: 10\nwood: 15\nmeat: 3";
  Menu *menu = new_menu((MAP_WIDTH * 2) - 1, MAP_HEIGHT, content, strlen(content));

  reset_screen();

  char s[32];
  int slen;
  while (1) {
    ScreenBuffer *screen_buffer = new_screen_buffer(85, 21);

    write(STDOUT_FILENO, "\x1b[?25l", 6);

    reset_screen();

    draw_map(gm, screen_buffer, 0, 0);
    draw_menu(menu, screen_buffer, MAP_WIDTH, 0);

    write_screen_buffer(screen_buffer);

    slen = snprintf(s, 32, "\x1b[%d;%dH", gm->player->y + 1, gm->player->x + 1);
    write(STDOUT_FILENO, s, slen);

    write(STDOUT_FILENO, "\x1b[?25h", 6);

    free_screen_buffer(screen_buffer);

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
