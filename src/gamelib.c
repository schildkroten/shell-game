#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "../lib/gamelib.h"

Item *create_item(ItemType type, char symbol, void *data) {
  Item *new_item = malloc(sizeof(Item));
  if (new_item == NULL) { return NULL; }

  new_item->type = type;
  new_item->symbol = symbol;
  new_item->data = data;

  return new_item;
}

Enemy *create_enemy(uint8_t x, uint8_t y, uint8_t health, uint8_t defense, Item *item) {
  Enemy *new_enemy = malloc(sizeof(Enemy));
  if (new_enemy == NULL) { return NULL; }

  new_enemy->x = x;
  new_enemy->y = y;
  new_enemy->health = health;
  new_enemy->defense = defense;
  new_enemy->item = item;

  return new_enemy;
}

int init_game_manager(GameManager *gm, ObjectTracker **tracker) {
  if (gm == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (gm->map.base != NULL) {
    errno = EINVAL;
    return -1;
  }

  if (tracker == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (get_window_size(&gm->win_width, &gm->win_height) == -1) { return -1; }
  if (get_cursor_pos(&gm->cursor_x, &gm->cursor_y) == -1) { return -1; }

  if (gm->win_width < 90 || gm->win_height < 24) {
    errno = ENOMSG;
    return -1;
  }

  gm->map.width = 60;
  gm->map.height = 24;
  gm->map.base = malloc(gm->map.width * gm->map.height * sizeof(char));

  if (gm->map.base == NULL) { return -1; }

  memset(gm->map.base, ',', gm->map.width * gm->map.height * sizeof(char));

  if (track_object(tracker, gm->map.base) == -1) { return -1; } 

  return 0;
}

int add_to_inventory(GameManager *gm, Item *item) {
  if (gm == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (item == NULL) {
    errno = EINVAL;
    return -1;
  }

  int i;
  for (i = 0; i < INVENTORY_SIZE - 1; i++) {
    if (gm->player.inventory[i] == item) {
      errno = EINVAL;
      return -1;
    }

    if (gm->player.inventory[i] == NULL) {
      gm->player.inventory[i] = item;
      return 0;
    }
  }

  if (i == INVENTORY_SIZE - 1) {
    return 1;
  }

  return 0;
}

int remove_from_inventory(GameManager *gm, Item *item) {
  if (gm == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (item == NULL) {
    errno = EINVAL;
    return -1;
  }

  uint8_t i;
  for (i = 0; i < INVENTORY_SIZE - 1; i++) {
    if (gm->player.inventory[i] == item) {
      gm->player.inventory[i] = NULL;
      return 0;
    }
  }

  if (i == INVENTORY_SIZE - 1) {
    return 1;
  }

  return 0;
}

int inventory_to_str(GameManager *gm, char *buf, size_t buf_size) {
  if (buf == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (buf_size < 1) {
    errno = EINVAL;
    return -1;
  }

  uint8_t i;
  for (i = 0; i < INVENTORY_SIZE - 1 && i < buf_size - 1; i++) {
    if (gm->player.inventory[i] == NULL) {
      buf[i] = ' ';
      continue;
    }

    buf[i] = gm->player.inventory[i]->symbol;
  }

  buf[i] = '\0';

  return 0;
}

#define BAR_SIZE 11

int stats_to_str(GameManager *gm, char *buf, size_t buf_size) {
  if (buf == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (buf_size < 1) {
    errno = EINVAL;
    return -1;
  }

  snprintf(
      buf,
      buf_size,
      "Stats\n"
      "Pos: %d, %d\n"
      "Health: %d/%d\n"
      "Defense: %d\n"
      "Hunger: %d/100\n"
      "Thirst: %d/100",
      gm->player.x,
      gm->player.y,
      gm->player.health,
      gm->player.max_health,
      gm->player.defense,
      gm->player.hunger,
      gm->player.thirst
  );

  return 0;
}

int draw_map(GameManager *gm, unsigned int start_x, unsigned int start_y, FrameBuffer *frame_buffer) {
  if (frame_buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  for (size_t y = start_y; y < gm->map.height + start_y; y++) {
    memcpy(&frame_buffer->buffer[y * frame_buffer->width + start_x], &gm->map.base[y * gm->map.width], gm->map.width);
  }

  for (int i = 0; i < MAX_ENEMYS - 1; i++) {
    if (gm->enemys[i] == NULL) { continue; }
    frame_buffer->buffer[(gm->enemys[i]->y + start_y) * frame_buffer->width + gm->enemys[i]->x] = '!';
  } 

  frame_buffer->buffer[gm->player.y * frame_buffer->width + gm->player.x] = '@';

  return 0;
}

int move_player(GameManager *gm, Direction direction) {
  if (gm == NULL) {
    errno = EINVAL;
    return -1;
  }

  switch (direction) {
    case UP:
      if (gm->player.y <= 0) { return 0; }
      gm->player.y--;
      break;

    case DOWN:
      if (gm->player.y >= gm->map.height - 1) { return 0; }
      gm->player.y++;
      break;

    case LEFT:
      if (gm->player.x <= 0) { return 0; }
      gm->player.x--;
      break;

    case RIGHT:
      if (gm->player.x >= gm->map.width - 1) { return 0; }
      gm->player.x++; 
      break;

    default:
      return 0;
  }

  return 0;
}

int move_cursor(GameManager *gm, unsigned int x, unsigned int y) {
  if (gm == NULL) {
    errno = EINVAL;
    return -1;
  }

  char buffer[32];
  snprintf(buffer, sizeof(buffer), "\x1b[%d;%dH", y + 1, x + 1);
  write(STDOUT_FILENO, buffer, strlen(buffer));

  gm->cursor_x = x;
  gm->cursor_y = y;

  return 0;
}

int init_menu(Menu *menu, size_t menu_width, size_t menu_height, const char *content, size_t content_len, ObjectTracker **tracker) {
  if (menu == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (content == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (menu_width < 1 || menu_height < 1) {
    errno = EINVAL;
    return -1;
  }

  if (content_len < 1) {
    errno = EINVAL;
    return -1;
  }

  if (tracker == NULL) {
    errno = EINVAL;
    return -1;
  }

  menu->width = menu_width;
  menu->height = menu_height;

  menu->content_len = content_len;
  menu->content = (char *)malloc(content_len * sizeof(char) + 1);

  if (menu->content == NULL) { return -1; }

  if (track_object(tracker, menu->content) == -1) { return -1; }

  memcpy(menu->content, content, menu->content_len);

  return 0;
}

int update_menu_content(Menu *menu, const char *content, size_t content_len, ObjectTracker **tracker) {
  if (menu == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (content == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (content_len < 1) {
    errno = EINVAL;
    return -1;
  }

  if (untrack_object(tracker, menu->content) == -1) { return -1; }

  free(menu->content);

  menu->content_len = content_len;
  menu->content = (char *)malloc(content_len * sizeof(char) + 1);

  if (menu->content == NULL) { return -1; }

  if (track_object(tracker, menu->content) == -1) { return -1; }

  memcpy(menu->content, content, menu->content_len);

  return 0;
}

int draw_menu(Menu *menu, unsigned int start_x, unsigned int start_y, FrameBuffer *frame_buffer) {
  if (frame_buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  frame_buffer->buffer[start_y * frame_buffer->width + start_x] = '+';
  frame_buffer->buffer[start_y * frame_buffer->width + start_x + menu->width - 1] = '+';
  frame_buffer->buffer[(start_y + menu->height - 1) * frame_buffer->width + start_x] = '+';
  frame_buffer->buffer[(start_y + menu->height - 1) * frame_buffer->width + start_x + menu->width - 1] = '+';

  for (unsigned int y = start_y + 1; y < menu->height + start_y - 1; y++) {
    frame_buffer->buffer[y * frame_buffer->width + start_x] = '|';
    frame_buffer->buffer[y * frame_buffer->width + start_x + menu->width - 1] = '|';
  }

  for (unsigned int x = start_x + 1; x < menu->width + start_x - 1; x++) {
    frame_buffer->buffer[start_y * frame_buffer->width + x] = '-';
    frame_buffer->buffer[(start_y + menu->height - 1) * frame_buffer->width + x] = '-';
  }

  unsigned int current_char = 0;
  for (unsigned int y = 1; y < menu->height - 1; y++) {
    for(unsigned int x = 2; x < menu->width - 2; x++) {
      if (current_char == menu->content_len - 1) { return 0; }

      switch (menu->content[current_char]) {
        case '\n':
          frame_buffer->buffer[(y + start_y) * frame_buffer->width + x + start_x] = ' ';
          break;

        default:
          frame_buffer->buffer[(y + start_y) * frame_buffer->width + x + start_x] = menu->content[current_char];
          current_char++;

          break;
      }
    }

    if (menu->content[current_char] == '\n') { current_char++; }
  }

  return 0;
}
