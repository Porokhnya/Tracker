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
	int pressedKey;
	if (Settings.newPressedKey)           // Если нажата новая кнопка
	{
		pressedKey = Settings.getPressedKey();
		Settings.newPressedKey = false;     // Кнопка считана, сбросить признак нажатия новой кнопки
	
		if (pressedKey > 0)
		{
			if (pressedKey == 7)
			{
				// выключаем подсветку
				Settings.displayBacklight(false);

			}
			else
			{
				// включаем подсветку
				Settings.displayBacklight(true);
			}
			// Здесь ,возможно, нужно настроить обработку остальных кнопок

		}
	}
  /*
    if(redButton.isClicked())
    {
    DBGLN(F("RED BUTTON CLICKED!"));
    }
  */

}
//--------------------------------------------------------------------------------------------------

