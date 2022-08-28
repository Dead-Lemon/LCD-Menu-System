## A, mostly, C based menu system for 16x2 style char LCD's
Currently depends on LiquidCrystal_I2C
Customizable for all screen sizes

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
init_menu(&system_menu);
  //add menu( menu item, parent menu item, label, linked variable, variable type, menu system object, enable Edit )
  //menu items must be added in sequetial groups. using the variable type E_RETURN functions as a back button, E_NULL does nothing,
  //root list
  add_menu_item(&M_overview, nullptr, "Overview", nullptr, E_RETURN, &system_menu, false);
  add_menu_item(&M_settings, nullptr, "Settings", nullptr, E_NULL, &system_menu, false);
  add_menu_item(&M_about, nullptr, "About", &about_txt, E_CHAR, &system_menu, true);
  add_menu_item(&M_test, nullptr, "Test Vars", nullptr, E_NULL, &system_menu, false);
  //sublist, links to menu_settings
  add_menu_item(&M_back1, &M_settings, "BACK", nullptr, E_RETURN, &system_menu, false);
  add_menu_item(&M_TempS1, &M_settings, "Temp 1", &TempS1_PV, E_FLOAT, &system_menu, false);
  add_menu_item(&M_TempS2, &M_settings, "Temp 2", &TempS2_PV, E_FLOAT, &system_menu, false);
  add_menu_item(&M_HumS1, &M_settings, "Humidity", &HumS1_PV, E_FLOAT, &system_menu, false);
  add_menu_item(&M_FanC1, &M_settings, "Fan Auto", &fan_mode, E_BOOL, &system_menu, true);
  add_menu_item(&M_FanC1_Rng, &M_settings, "Fan State", &fan_running, E_BOOL, &system_menu, false);

  //sublist, links to temp sensor 1
  add_menu_item(&M_back2, &M_TempS1, "BACK", nullptr, E_RETURN, &system_menu, false);
  add_menu_item(&M_TempS1_max, &M_TempS1, "Max Temp", &TempS1_SP_max, E_FLOAT, &system_menu, true);
  add_menu_item(&M_TempS1_min, &M_TempS1, "Min Temp", &TempS1_SP_min, E_FLOAT, &system_menu, true);

  add_menu_item(&M_back3, &M_TempS2, "BACK", nullptr, E_RETURN, &system_menu, false);
  add_menu_item(&M_TempS2_max, &M_TempS2, "Max Temp", &TempS2_SP_max, E_FLOAT, &system_menu, true);
  add_menu_item(&M_TempS2_min, &M_TempS2, "Min Temp", &TempS2_SP_min, E_FLOAT, &system_menu, true);

  //sublist, links to test values
  add_menu_item(&M_back4, &M_test, "BACK", nullptr, E_RETURN, &system_menu, false);
  add_menu_item(&M_Test_float, &M_test, "float", &test_float, E_FLOAT, &system_menu, true);
  add_menu_item(&M_Test_int8, &M_test, "int8", &test_int8, E_INT8, &system_menu, true);
  add_menu_item(&M_Test_int16, &M_test, "int16", &test_int16, E_INT16, &system_menu, true);
  add_menu_item(&M_Test_int32, &M_test, "int32", &test_int32, E_INT32, &system_menu, true);
  add_menu_item(&M_back4, &M_test, "BACK", nullptr, E_RETURN, &system_menu, false);

//draw the latest view
menu_draw(&system_menu, &lcd);

//basic 4 button controll, with 200ms debounce
  if(digitalRead(13)) {menu_control(&system_menu, 200, E_DOWN);}
  if(digitalRead(12)) {menu_control(&system_menu, 200, E_UP);}
  if(digitalRead(10)) {menu_control(&system_menu, 200, E_ENTER);}
  if(digitalRead(11)) {menu_control(&system_menu, 200, E_BACK);}
  ```
