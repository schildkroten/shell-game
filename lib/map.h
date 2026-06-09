#ifndef MAP_H
#define MAP_H

#include <stdlib.h>

typedef struct {
  int rows;
  int cols;
  char **map;
} Map;

Map *new_map(size_t cols, size_t rows);
int free_map(Map *map);

#endif
