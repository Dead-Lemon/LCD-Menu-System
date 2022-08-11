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

//add menu item ( menu item, parent menu item, label, linked variable, variable type, menu system object, enable Edit )
add_menu_item(&menu_overview, nullptr, "Overview", nullptr, E_RETURN, &system_menu, false); 

//basic 4 button controll
  if(digitalRead(13)) {menu_control(&system_menu, 200, E_DOWN);}
  if(digitalRead(12)) {menu_control(&system_menu, 200, E_UP);}
  if(digitalRead(10)) {menu_control(&system_menu, 200, E_ENTER);}
  if(digitalRead(11)) {menu_control(&system_menu, 200, E_BACK);}
  
  ```
