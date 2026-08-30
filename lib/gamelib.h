/* Libray for this game created by Angus Bonney 2026 */

#ifndef GAMELIB_H
#define GAMELIB_H

#include "engine.h"

typedef enum {
  PICKAXE,
  AXE,
  WOOD,
  STONE
} ItemID;

/* ItemType is for identifying what kind of data
 * is stored in an item struct. */
typedef enum {
  TOOL,
  RESOURCE
} ItemType;

/* The ToolData struct is put into the Item struct
 * under the data pointer and holds info specific to
 * tools. */
typedef struct {
  ItemID id;
  uint8_t damage;
  uint8_t durability;
  uint8_t breaking_power;
} ToolData;

/* Rsource data is put into the Item struct under
 * the data pointer and only needs to hold the
 * ID of the resource. */
typedef struct {
  ItemID id;
} ResourceData;

/* The Item struct is what is interfaced. It pretty
 * much just holds some data and an ItemType for 
 * how that data should be interperated. */
typedef struct {
  ItemType type;
  char symbol;
  void *data;
} Item;

/* Creates a new Item. Returns a pointer to the item on
 * success and NULL on failure. */
Item *create_item(ItemType type, char symbol, void *data);

#define INVENTORY_SIZE 64

/* The Player struct holds all info related to the playey. */
typedef struct {
  uint8_t x, y;
  uint8_t health, max_health;
  uint8_t defense;
  uint8_t hunger, thirst;

  /* The inventory is a fixed array of Item pointers. */
  Item *inventory[INVENTORY_SIZE];
} Player;

/* The Map struct just holds its width, height, and an
 * array that holds the actual map. */
typedef struct {
  size_t width, height;
  char *base;
} Map;

#define MAX_ENEMYS 32

/* The enemy struct is similar to the Player struct but
 * can only hold one item. */
typedef struct {
  uint8_t x, y;

  int8_t health;
  uint8_t defense;

  Item *item;
} Enemy;

/* Creates a new enemy. Return a pointer to the enemy on
 * success and NULL on failure. */
Enemy *create_enemy(uint8_t x, uint8_t y, uint8_t health, uint8_t defense, Item *item);

/* The GAME_MANAGER_BASE defines the base GameManager struct.
 * It's quite big as it also needs to define the base states
 * of all the structs contained in it. */
#define GAME_MANAGER_BASE \
  {\
    0, 0, /* Win_width, Win_height */\
    0, 0, /* Cursor_x, Cursor_y */\
    \
    { /* Player */\
      0, 0, /* x, y */\
      10, 10, /* Health, Max_health */\
      0, /* Defense */\
      0, 0, /* Hunger, Thirst */\
      {[0 ... INVENTORY_SIZE - 1] = NULL} /* Inventory */\
    },\
    \
    { /* Map */\
      0, 0, /* Width, Height */\
      NULL /* Base Map Array */\
    },\
    { /* Enemys */\
      [0 ... MAX_ENEMYS - 1] = NULL\
    }\
  }

/* The GameManager struct holds all the info for the
 * game in one easily accessable place. */
typedef struct {
  size_t win_width, win_height;
  unsigned int cursor_x, cursor_y;

  Player player;
  Map map;

  /* The enemys are stored in a fixed array of
   * Enemy pointers. */
  Enemy *enemys[MAX_ENEMYS];
} GameManager;

/* Fills out all the fields of a GameManager struct. Returns -1
 * on failure. */
int init_game_manager(GameManager *gm, ObjectTracker **tracker);

/* Adds and item to the inventory. Returns -1 on failure
 * and 1 when the inventory os full. */
int add_to_inventory(GameManager *gm, Item *item);

/* Removes an item from the inventory. Returns -1 on failure. */
int remove_from_inventory(GameManager *gm, Item *item);

/* Converts the current inventory into a string so it can be
 * displayed in a menu. Returns -1 on failure. */
int inventory_to_str(GameManager *gm, char *buf, size_t buf_size);

/* Converts the players current stats to a string to be put in
 * a menu. Returns -1 on failure. */
int stats_to_str(GameManager *gm, char *buf, size_t buf_size);

/* Draws the map to a FrameBuffer witch the top left corner at
 * (start_x, start_y). Returns -1 on failure. */
int draw_map(GameManager *gm, unsigned int start_x, unsigned int start_y, FrameBuffer *frame_buffer);

/* Defines the different directions players and enemys
 * can move. */
typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT
} Direction;

/* Moves the player in a direction. Wont move the player
 * outside the map and returns -1 on failure. */
int move_player(GameManager *gm, Direction direction);

/* Moves the cursor to (x, y) and stores that in the game manager.
 * Returns -1 on failure. */
int move_cursor(GameManager *gm, unsigned int x, unsigned int y);

/* The base Menu struct. */
#define MENU_BASE {0, 0, 0, NULL}

/* The Menu struct just holds the width and height of the
 * window as well as the string it holds and its length. */
typedef struct {
  size_t width, height;

  size_t content_len;
  char *content;
} Menu;

/* Initilizes a Menu struct. Returns -1 on failure. */
int init_menu(Menu *menu, size_t menu_width, size_t menu_height, const char *content, size_t content_len, ObjectTracker **tracker);

/* Updates a Menus content field. Returns -1 on failure. */
int update_menu_content(Menu *menu, const char *content, size_t content_len, ObjectTracker **tracker);

/* Draws a Menu into a FrameBuffer with the top left corner at
 * (start_x, start_y). Returns -1 on failure. */
int draw_menu(Menu *menu, unsigned int start_x, unsigned int start_y, FrameBuffer *frame_buffer);

#endif
