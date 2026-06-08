#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lib/gamemanager.h"
#include "lib/player.h"
#include "lib/map.h"

int main(int argc, char **argv) {
  Map *map = new_map(10, 10);

  if (draw_map(map) == -1) {
    fprintf(stderr, "draw_map failed");
    exit(1);
  }

  if (free_map(map) == -1) {
    fprintf(stderr, "free_map failed");
    exit(1);
  }

  return 0;
}
