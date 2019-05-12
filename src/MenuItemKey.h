#ifndef _MENUITEMKEY_H_
#define _MENUITEMKEY_H_

#include <MenuItem.h>

class MenuItemKey : public MenuItem {
public:
  int minimum = 0;
  int maximum = 11;
  int value = 0;

  MenuItemKey(const String& title, int value, int tg = 0, TCallBackEnter cb = 0)
  : MenuItem(title, tg, cb), value(value) {};

  MenuItemKey(const String& title, int value, TCallBackEnter cb)
  : MenuItem(title, cb), value(value) {};

  virtual void onAfterDraw();
  virtual void onEnter();
  
  void setValue(int value);
private:
  void drawNum(int value, int flg);
};

#endif