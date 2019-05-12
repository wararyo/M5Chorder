#ifndef _MENUITEMPITCH_H_
#define _MENUITEMPITCH_H_

#include <MenuItem.h>

class MenuItemPitch : public MenuItem {
public:
  int minimum = 0;
  int maximum = 11;
  int value = 0;

  MenuItemPitch(const String& title, int value, int tg = 0, TCallBackEnter cb = 0)
  : MenuItem(title, tg, cb), value(value) {};

  MenuItemPitch(const String& title, int value, TCallBackEnter cb)
  : MenuItem(title, cb), value(value) {};

  virtual void onAfterDraw();
  virtual void onEnter();
  
  void setValue(int value);
private:
  void drawNum(int value, int flg);
};

#endif