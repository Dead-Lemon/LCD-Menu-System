/* https://wokwi.com/projects/339710033303437907 */

#include <LiquidCrystal_I2C.h>
#include <stdio.h>
#include "menu_system.h"

struct MENU_SYSTEM system_menu;

//create menu items
struct MENU_ITEM M_overview, M_settings, M_about, M_test; //root
struct MENU_ITEM M_TempS1, M_TempS2, M_HumS1, M_FanC1; //settings submenu 
struct MENU_ITEM M_TempS1_max, M_TempS1_min, M_TempS2_max, M_TempS2_min, M_FanC1_Auto, M_FanC1_Rng;
struct MENU_ITEM M_Test_int8, M_Test_int16, M_Test_uint16, M_Test_int32, M_Test_float, M_Test_bool; //test displau values
struct MENU_ITEM M_back1, M_back2, M_back3, M_back4; //back button menu item, for 3 or less, button input


float TempS1_PV, TempS1_SP_min, TempS1_SP_max;
float TempS2_PV, TempS2_SP_min, TempS2_SP_max;
float HumS1_PV;
bool fan_mode, fan_running;
float test_float = 321.22;
int8_t test_int8 = 0;
int16_t test_int16 = -78;
uint16_t test_uint16 = 0;
int32_t test_int32 = -11111;
bool cool = false;
char about_txt[20] = "Ver: 0.4.12";

LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);

void test_var_update() //silly function to simulate values
{
  TempS1_PV = (random(30, 90)/3.1);
  TempS2_PV = (random(30, 90)/2.9);
  HumS1_PV = (random(30, 300)/3.5);
  if (fan_mode) {fan_running = (TempS1_PV > TempS1_SP_max);}
  test_float += 1.2;
  test_int8 += 1;
  test_int16 += 1;
  test_uint16 += 1; 
  test_int32 += 333;
}


void setup() 
{
  Serial.begin(115200);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  pinMode(13, INPUT);

  menu_lcd_init(&lcd);

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


}

void loop() 
{
  if(digitalRead(13)) {menu_control(&system_menu, 200, E_DOWN);}
  if(digitalRead(12)) {menu_control(&system_menu, 200, E_UP);}
  if(digitalRead(10)) {menu_control(&system_menu, 200, E_ENTER);}
  if(digitalRead(11)) {menu_control(&system_menu, 200, E_BACK);}
  menu_draw(&system_menu, &lcd);
  if (!system_menu.visible)
    {
      lcd.setCursor(0,0);
      lcd.print("Overview is now displayed");
      lcd.setCursor(0,1);
      lcd.print("Press ENTER for menu");
      lcd.scrollDisplayLeft();
    }


test_var_update();
}

