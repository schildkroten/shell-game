#ifndef IO_H
#define IO_H

#include <stdlib.h>

typedef struct Object {
  void *ptr;
  struct Object *next;
} ObjectTracker;

int track_object(ObjectTracker **tracker, void *object_ptr);
int untrack_object(ObjectTracker **tracker, void *object_ptr);
int free_tracked_objects(ObjectTracker **tracker);

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

int get_window_size(size_t *cols_res, size_t *rows_res);
int get_cursor_pos(unsigned int *x_res, unsigned int *y_res);
int move_cursor_to(unsigned int x, unsigned int y);

void reset_screen();

#define INIT_APPEND_BUFFER {0, NULL}

typedef struct {
  size_t len;
  char *buffer;
} AppendBuffer;

int append_to_buffer(AppendBuffer *append_buffer, char *str, size_t str_len);

#define FRAME_BUFFER_BASE {0, 0, NULL}

typedef struct {
  size_t width, height;
  char *buffer;
} FrameBuffer;

int init_frame_buffer(FrameBuffer *frame_buffer, size_t frame_width, size_t frame_height);
int write_frame_buffer(FrameBuffer frame_buffer);

#define MENU_BASE {0, 0, 0, NULL}

typedef struct {
  size_t width, height;

  size_t content_len;
  char *content;
} Menu;

int init_menu(Menu *menu, size_t menu_width, size_t menu_height, const char *content, size_t content_len, ObjectTracker **tracker);
int update_menu_content(Menu *menu, const char *content, size_t content_len, ObjectTracker **tracker);
int draw_menu(Menu menu, unsigned int start_x, unsigned int start_y, FrameBuffer *frame_buffer);

#endif
