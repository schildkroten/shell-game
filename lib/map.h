#ifndef MAP_H
#define MAP_H

#include <stdlib.h>

typedef struct {
  int rows, cols;
  char **map;
} Map;

Map *new_map(int cols, int rows);
int free_map(Map *map);

#endif
