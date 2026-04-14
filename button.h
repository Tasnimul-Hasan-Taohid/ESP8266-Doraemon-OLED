#pragma once
#include <Arduino.h>
#include "config.h"

class Button {
public:
  Button();
  void  begin();
  void  update();
  bool  wasPressed();
  bool  wasHeld();

private:
  bool          _last, _current;
  bool          _pressFlag, _holdFlag, _holdFired;
  unsigned long _debounceTime, _pressTime;
};
