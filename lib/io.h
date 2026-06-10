#ifndef IO_H
#define IO_H

#define CTRL_KEY(k) ((k) & 0x1f)

enum Keys {
  ARROW_UP = 1000,
  ARROW_DOWN,
  ARROW_LEFT,
  ARROW_RIGHT,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN
};

void enable_raw_mode();
void disable_raw_mode();

int get_keypress();

typedef struct {
  int len;
  char *buffer;
} AppendBuffer;

AppendBuffer *new_append_buffer();
int free_buffer(AppendBuffer *append_buffer);

int append_to_buffer(AppendBuffer *append_buffer, const char *str, int len);

typedef struct {
  int cols, rows;
  char **buffer;
} ScreenBuffer;

ScreenBuffer *new_screen_buffer(int cols, int rows);
int free_screen_buffer(ScreenBuffer *screen_buffer);

int write_screen_buffer(ScreenBuffer *screen_buffer);

typedef struct {
  int width, height;

  int content_len;
  char *content;
} Menu;

Menu *new_menu(int width, int height, const char *content, int content_len);
int free_menu(Menu *menu);

int draw_menu(Menu *menu, ScreenBuffer *screen_buffer, int start_x, int start_y);

#endif
