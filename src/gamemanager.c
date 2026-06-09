#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../lib/player.h"
#include "../lib/map.h"

typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT
} Direction;

typedef struct {
  Player *player;
  Map *map;
} GameManager;

GameManager *new_gamemanager(size_t map_rows, size_t map_cols) {
  GameManager *new_gm;
  if ((new_gm = malloc(sizeof(GameManager))) == NULL) {
    fprintf(stderr, "new_gamemanager: failed to allocate memory for struct\n");
    return NULL;
  }
  
  if ((new_gm->player = new_player()) == NULL) {
    fprintf(stderr, "new_gamemanager: failed to create player\n");
    return NULL;
  }

  if ((new_gm->map = new_map(map_rows, map_cols)) == NULL) {
    fprintf(stderr, "new_gamemanager: failed to create map\n");
    return NULL;
  }

  return new_gm;
}

int free_gamemanager(GameManager *gm) {
  if (gm == NULL) {
    fprintf(stderr, "free_gamemanager: gamemanager is NULL\n");
    return -1;
  }

  free_player(gm->player);
  free_map(gm->map);
  free(gm);

  return 0;
}

int draw_map(GameManager *gm) {
  if (gm == NULL) {
    fprintf(stderr, "draw_map: gamemanager is NULL\n");
    return -1;
  }

  if (gm->map == NULL) {
    fprintf(stderr, "draw_map: map is NULL\n");
  }

  for (int y = 0; y < gm->map->rows; y++) {
    for (int x = 0; x < gm->map->cols; x++) {
      if (x == gm->player->x && y == gm->player->y) {
        printf("@ ");
      } else {
        printf("%c ", gm->map->map[y][x]);
      }
    }
    printf("\n");
  }

  return 0;
}

int move_player(GameManager *gm, Direction direction) {
  if (gm == NULL) {
    fprintf(stderr, "move_player: gamemanager is NULL\n");
    return -1;
  }

  if (gm->player == NULL) {
    fprintf(stderr, "move_player: player is NULL\n");
    return -1;
  }

  if (gm->map == NULL) {
    fprintf(stderr, "move_player: map is NULL\n");
    return -1;
  }

  switch (direction) {
    case UP:
      if (gm->player->y <= 0) break;
      gm->player->y--;
      break;

    case DOWN:
      if (gm->player->y >= gm->map->rows) break;
      gm->player->y++;
      break;

    case LEFT:
      if (gm->player->x <= 0) break;
      gm->player->x--;
      break;

    case RIGHT:
      if (gm->player->x >= gm->map->rows) break;
      gm->player->x++;
      break;

    default:
      fprintf(stderr, "move_player: invalid direction\n");
      return -1;
  }

  return 0;
}
