/* General purpose terminal IO library created by Angus Bonney 2026 */

#ifndef IO_H
#define IO_H

#include <stdlib.h>

/* The ObjectTracker type is a linked list to store
 * pointers to various objects. */
typedef struct Object {
  void *ptr;
  struct Object *next;
} ObjectTracker;

/* Stores a pointer in an ObjectTracker */
int track_object(ObjectTracker **tracker, void *object_ptr);

/* Removes a pointer from an ObjectTracker */
int untrack_object(ObjectTracker **tracker, void *object_ptr);

/* Frees all the pointers in an ObjectTracker */
int free_tracked_objects(ObjectTracker **tracker);

/* Enable/disable raw mode for the terminal. Raw mode is
 * just a mode where keyboard presses go straight to
 * the program instead of buffered by the terminal. */
void enable_raw_mode();
void disable_raw_mode();

#define CTRL_KEY(k) ((k) & 0x1f)

/* This enum just holds some definitions for
 * non ascii keys that can be pressed. */
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

/* Gets a keypress from the user. Returns the ascii
 * key code or keys enum value or -1 on faliure. */
int get_keypress();

/* Fetches the terminal window size and puts it into the
 * cols_res and rows_res pointers. Returns -1 on faliure. */
int get_window_size(size_t *cols_res, size_t *rows_res);

/* Gets the current cursor position on screen and puts it into 
 * the x_res and y_res pointers. Returns -1 on failure. */
int get_cursor_pos(unsigned int *x_res, unsigned int *y_res);

/* Clears the screen. */
void reset_screen();

/* The base AppendBuffer struct. */
#define INIT_APPEND_BUFFER {0, NULL}

/* The AppendBuffer struct holds a resizable string and its
 * length. */
typedef struct {
  size_t len;
  char *buffer;
} AppendBuffer;

/* Appends a string to an AppendBuffer. Returns -1 on failure */
int append_to_buffer(AppendBuffer *append_buffer, char *str, size_t str_len);

/* The base FrameBuffer struct. */
#define FRAME_BUFFER_BASE {0, 0, NULL}

/* The FrameBuffer struct acts as a grid of characters. */
typedef struct {
  size_t width, height;
  char *buffer;
} FrameBuffer;

/* Allocates memory for the FrameBuffer buffer. Returns -1 on failure. */
int init_frame_buffer(FrameBuffer *frame_buffer, size_t frame_width, size_t frame_height);

/* Writes the contents of a FrameBuffer to STDOUT */
int write_frame(FrameBuffer frame_buffer);

#endif
