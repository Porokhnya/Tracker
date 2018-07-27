//--------------------------------------------------------------------------------------------------
#include "Buttons.h"
#include "CONFIG.h"
#include "Settings.h"
#include "ScreenHAL.h"
//--------------------------------------------------------------------------------------------------
ButtonsList Buttons;
//--------------------------------------------------------------------------------------------------
ButtonsList::ButtonsList()
{
  inited = false;
  wantProcessKey = false;
}
//--------------------------------------------------------------------------------------------------
void ButtonsList::begin()
{
  // redButton.begin(BUTTON_RED);

  inited = true;
}
//--------------------------------------------------------------------------------------------------
void ButtonsList::onKeyPressed(int key)
{
  pressedKey = key;
  wantProcessKey = true;
}
//--------------------------------------------------------------------------------------------------
void ButtonsList::update()
{
	if (!inited)
		return;

   if(wantProcessKey)
   {
      wantProcessKey = false;

      if (pressedKey > 0)
      {
        if (pressedKey == BUTTON_7)
        {
          // выключаем подсветку
          Settings.displayBacklight(false);
  
        }
        else
        {
          // включаем подсветку
          Settings.displayBacklight(true);

          AbstractHALScreen* screen = Screen.getActiveScreen();
          if(screen)
          {
            screen->notifyButtonPressed(pressedKey);
          }
          
        }
        // Здесь ,возможно, нужно настроить обработку остальных кнопок
  
      }      
   } // if(wantProcessKey)


  /*
    if(redButton.isClicked())
    {
    DBGLN(F("RED BUTTON CLICKED!"));
    }
  */

}
//--------------------------------------------------------------------------------------------------

