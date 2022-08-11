## A C based menu system for 16x2 style char LCD's
Currently depends on LiquidCrystal_I2C
Customizable for all screen sizes

*Still need to seperate it into its own c library.*

Menu items will be displayed in the same order as they are added.
(You can play list groupings in any order)
use nullpntr if you have no input.

[working example](https://wokwi.com/projects/339710033303437907 "working example")


### Some basic usage :

```
#define MENU_MAX_SIZE 100 //max number of menu entries
#define LCD_COLUMNS 16
#define LCD_ROWS 2
#define LCD_I2C_ADDRESS 0x27

//create menu structures

//main management object for the menu
struct MENU_SYSTEM system_menu; 

//root
struct MENU_ITEM menu_overview, menu_settings, menu_about; 
//sub menu items
struct MENU_ITEM test, test1, test2, test3, test4, test5, test6, test7; 

//initilize lcd
menu_lcd_init(&lcd);
//initilize menu
init_menu(&system_menu, MENU_MAX_SIZE, 16 , 2);

  init_menu(&system_menu, MENU_MAX_SIZE, 16 , 2);
  //add menu( menu item, parent menu item, label, linked variable, variable type, menu system object, enable Edit )
  //menu items must be added in sequetial groups.
  //root list
  add_menu_item(&menu_overview, nullptr, "Overview", nullptr, E_RETURN, &system_menu, false);
  add_menu_item(&menu_settings, nullptr, "Settings", nullptr, E_NULL, &system_menu, false);
  add_menu_item(&menu_about, nullptr, "About", &about_txt, E_CHAR, &system_menu, true);
  add_menu_item(&test, nullptr, "Float", &f1, E_FLOAT, &system_menu, false);
  //sublist, links to menu_settings
  add_menu_item(&back1, &menu_settings, "BACK", nullptr, E_RETURN, &system_menu, false);
  add_menu_item(&test1, &menu_settings, "float11", &f1, E_FLOAT, &system_menu, true);
  //sublist, links to test
  add_menu_item(&back2, &test, "BACK", nullptr, E_RETURN, &system_menu, false);
  add_menu_item(&test4, &test, "uint1", &ui16, E_UINT16, &system_menu, true);
  add_menu_item(&test5, &test, "bloat menu", &ui16, E_UINT16, &system_menu, false);
  //sublist, links to sublist test5
  add_menu_item(&back3, &test5, "BACK", nullptr, E_RETURN, &system_menu, false);
  add_menu_item(&more_depth, &test5, "deep1", &cool, E_BOOL, &system_menu, true);
  add_menu_item(&more2, &test5, "deep2", &cool, E_BOOL, &system_menu, true);

//draw the latest view
menu_draw(&system_menu, &lcd);

//basic 4 button controll, with 200ms debounce
  if(digitalRead(13)) {menu_control(&system_menu, 200, E_DOWN);}
  if(digitalRead(12)) {menu_control(&system_menu, 200, E_UP);}
  if(digitalRead(10)) {menu_control(&system_menu, 200, E_ENTER);}
  if(digitalRead(11)) {menu_control(&system_menu, 200, E_BACK);}
  ```
