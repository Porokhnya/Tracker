//--------------------------------------------------------------------------------------------------
#include "Buttons.h"
#include "CONFIG.h"
#include "Settings.h"
//--------------------------------------------------------------------------------------------------
ButtonsList Buttons;
//--------------------------------------------------------------------------------------------------
ButtonsList::ButtonsList()
{
  inited = false;
}
//--------------------------------------------------------------------------------------------------
void ButtonsList::begin()
{
  // redButton.begin(BUTTON_RED);

  inited = true;
}
//--------------------------------------------------------------------------------------------------
void ButtonsList::update()
{
  if (!inited)
    return;
/*
  int pressedKey = Settings.getPressedKey();
  if (pressedKey > 0)
  {
    if (pressedKey == 4)
    {
      // выключаем подсветку
      Settings.displayBacklight(false);

    }
    else
    {
      // включаем подсветку
      Settings.displayBacklight(true);
    }
  }
*/
  /*
    if(redButton.isClicked())
    {
    DBGLN(F("RED BUTTON CLICKED!"));
    }
  */

}
//--------------------------------------------------------------------------------------------------

