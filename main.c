#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lib/gamemanager.h"

int main(int argc, char **argv) {
  GameManager *gm = new_gamemanager(10, 10);

  int c;
  while (1) {
    if (draw_map(gm) == -1) {
      fprintf(stderr, "draw_map failed\n");
      exit(1);
    }

    printf("\nEnter a direction:\n");
    scanf("%d", &c);

    switch (c) {
      case UP:
        move_player(gm, UP);
        break;

      case DOWN:
        move_player(gm, DOWN);
        break;

      case LEFT:
        move_player(gm, LEFT);
        break;

      case RIGHT:
        move_player(gm, RIGHT);
        break;

      default:
        printf("\ninvalid direction\n");
        break;
    }
  }

  if (free_gamemanager(gm) == -1) {
    fprintf(stderr, "free_gamemanager failed\n");
    exit(1);
  }

  return 0;
}
