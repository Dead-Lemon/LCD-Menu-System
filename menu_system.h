#include "Arduino.h"
#include <LiquidCrystal_I2C.h>

#define MENU_MAX_SIZE 100 //max number of menu entries
#define LCD_COLUMNS 16
#define LCD_ROWS 2
#define LCD_I2C_ADDRESS 0x27

enum VAR_TYPE_ENUM {
  E_BOOL,
  E_INT8,
  E_INT16,
  E_INT32,
  E_UINT8,
  E_UINT16,
  E_UINT32,
  E_FLOAT,
  E_CHAR,
  E_NULL,
  E_RETURN,
};

enum MENU_NAVIGATION_ENUM {
  E_NONE,
  E_UP,
  E_DOWN,
  E_BACK,
  E_ENTER,
};

enum MENU_SYMBOL_ENUM {
  E_DOWN_ARROW,
  E_UP_ARROW,
  E_CURSOR,
  E_TICKED,
  E_UNTICKED,
  E_ENTER_ARROW,
};

union VAR_POINTER_UNION {
  bool* U_BOOL;
  int8_t* U_INT8;
  int16_t* U_INT16;
  int32_t* U_INT32;
  uint8_t* U_UINT8;
  uint16_t* U_UINT16;
  uint32_t* U_UINT32;
  float* U_FLOAT;
  char* U_CHAR;
};

union VAR_UNION {
  bool U_BOOL;
  int8_t U_INT8;
  int16_t U_INT16;
  int32_t U_INT32;
  uint8_t U_UINT8;
  uint16_t U_UINT16;
  uint32_t U_UINT32;
  float U_FLOAT;
  char U_CHAR;
};


static const byte menu_downArrow[8] = {
  0b00000, //   
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b10101, // * * *
  0b01110, //  ***
  0b00100  //   *
};

static const byte menu_upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b10101, // * * *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00000  //   
};
//cursor
static const byte menu_cursor[8] = {
  B00000, //  
  B00100, //   *
  B00010, //    *
  B00001, //     *
  B00010, //    *
  B00100, //   *
  B00000, //  
  B00000  //
};

//  tick mark 
static const byte menu_ticked[8] = {
  B00000, //     
  B11011, //** **  
  B10001, //*   *
  B01110, // ***   
  B01110, // ***  
  B01110, // ***  
  B10001, //*   *
  B11011  //** **     
};
// unticked
static const byte menu_unticked[8] = {
  B00000, //     
  B11011, //** **     
  B10001, //*   *
  B00000, //     
  B00000, // 
  B00000, //
  B10001, //*   * 
  B11011  //** **       
};
//enter
static const byte menu_enter[8] = {
  B00000, //    
  B00001, //    * 
  B00001, //    *
  B00101, //  * *
  B01001, // *  *
  B11111, //*****  
  B01000, // *   
  B00100  //  *   
};

struct TIMER_SIMPLE {
  uint32_t delay; //debounce timer setpoint
  uint32_t start_time; //
  bool lapsed; //state of timer
};

struct MENU_ITEM {
  char *label;
  uint8_t parent_id, id; //id is location in menu array, parent id links to anther, -1 for root
  union VAR_POINTER_UNION var_pointer;
  uint8_t var_type;
  bool edit, show_edit; //set menu entry as editable.
  bool end; //terminator for detecting end of list
};

//main control object, keeps list of menu items to navigate and display
struct MENU_SYSTEM {
  uint8_t head; //potition of next open slot in list array
  uint8_t max; //max size of list array
  struct MENU_ITEM *list[MENU_MAX_SIZE];
  union VAR_UNION var_buffer;
  bool visible, clear; //render menu, clear lcd
  bool update; //menu changed, required redraw
  uint8_t navigation; //menu nativation 0-no change 1-up 2-down -3 back -4 enter
  uint8_t selected_menu_head, selected_menu_tail; //creates menu selection to browse through
  uint8_t nav_position; //selection postion
  uint8_t nav_parent_id, nav_change_id; //id for matching items to display
  int8_t var_edit_position; //used as 10^var_edit_position when editing a variable.
  uint8_t lcd_columns, lcd_rows; //lcd size
  struct TIMER_SIMPLE debounce; //button input debounce
};

//use in setup / run once

//initilizes screen and creates custom menu chars
void menu_lcd_init(LiquidCrystal_I2C *_lcd);
//initilize the menu system object, uses defined LCD size and array size
void init_menu(struct MENU_SYSTEM *_menu);
//add menu item to menu_system list, this function must be called for each item to be added. 
//items are displayed in the order added. 
void add_menu_item(struct MENU_ITEM *_menu_item, struct MENU_ITEM *_parent_item, char *_label, void *_var_pointer, uint8_t _var_type, struct MENU_SYSTEM *_menu, bool _set_editable);

//use in main loop

//navigate through the menu. debouce input with ms, dir = 0 - none, 1 - up, 2 - down, 3 - enter, 4 - back
void menu_control(struct MENU_SYSTEM *_menu, uint32_t _debounce, uint8_t _dir);
//redraws the screen with the latest values.
void menu_draw(struct MENU_SYSTEM *_menu, LiquidCrystal_I2C *_lcd);

//internally used

//calculates exponential, used when editing values
int32_t math_exp(int base, unsigned int exp);
//menu function for editing a variables value
void menu_edit_var(struct MENU_SYSTEM *_menu, int8_t _dir);
//updates the menu list selection
void menu_update(struct MENU_SYSTEM *_menu);
//checks if the timer has elpased or not
void check_timer(struct TIMER_SIMPLE *_timer);
//start soft timer with set delay time.
void start_timer(struct TIMER_SIMPLE *_timer, uint32_t delay_time);



