#ifndef MAP_H
#define MAP_H

#include <stdlib.h>

typedef struct {} Map;

Map *new_map(size_t cols, size_t rows);
int free_map(Map *map);
int draw_map(Map *map);

#endif
