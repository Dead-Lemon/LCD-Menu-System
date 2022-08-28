#include <stdio.h>
#include "Arduino.h"
#include "menu_system.h"

void check_timer(struct TIMER_SIMPLE *_timer)
{
  if ((_timer->delay + _timer->start_time) < millis()) {
    _timer->lapsed = true;
  } else {
    _timer->lapsed = false;
  }
}

void start_timer(struct TIMER_SIMPLE *_timer, uint32_t delay_time)
{
  _timer->delay = delay_time;
  _timer->start_time = millis();
  _timer->lapsed = false;
}


void init_menu(struct MENU_SYSTEM *_menu)
{
  _menu->head = 0;
  _menu->max = MENU_MAX_SIZE-1;
  _menu->visible = false;
  _menu->update = true; 
  _menu->navigation = 0;
  _menu->nav_position = 0;
  _menu->nav_parent_id = -2;
  _menu->nav_change_id = -1;
  _menu->lcd_columns = LCD_COLUMNS;
  _menu->lcd_rows = LCD_ROWS;
  _menu->debounce.delay = 100;
  _menu->debounce.start_time = 0;
  _menu->debounce.lapsed = true; 
}

void add_menu_item(struct MENU_ITEM *_menu_item, struct MENU_ITEM *_parent_item, char *_label, void *_var_pointer, uint8_t _var_type, struct MENU_SYSTEM *_menu, bool _set_editable)
{
  _menu->list[_menu->head] = _menu_item;
  _menu_item->id = _menu->head; //menu item id represents position in menu list
  _menu->head = _menu->head + 1;
  _menu_item->end = true;
  _menu_item->edit = _set_editable;
  _menu_item->show_edit = false;

  if (_parent_item == nullptr) {
    _menu_item->parent_id = -1; //give root id if no parent menu item is give
  } else {
    _menu_item->parent_id = _parent_item->id; //link sub menu to parent
    _parent_item->end = false; //remove end terminator from parent list.
  }
  
  _menu_item->label = _label;
  _menu_item->var_type = _var_type;
  switch (_var_type) 
  {
    case(0):
      _menu_item->var_pointer.U_BOOL = _var_pointer;
      break;
    case(1):
      _menu_item->var_pointer.U_INT8 = _var_pointer;
      break;
    case(2):
      _menu_item->var_pointer.U_INT16 = _var_pointer;
      break;
    case(3):
      _menu_item->var_pointer.U_INT32 = _var_pointer;
      break;
    case(4):
      _menu_item->var_pointer.U_UINT8 = _var_pointer;
      break;
    case(5):
      _menu_item->var_pointer.U_UINT16 = _var_pointer;
      break;
    case(6):
      _menu_item->var_pointer.U_UINT32 = _var_pointer;
      break;
    case(7):
      _menu_item->var_pointer.U_FLOAT = _var_pointer;
      break;
    case(8):
      _menu_item->var_pointer.U_CHAR = _var_pointer;
      break;
  }
}

void menu_lcd_init(LiquidCrystal_I2C *_lcd)
{
  _lcd->init();
  _lcd->createChar(E_DOWN_ARROW, menu_downArrow);
  _lcd->createChar(E_UP_ARROW, menu_upArrow);
  _lcd->createChar(E_CURSOR, menu_cursor);
  _lcd->createChar(E_TICKED, menu_ticked);
  _lcd->createChar(E_UNTICKED, menu_unticked);
  _lcd->createChar(E_ENTER_ARROW, menu_enter);
  _lcd->clear();
  _lcd->backlight();
}

void menu_update(struct MENU_SYSTEM *_menu)
{
  if (_menu->nav_parent_id != _menu->nav_change_id) { //check if menu ID selection has changed, generate new list. 
    _menu->nav_parent_id = _menu->nav_change_id;

    for(uint8_t i = 0; i <= _menu->head; i++) //search for 1st entry of id, save as tail
    {
      if (_menu->nav_parent_id == _menu->list[i]->parent_id) {
        _menu->selected_menu_tail = i; 
        _menu->nav_position = i; //puts cursor at top of selected list
        break;
      } 
    }

    for(uint8_t i = _menu->selected_menu_tail; i <= _menu->head; i++) //search for last entry of id, save as head
    { 
      if (_menu->nav_parent_id != _menu->list[i]->parent_id) {
        _menu->selected_menu_head = i-1;
        break;
      }
    }
  }
}


void menu_draw(struct MENU_SYSTEM *_menu, LiquidCrystal_I2C *_lcd)
{
  if (_menu->visible) { //if menu is set to visable, render
    uint8_t cursor_pos = 0;
    char buffer[LCD_COLUMNS];
    menu_update(_menu);
    if (_menu->update) {
      _lcd->clear(); //only clears LCD if a change in menu position ocurs, otherwise text is just overriden, reduces flickering
    }

    switch(_menu->list[_menu->nav_position]->show_edit)
    {
      case(false): //draw menu list
        _lcd->setCursor(0,0);
        _lcd->write(E_CURSOR);  
        for(uint8_t i = 0; i < _menu->lcd_rows; i++) //update LCD buffer with displayable menu items
        {
          if(((_menu->nav_position + i) <= _menu->selected_menu_head) && ((_menu->nav_position + i) >= _menu->selected_menu_tail)) //check if menu selection is within bounds of selection
          {
            snprintf(buffer, (_menu->lcd_columns/2)+2, "%s", _menu->list[i + _menu->nav_position]->label); //copy unknow string size into fixed sized string buffer
            _lcd->print(buffer);
            _lcd->setCursor((_menu->lcd_columns/2)+2, i);
            switch(_menu->list[_menu->nav_position + i]->var_type) 
            {
            case(E_FLOAT):
              _lcd->print(*_menu->list[_menu->nav_position + i]->var_pointer.U_FLOAT);
              break;
            case(E_INT8):
              _lcd->print(*_menu->list[_menu->nav_position + i]->var_pointer.U_INT8);
              break;
            case(E_INT16):
              _lcd->print(*_menu->list[_menu->nav_position + i]->var_pointer.U_INT16);
              break;
            case(E_INT32):
              _lcd->print(*_menu->list[_menu->nav_position + i]->var_pointer.U_INT32);
              break;
            case(E_UINT8):
              _lcd->print(*_menu->list[_menu->nav_position + i]->var_pointer.U_UINT8);
              break;
            case(E_UINT16):
              _lcd->print(*_menu->list[_menu->nav_position + i]->var_pointer.U_UINT16);
              break;
            case(E_UINT32):
              _lcd->print(*_menu->list[_menu->nav_position + i]->var_pointer.U_UINT32);
              break;
            case(E_BOOL):
              _lcd->setCursor((_menu->lcd_columns)-2,i);
              if((*_menu->list[_menu->nav_position + i]->var_pointer.U_BOOL)) {_lcd->write(E_TICKED);} else {_lcd->write(E_UNTICKED);}
              break;
            case(E_NULL):
              break;
            }
            _lcd->setCursor(0,i+1); //set cursor to next row
          }
        }
        break;
      case(true): //displays edit screen of selected var
        _lcd->setCursor(0,0);
        _lcd->print(_menu->list[_menu->nav_position]->label);
        _lcd->setCursor(0,1);
        switch(_menu->list[_menu->nav_position]->var_type) 
          {
            case(E_FLOAT):
              _lcd->print(_menu->var_buffer.U_FLOAT);
              break;
            case(E_INT8):
              _lcd->print(_menu->var_buffer.U_INT8);
              break;
            case(E_INT16):
              _lcd->print(_menu->var_buffer.U_INT16);
              break;
            case(E_INT32):
              _lcd->print(_menu->var_buffer.U_INT32);
              break;
            case(E_UINT8):
              _lcd->print(_menu->var_buffer.U_UINT8);
              break;
            case(E_UINT16):
              _lcd->print(_menu->var_buffer.U_UINT16);
              break;
            case(E_UINT32):
              _lcd->print(_menu->var_buffer.U_UINT32);
              break;
            case(E_BOOL):
              _menu->var_edit_position = 0;
              if(_menu->var_buffer.U_BOOL) {
                _lcd->print("ON");
                _lcd->setCursor(4,1);
                _lcd->write(E_TICKED);
              } else {
                _lcd->print("OFF");
                _lcd->setCursor(4,1);
                _lcd->write(E_UNTICKED);
              }
              break;
            case(E_CHAR):
              _menu->var_edit_position = 0;
              _lcd->print(_menu->list[_menu->nav_position]->var_pointer.U_CHAR);
              break;
            case(E_NULL):
              break;
          }
        if (_menu->list[_menu->nav_position]->var_type != E_CHAR) //disable if char is being printed
        {
          _lcd->print(" =x");
        }
        if (_menu->list[_menu->nav_position]->var_type == E_FLOAT) 
        { 
          _lcd->print(0.1 * math_exp(10, _menu->var_edit_position));  //display incriment about for a float
        }
        else if (_menu->list[_menu->nav_position]->var_type != E_CHAR) //hides incriments if a char is to be displayed
        { 
          _lcd->print(1 * math_exp(10, _menu->var_edit_position)); //displays incriment amount 1000, 100, 10 & 1
        }
        if ((_menu->var_edit_position == 0)) 
        {
          _lcd->write(E_ENTER_ARROW);} else {_lcd->write(126); //draw next arrow or return symbol
        }
        break;
      }
    _menu->update = false;
  } 
  else if (_menu->clear)
  {
    _lcd->clear(); //clear lcd once
    _menu->clear = false;
  }
}

int32_t math_exp(int base, unsigned int exp) 
{
    int32_t result = 1;
    for (uint8_t i = 0; i < exp; i++)
        result *= base;
    return result;
 }

void menu_edit_var(struct MENU_SYSTEM *_menu, int8_t _dir)
{
  switch(_menu->list[_menu->nav_position]->var_type) 
    {
      case(E_FLOAT):
        _menu->var_buffer.U_FLOAT += ((_dir) * 0.1 * math_exp(10, _menu->var_edit_position));
        break;
      case(E_INT8):
        _menu->var_buffer.U_INT8 += (_dir * math_exp(10, _menu->var_edit_position));
        break;
      case(E_INT16):
        _menu->var_buffer.U_INT16 += (_dir * math_exp(10, _menu->var_edit_position));
        break;
      case(E_INT32):
        _menu->var_buffer.U_INT32 += (_dir * math_exp(10, _menu->var_edit_position));
        break;
      case(E_UINT8):
        _menu->var_buffer.U_UINT8 += (_dir * math_exp(10, _menu->var_edit_position));
        break;
      case(E_UINT16):
        _menu->var_buffer.U_UINT16 += (_dir * math_exp(10, _menu->var_edit_position));
        break;
      case(E_UINT32):
        _menu->var_buffer.U_UINT32 += (_dir * math_exp(10, _menu->var_edit_position));
        break;
      case(E_BOOL):
        _menu->var_buffer.U_BOOL = !_menu->var_buffer.U_BOOL;
    }
}

void menu_control(struct MENU_SYSTEM *_menu, uint32_t _debounce, uint8_t _dir)
{
  uint8_t dir = 0;
  check_timer(&_menu->debounce);
  if (_menu->debounce.lapsed) 
  {
    dir = _dir;
    _menu->update = true;
    start_timer(&_menu->debounce, _debounce);
  }
  if ((_menu->list[_menu->nav_position]->var_type == E_RETURN) && (dir == E_ENTER) && (_menu->visible)) { dir = E_BACK; }
  if (_menu->list[_menu->nav_position]->show_edit) //enables var edit mode
  {
    switch(dir)
    {
      case(E_NONE):
        break;
      case(E_DOWN):
        menu_edit_var(_menu, -1);
        break;
      case(E_UP):
        menu_edit_var(_menu, 1);
        break;
      case(E_ENTER):
        if (_menu->var_edit_position != 0) 
        {
          _menu->var_edit_position -= 1; 
        }
        else
        {
          *_menu->list[_menu->nav_position]->var_pointer.U_FLOAT = _menu->var_buffer.U_FLOAT; //save buffer 
          _menu->list[_menu->nav_position]->show_edit = false; //return to menu
        }
        break;
      case(E_BACK):
        _menu->list[_menu->nav_position]->show_edit = false; //discard buffer and return to menu
        break;
    }
  } 
  else 
  {
    switch(dir)
    {
      case(E_NONE):
        break;
      case(E_UP):
          _menu->nav_position -= 1;
        break;
      case(E_DOWN):
        _menu->nav_position += 1;
        break;
      case(E_ENTER): 
        if (!_menu->visible)
        { 
          _menu->visible = true;
        } 
        else if ((!_menu->list[_menu->nav_position]->end)) //check if there any items attached
        {
          _menu->nav_change_id = _menu->nav_position;
        }
        else if ((_menu->list[_menu->nav_position]->edit) && (!_menu->list[_menu->nav_position]->show_edit)) //sets up the variable edit display
        {
          _menu->list[_menu->nav_position]->show_edit = true;
          _menu->var_edit_position = 3;
          _menu->var_buffer.U_FLOAT = *_menu->list[_menu->nav_position]->var_pointer.U_FLOAT; //seems to be ok loading any value across
        }
        break;
      case(E_BACK):
        if(_menu->list[_menu->nav_position]->parent_id != 255) 
        {
            _menu->nav_change_id = _menu->list[_menu->list[_menu->nav_position]->parent_id]->parent_id; //some high concept array positioning, to go back, we need to look at the parent id, of the current items parent id. since the list form based on parent ids
        }
        else
        {
          if (_menu->visible) 
            {
              _menu->visible = false;
              _menu->clear = true;
            }
        }
        break;
    }
  }

  if((_menu->nav_position <= _menu->selected_menu_tail) || (_menu->nav_position == 255)) //reset position if it moves past selection
  {
    _menu->nav_position = _menu->selected_menu_tail;
  } 
  else if (_menu->nav_position > _menu->selected_menu_head) 
  {
    _menu->nav_position = _menu->selected_menu_head;
  }
}

