//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "MainScreen.h"
#include "DS3231.h"
#include "CONFIG.h"
#include "Settings.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MainScreen* mainScreen = NULL;        
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MainScreen::MainScreen() : AbstractHALScreen("Main")
{
  mainScreen = this;
  adcValue = 0;
  memset(&temp,0,sizeof(DS18B20Temperature));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::onDeactivate()
{
  // станем неактивными
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::onActivate()
{
  // мы активизируемся
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::doSetup(HalDC* hal)
{
  // первоначальная настройка экрана
  
  #if DISPLAY_USED == DISPLAY_ILI9341
  
  	screenButtons->addButton(5, 275, 190, 40, "ПРОБА");
  	// screenButtons->addButton(200, 275, 35, 40, "z", BUTTON_SYMBOL); // кнопка Часы 
    
  #elif DISPLAY_USED == DISPLAY_NOKIA5110
    //TODO: Тут дополнительная инициализация Nokia 5110, если надо
  #else
    #error "Unsupported display!"  
  #endif

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::doUpdate(HalDC* hal)
{
  if(!isActive())
    return;
    
	// обновление экрана
  static uint32_t tempUpdateTimer = 0;
  if(millis() - tempUpdateTimer > SENSORS_UPDATE_FREQUENCY)
  {
    tempUpdateTimer = millis();
    bool anyChanges = false;
    
    DS18B20Temperature thisTemp = Settings.getDS18B20Temperature();
    
    if(memcmp(&thisTemp,&temp,sizeof(DS18B20Temperature)))
    {
      memcpy(&temp,&thisTemp,sizeof(DS18B20Temperature));
      drawTemperature(hal);
      anyChanges = true;
    }

    uint16_t thisADCVal = Settings.getAnalogSensorValue();
    if(thisADCVal != adcValue)
    {
      adcValue = thisADCVal;
      drawADC(hal);
      anyChanges = true;
    }

    if(anyChanges)
    {
      hal->updateDisplay();
    }
  } // if(millis() - ....
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawADC(HalDC* hal)
{  
  // отрисовка показаний ADC
  
  hal->setFont(SCREEN_SMALL_FONT);
  hal->setColor(SCREEN_TEXT_COLOR);

  uint8_t fontHeight = hal->getFontHeight(SCREEN_SMALL_FONT);
  
  String adcString = F("АЦП: ");
  adcString += adcValue;

  hal->print(adcString.c_str(),0,fontHeight + 2);
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawTemperature(HalDC* hal)
{  
  // отрисовка температуры
  hal->setFont(SCREEN_SMALL_FONT);
  hal->setColor(SCREEN_TEXT_COLOR);

  // рисуем температуру
  String tempString = F("Температура: ");
  
  if(temp.Value == NO_TEMPERATURE_DATA) // нет температуры
  {
    tempString += F("<no data>");
  }
  else // есть температура
  {
    
    tempString += temp.Value;
    tempString += ".";

    if(temp.Fract < 10)
      tempString += '0';

    tempString += temp.Fract;

  }
  hal->print(tempString.c_str(), 0, 0);
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::doDraw(HalDC* hal)
{
  temp = Settings.getDS18B20Temperature();
  drawTemperature(hal);

   adcValue = Settings.getAnalogSensorValue();
   drawADC(hal);
   
   hal->updateDisplay();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::onButtonPressed(HalDC* hal, int pressedButton)
{
  // обработчик нажатия на кнопку. Номера кнопок начинаются с 0 и идут в том порядке, в котором мы их добавляли
	if (pressedButton == 0)
	{
		hal->switchToScreen("Settings"); // переключаемся на экран настроек
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

