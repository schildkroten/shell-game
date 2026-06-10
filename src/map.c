#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
  int rows, cols;
  char **map;
} Map;

Map *new_map(int cols, int rows) {
  if (cols <= 0 || rows <= 0) {
    fprintf(stderr, "new_map: cols and rows must be >= 1\r\n");
    return NULL;
  }

  Map *new_map;
  if ((new_map = malloc(sizeof(Map))) == NULL) {
    fprintf(stderr, "new_map: failed to allocate memory for struct\r\n");
    return NULL;
  }

  if ((new_map->map = malloc(sizeof(char *) * rows)) == NULL) {
    fprintf(stderr, "new_map: failed to allocate memory for map rows\r\n");
    return NULL;
  }

  for (int i = 0; i < rows; i++) {
    if ((new_map->map[i] = malloc(sizeof(char) * cols)) == NULL) {
      fprintf(stderr, "new_map: failed to allocate memory for map columns\r\n");
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
    fprintf(stderr, "free_map: map is NULL\r\n");
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
