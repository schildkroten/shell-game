#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

typedef struct {
  int rows, cols;
  char **map;
} Map;

Map *new_map(int cols, int rows) {
  if (cols <= 0 || rows <= 0) {
    errno = EINVAL;
    return NULL;
  }

  Map *new_map;
  if ((new_map = malloc(sizeof(Map))) == NULL) {
    return NULL;
  }

  if ((new_map->map = malloc(sizeof(char *) * rows)) == NULL) {
    return NULL;
  }

  for (int i = 0; i < rows; i++) {
    if ((new_map->map[i] = malloc(sizeof(char) * cols)) == NULL) {
      return NULL;
    }
  }

  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      new_map->map[y][x] = ',';
    }
  }

  new_map->rows = rows;
  new_map->cols = cols;

  return new_map;
}

int free_map(Map *map) {
  if (map == NULL) {
    errno = EINVAL;
    return -1;
  }

  for (int i = 0; i < map->rows; i++) {
    free(map->map[i]);
  }

  free(map->map);

  map->cols = 0;
  map->rows = 0;

  free(map);

  return 0;
}
