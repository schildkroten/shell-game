#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

typedef struct Object {
  void *ptr;
  struct Object *next;
} ObjectTracker;

int track_object(ObjectTracker **tracker, void *object_ptr) {
  if (tracker == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (object_ptr == NULL) {
    errno = EINVAL;
    return -1;
  }

  struct Object *new_object = (struct Object *)malloc(sizeof(struct Object));
  if (new_object == NULL) { return -1; }

  new_object->ptr = object_ptr;

  if (*tracker == NULL) {
    new_object->next = NULL;
    *tracker = new_object;
    return 0;
  }

  new_object->next = *tracker;
  *tracker = new_object;

  return 0;
}

int untrack_object(ObjectTracker **tracker, void *object_ptr) {
  if (tracker == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (*tracker == NULL) { return 0; }

  struct Object *current = *tracker;

  if (current->ptr == object_ptr) {
    *tracker = current->next;
    free(current);
    return 0;
  }

  struct Object *prev = NULL;
  while (current != NULL && current->ptr != object_ptr) {
    prev = current;
    current = current->next;
  }

  if (current == NULL) { return 0; }

  prev->next = current->next;
  free(current);

  return 0;
}

int free_tracked_objects(ObjectTracker **tracker) {
  if (tracker == NULL) { return 0; }

  struct Object *object = *tracker;
  struct Object *tmp;
  while (object != NULL) {
    printf("freeing object %p\r\n", object->ptr);

    tmp = object;
    object = object->next;

    free(tmp->ptr);
    free(tmp);
  }

  return 0;
}

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

struct termios orig_terminal;

void disable_raw_mode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_terminal) == -1) {
    perror("disable_raw_mode");
    exit(1);
  }
}

void enable_raw_mode() {
  if (tcgetattr(STDIN_FILENO, &orig_terminal) == -1) {
    perror("enable_raw_mode");
    exit(1);
  }

  struct termios raw_terminal = orig_terminal;

  raw_terminal.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw_terminal.c_oflag &= ~(OPOST);
  raw_terminal.c_cflag |= (CS8);
  raw_terminal.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  raw_terminal.c_cc[VMIN] = 0;
  raw_terminal.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_terminal) == -1) {
    perror("enable_raw_mode");
    exit(1);
  }
}

int get_keypress() {
  int nread;
  char c;

  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) { return -1; }
  }

  if (c == '\x1b') {
    char seq[3];

    if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

    if (seq[0] == '[') {
      if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
        if (seq[2] == '~') {
          switch (seq[1]) {
            case '1': return HOME_KEY;
            case '3': return DEL_KEY;
            case '4': return END_KEY;
            case '5': return PAGE_UP;
            case '6': return PAGE_DOWN;
            case '7': return HOME_KEY;
            case '8': return END_KEY;
          }
        }
      } else {
        switch (seq[1]) {
          case 'A': return ARROW_UP;
          case 'B': return ARROW_DOWN;
          case 'C': return ARROW_RIGHT;
          case 'D': return ARROW_LEFT;
          case 'H': return HOME_KEY;
          case 'F': return END_KEY;
        }
      }
    } else if (seq[0] == 'O') {
      switch (seq[1]) {
        case 'H': return HOME_KEY;
        case 'F': return END_KEY;
      }
    }

    return '\x1b';
  }

  return c;
}

int get_window_size(size_t *cols_res, size_t *rows_res) {
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) { return -1; }

  *cols_res = ws.ws_col;
  *rows_res = ws.ws_row;

  return 0;
}

int get_cursor_pos(unsigned int *x_res, unsigned int *y_res) {
  char buffer[32];

  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) { return -1; }

  unsigned int i = 0;
  for (i = 0; i < sizeof(buffer) - 1; i++) {
    if (read(STDIN_FILENO, &buffer[i], 1) != 1) { break; }
    if (buffer[i] == 'R')  { break; }
  }

  buffer[i] = '\0';

  if (buffer[0] != '\x1b' || buffer[1] != '[') { return -1; }

  if (sscanf(&buffer[2], "%d;%d", y_res, x_res) != 2) { return -1; }

  return 0;
}

int move_cursor_to(unsigned int x, unsigned int y) {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "\x1b[%d;%dH", y + 1, x + 1);
  write(STDOUT_FILENO, buffer, strlen(buffer));

  return 0;
}

void reset_screen() {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
}

#define INIT_APPEND_BUFFER {0, NULL}

typedef struct {
  size_t len;
  char *buffer;
} AppendBuffer;

int append_to_buffer(AppendBuffer *append_buffer, char *str, size_t str_len) {
  if (append_buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (str == NULL || str_len < 1) {
    errno = EINVAL;
    return -1;
  }

  char *new_buffer = (char *)realloc(append_buffer->buffer, append_buffer->len + str_len);
  if (new_buffer == NULL) { return -1; }

  memcpy(&new_buffer[append_buffer->len], str, str_len);

  append_buffer->buffer = new_buffer;
  append_buffer->len += str_len;

  return 0;
}

typedef struct {
  size_t width, height;
  char *buffer;
} FrameBuffer;

int init_frame_buffer(FrameBuffer *frame_buffer, size_t frame_width, size_t frame_height) {
  if (frame_buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (frame_buffer->buffer != NULL) {
    errno = EINVAL;
    return -1;
  }

  if (frame_width < 1 || frame_height < 1) {
    errno = EINVAL;
    return -1;
  }

  frame_buffer->buffer = (char *)malloc(frame_width * frame_height * sizeof(char));

  if (frame_buffer->buffer == NULL) { return -1; }

  memset(frame_buffer->buffer, ' ', frame_width * frame_height * sizeof(char));

  frame_buffer->width = frame_width;
  frame_buffer->height = frame_height;

  return 0;
}

int write_frame_buffer(FrameBuffer frame_buffer) {
  if (frame_buffer.buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (frame_buffer.width < 1 || frame_buffer.height < 1) {
    errno = EINVAL;
    return -1;
  }

  AppendBuffer append_buffer = INIT_APPEND_BUFFER;

  for (unsigned int row = 0; row < frame_buffer.height; row++) {
    if (append_to_buffer(&append_buffer, &frame_buffer.buffer[row * frame_buffer.width], frame_buffer.width) == -1) {
      return -1;
    }

    if (append_to_buffer(&append_buffer, "\x1b[K", 3) == -1) { return -1; }

    if (row == frame_buffer.height - 1) { continue; }

    if (append_to_buffer(&append_buffer, "\r\n", 2) == -1) { return -1; }
  }

  write(STDOUT_FILENO, append_buffer.buffer, append_buffer.len);

  free(append_buffer.buffer);

  return 0;
}

typedef struct {
  size_t width, height;

  size_t content_len;
  char *content;
} Menu;

int init_menu(Menu *menu, size_t menu_width, size_t menu_height, const char *content, size_t content_len) {
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

  menu->width = menu_width;
  menu->height = menu_height;

  menu->content_len = content_len;
  menu->content = (char *)malloc(content_len * sizeof(char));

  if (menu->content == NULL) { return -1; }

  memcpy(menu->content, content, menu->content_len);

  return 0;
}

int draw_menu(Menu menu, unsigned int start_x, unsigned int start_y, FrameBuffer *frame_buffer) {
  if (frame_buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  frame_buffer->buffer[start_y * frame_buffer->width + start_x] = '+';
  frame_buffer->buffer[start_y * frame_buffer->width + start_x + menu.width - 1] = '+';
  frame_buffer->buffer[(start_y + menu.height - 1) * frame_buffer->width + start_x] = '+';
  frame_buffer->buffer[(start_y + menu.height - 1) * frame_buffer->width + start_x + menu.width - 1] = '+';

  for (unsigned int y = start_y + 1; y < menu.height + start_y - 1; y++) {
    frame_buffer->buffer[y * frame_buffer->width + start_x] = '|';
    frame_buffer->buffer[y * frame_buffer->width + start_x + menu.width - 1] = '|';
  }

  for (unsigned int x = start_x + 1; x < menu.width + start_x - 1; x++) {
    frame_buffer->buffer[start_y * frame_buffer->width + x] = '-';
    frame_buffer->buffer[(start_y + menu.height - 1) * frame_buffer->width + x] = '-';
  }

  unsigned int current_char = 0;
  for (unsigned int y = 1; y < menu.height - 1; y++) {
    for(unsigned int x = 2; x < menu.width - 2; x++) {
      if (current_char == menu.content_len) { return 0; }

      switch (menu.content[current_char]) {
        case '\n':
          frame_buffer->buffer[(y + start_y) * frame_buffer->width + x + start_x] = ' ';
          break;

        default:
          frame_buffer->buffer[(y + start_y) * frame_buffer->width + x + start_x] = menu.content[current_char];
          current_char++;

          break;
      }
    }

    if (menu.content[current_char] == '\n') { current_char++; }
  }

  return 0;
}
