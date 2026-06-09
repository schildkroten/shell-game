#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
  int x, y;
} Player;

Player *new_player();
int free_player(Player *player);

#endif
