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

    Button redButton;
    Button blue1Button;
    Button blue2Button;
    Button yellowButton;

private:

    bool inited;
    
};
//--------------------------------------------------------------------------------------------------
extern ButtonsList Buttons;
//--------------------------------------------------------------------------------------------------

