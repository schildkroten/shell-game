#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int cols;
  int rows;
  char **map;
} Map;

Map *new_map(size_t cols, size_t rows) {
  if (cols <= 0 || rows <= 0) {
    fprintf(stderr, "new_map: cols and rows must be >= 1");
    return NULL;
  }

  Map *new_map;
  if ((new_map = malloc(sizeof(Map))) == NULL) {
    fprintf(stderr, "new_map: failed to allocate memory for struct");
    return NULL;
  }

  if ((new_map->map = malloc(sizeof(char *) * rows)) == NULL) {
    fprintf(stderr, "new_map: failed to allocate memory for map rows");
    return NULL;
  }

  for (int i = 0; i < cols; i++) {
    new_map->map[i] = malloc(sizeof(char) * cols);
    if (new_map->map[i] == NULL) {
      fprintf(stderr, "new_map: failed to allocate memory for map columns");
      return NULL;
    }
  }

  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      new_map->map[y][x] = '#';
    }
  }

  new_map->cols = cols;
  new_map->rows = rows;

  return new_map;
}

int free_map(Map *map) {
  if (map == NULL) {
    fprintf(stderr, "free_map: map to free cannot be NULL");
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

int draw_map(Map *map) {
  if (map == NULL) {
    fprintf(stderr, "draw_map: map to draw cannot be NULL");
    return -1;
  }

  for (int y = 0; y < map->rows; y++) {
    for (int x = 0; x < map->cols; x++) {
      printf("%c ", map->map[y][x]);
    }
    printf("\n");
  }

  return 0;
}

