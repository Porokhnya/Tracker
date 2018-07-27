#pragma once
//--------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include "CoreButton.h"
//--------------------------------------------------------------------------------------------------
class ButtonsList
{
  public:
    ButtonsList();
    void begin();
    void update();

//    Button redButton;

  void onKeyPressed(int key);

private:

    bool inited;
    int pressedKey;
    bool wantProcessKey;
    
};
//--------------------------------------------------------------------------------------------------
extern ButtonsList Buttons;
//--------------------------------------------------------------------------------------------------

