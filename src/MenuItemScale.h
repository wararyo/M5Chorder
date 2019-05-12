#ifndef _MENUITEMSCALE_H_
#define _MENUITEMSCALE_H_

#include <MenuItem.h>
#include <Scale.h>

class MenuItemScale : public MenuItem {
public:
  int minimum = 0;
  int maximum = 1;
  int value = 0;

  MenuItemScale(const String& title, int value, int tg = 0, TCallBackEnter cb = 0)
  : MenuItem(title, tg, cb), value(value), maximum(Scale::getAvailableScales().size()) {};

  MenuItemScale(const String& title, int value, TCallBackEnter cb)
  : MenuItem(title, cb), value(value), maximum(Scale::getAvailableScales().size()) {};

  virtual void onAfterDraw();
  virtual void onEnter();
  
  void setValue(int value);
private:
  void drawNum(int value, int flg);
};

#endif