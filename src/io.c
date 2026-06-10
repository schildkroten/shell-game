#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

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
    fprintf(stderr, "disable_raw_mode: %s\r\n", strerror(errno));
    exit(1);
  }
}

void enable_raw_mode() {
  if (tcgetattr(STDIN_FILENO, &orig_terminal) == -1) {
    fprintf(stderr, "enable_raw_mode: %s\r\n", strerror(errno));
    exit(1);
  }

  atexit(disable_raw_mode);

  struct termios raw_terminal = orig_terminal;

  raw_terminal.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw_terminal.c_oflag &= ~(OPOST);
  raw_terminal.c_cflag |= (CS8);
  raw_terminal.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  raw_terminal.c_cc[VMIN] = 0;
  raw_terminal.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_terminal) == -1) {
    fprintf(stderr, "enable_raw_mode: %s\r\n", strerror(errno));
    exit(1);
  }
}

int get_keypress() {
  int nread;
  char c;

  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) {
      fprintf(stderr, "get_keypress: %s\r\n", strerror(errno));
      exit(1);
    }
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

typedef struct {
  int len;
  char *buffer;
} AppendBuffer;

AppendBuffer *new_append_buffer() {
  AppendBuffer *new_append_buffer;
  if ((new_append_buffer = malloc(sizeof(AppendBuffer))) == NULL) {
    fprintf(stderr, "new_append_buffer: failed to allocate memory for struct\r\n");
    return NULL;
  }

  new_append_buffer->len = 0;

  return new_append_buffer;
}

int free_append_buffer(AppendBuffer *buffer) {
  if (buffer == NULL) {
    fprintf(stderr, "free_buffer: buffer is NULL\r\n");
    return -1;
  }

  free(buffer->buffer);
  free(buffer);

  return 0;
}

int append_to_buffer(AppendBuffer *append_buffer, const char *str, int len) {
  if (append_buffer == NULL) {
    fprintf(stderr, "append_to_buffer: buffer is NULL\r\n");
    return -1;
  }

  append_buffer->buffer = realloc(append_buffer->buffer, append_buffer->len + len);

  if (append_buffer->buffer == NULL) {
    fprintf(stderr, "append_to_buffer: realloc failed\r\n");
    return -1;
  }

  memcpy(&append_buffer->buffer[append_buffer->len], str, len);
  append_buffer->len += len;

  return 0;
}

typedef struct {
  int cols, rows;
  char **buffer;
} ScreenBuffer;

ScreenBuffer *new_screen_buffer(int cols, int rows) {
  ScreenBuffer *new_screen_buffer;
  if ((new_screen_buffer = malloc(sizeof(ScreenBuffer))) == NULL) {
    fprintf(stderr, "new_screen_buffer: failed to allocate memory for struct\r\n");
    return NULL;
  }

  if ((new_screen_buffer->buffer = malloc(sizeof(char *) * rows)) == NULL) {
    fprintf(stderr, "new_screen_buffer: failed to allocate memory for buffer rows\r\n");
    return NULL;
  }

  for (int i = 0; i < rows; i++) {
    if ((new_screen_buffer->buffer[i] = malloc(sizeof(char) * cols)) == NULL) {
      fprintf(stderr, "new_screen_buffer: failed to allocate memory for buffer columns\r\n");
      return NULL;
    }
  }

  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      new_screen_buffer->buffer[y][x] = ' ';
    }
  }

  new_screen_buffer->cols = cols;
  new_screen_buffer->rows = rows;

  return new_screen_buffer;
}

int free_screen_buffer(ScreenBuffer *screen_buffer) {
  if (screen_buffer == NULL) {
    fprintf(stderr, "free_screen_buffer: buffer is NULL\r\n");
    return -1;
  }

  for (int i = 0; i < screen_buffer->rows; i++) {
    free(screen_buffer->buffer[i]);
  }

  free(screen_buffer);

  return 0;
}

int write_screen_buffer(ScreenBuffer *screen_buffer) {
  if (screen_buffer == NULL) {
    fprintf(stderr, "insert_into_screen_buffer: buffer is null\r\n");
    return -1;
  }

  AppendBuffer *append_buffer = new_append_buffer();

  for (int i = 0; i < screen_buffer->rows; i++) {
    append_to_buffer(append_buffer, screen_buffer->buffer[i], screen_buffer->cols);
    append_to_buffer(append_buffer, "\r\n", 2);
  }

  write(STDOUT_FILENO, append_buffer->buffer, append_buffer->len);

  free_append_buffer(append_buffer);

  return 0;
}

typedef struct {
  int width, height;

  int content_len;
  char *content;
} Menu;

Menu *new_menu(int width, int height, const char *content, int content_len) {
  Menu *new_menu;
  if ((new_menu = malloc(sizeof(Menu))) == NULL) {
    fprintf(stderr, "new_menu: failed to allocate memory for struct\r\n");
    return NULL;
  }

  if ((new_menu->content = malloc(sizeof(char) * content_len)) == NULL) {
    fprintf(stderr, "new_menu: failed to allocate memory for menu content\r\n");
    return NULL;
  }

  if ((new_menu->content = strdup(content)) == NULL) {
    fprintf(stderr, "new_menu: failed to set content feild of menu\r\n");
    return NULL;
  }

  new_menu->width = width;
  new_menu->height = height;
  new_menu->content_len = content_len;

  return new_menu;
}

int free_menu(Menu *menu) {
  if (menu == NULL) {
    fprintf(stderr, "free_menu: menu is NULL\r\n");
    return -1;
  }

  free(menu->content);
  free(menu);

  return 0;
}

int draw_menu(Menu *menu, ScreenBuffer *buffer, int start_x, int start_y) {
  if (menu == NULL) {
    fprintf(stderr, "draw_menu: menu is NULL\r\n");
    return -1;
  }

  if (buffer == NULL) {
    fprintf(stderr, "draw_menu: buffer is NULL\r\n");
    return -1;
  }

  return 0;
}
