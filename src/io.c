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
    perror("disable_raw_mode");
    exit(1);
  }
}

void enable_raw_mode() {
  if (tcgetattr(STDIN_FILENO, &orig_terminal) == -1) {
    perror("enable_raw_mode");
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
    perror("enable_raw_mode");
    exit(1);
  }
}

int get_keypress() {
  int nread;
  char c;

  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) {
      perror("get_keypress");
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
    return NULL;
  }

  new_append_buffer->len = 0;

  return new_append_buffer;
}

int free_append_buffer(AppendBuffer *buffer) {
  if (buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  free(buffer->buffer);
  free(buffer);

  return 0;
}

int append_to_buffer(AppendBuffer *append_buffer, const char *str, int len) {
  if (append_buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  append_buffer->buffer = realloc(append_buffer->buffer, append_buffer->len + len);

  if (append_buffer->buffer == NULL) {
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
    return NULL;
  }

  if ((new_screen_buffer->buffer = malloc(sizeof(char *) * rows)) == NULL) {
    return NULL;
  }

  for (int i = 0; i < rows; i++) {
    if ((new_screen_buffer->buffer[i] = malloc(sizeof(char) * cols)) == NULL) {
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
    errno = EINVAL;
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
    errno = EINVAL;
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

  int open;

  int content_len;
  char *content;
} Menu;

Menu *new_menu(int width, int height, const char *content, int content_len) {
  Menu *new_menu;
  if ((new_menu = malloc(sizeof(Menu))) == NULL) {
    return NULL;
  }

  if ((new_menu->content = malloc(sizeof(char) * content_len)) == NULL) {
    return NULL;
  }

  if ((new_menu->content = strdup(content)) == NULL) {
    return NULL;
  }

  new_menu->width = width;
  new_menu->height = height;
  new_menu->open = 0;
  new_menu->content_len = content_len;

  return new_menu;
}

int free_menu(Menu *menu) {
  if (menu == NULL) {
    errno = EINVAL;
    return -1;
  }

  free(menu->content);
  free(menu);

  return 0;
}

int draw_menu(Menu *menu, ScreenBuffer *screen_buffer, int start_x, int start_y) {
  if (menu == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (screen_buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (screen_buffer->rows < start_y + menu->height || screen_buffer->cols < start_x + menu->width) {
    errno = EINVAL;
    return -1;
  }

  if (menu->open == 0) {
    return 0;
  }

  screen_buffer->buffer[start_y][start_x] = '+';
  screen_buffer->buffer[start_y][start_x + menu->width - 1] = '+';
  screen_buffer->buffer[start_y + menu->height - 1][start_x] = '+';
  screen_buffer->buffer[start_y + menu->height - 1][start_x + menu->width - 1] = '+';

  for (int x = 1; x < menu->width - 1; x++) {
    screen_buffer->buffer[start_y][x + start_x] = '-';
    screen_buffer->buffer[start_y + menu->height - 1][x + start_x] = '-';
  }

  for (int y = 1; y < menu->height - 1; y++) {
    screen_buffer->buffer[y + start_y][start_x] = '|';
    screen_buffer->buffer[y + start_y][start_x + menu->width - 1] = '|';
  }

  int current_char = 0;
  for (int y = 1; y < menu->height - 1; y++) {
    for (int x = 2; x < menu->width - 2; x++) {
      if (menu->content[current_char] == '\n') {
        screen_buffer->buffer[y + start_y][x + start_x] = ' ';
      } else {
        screen_buffer->buffer[y + start_y][x + start_x] = menu->content[current_char];

        if (current_char == menu->content_len - 1) break;

        current_char++;
      }
    }
    if (current_char == menu->content_len - 1) break;

    if (menu->content[current_char] == '\n') {
      current_char++;
    }
  }

  return 0;
}
