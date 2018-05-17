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


#if DISPLAY_USED == DISPLAY_ILI9341

	screenButtons->addButton(5, 275, 190, 40, "hgfddfd");
	// screenButtons->addButton(200, 275, 35, 40, "z", BUTTON_SYMBOL); // кнопка Часы 
#else
  #error "Unsupported display!"  
#endif


  // первоначальная настройка экрана
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
    
    DS18B20Temperature thisTemp = Settings.getDS18B20Temperature();
    
    if(memcmp(&thisTemp,&temp,sizeof(DS18B20Temperature)))
    {
      memcpy(&temp,&thisTemp,sizeof(DS18B20Temperature));
      drawTemperature(hal);
    }

    uint16_t thisADCVal = Settings.getAnalogSensorValue();
    if(thisADCVal != adcValue)
    {
      adcValue = thisADCVal;
      drawADC(hal);
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
  
  String adcString = F("ADC: ");
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
  String tempString = F("Temp: ");
  
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

