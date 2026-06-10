#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

typedef struct {
  int x, y;
} Player;

Player *new_player() {
  Player *new_player;
  if ((new_player = malloc(sizeof(Player))) == NULL) {
    return NULL;
  }

  new_player->x = 0;
  new_player->x = 0;

  return new_player;
}

int free_player(Player *player) {
  if (player == NULL) {
    errno = EINVAL;
    return -1;
  }

  player->x = 0;
  player->y = 0;

  free(player);

  return 0;
}
