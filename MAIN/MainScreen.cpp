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
  hal->clearScreen(WHITE); 
  hal->setTextColor(WHITE,BLACK);
  hal->setTextSize(1);
  hal->setCursor(0,0);


  hal->println(F("Строка1"));
  hal->println(F("Вторая"));
  hal->println(F("И ещё"));

/*
  hal->println(F("АБВГДЕЖЗИЙКЛМНОП"));
  hal->println(F("РСТУФХЦЧШЩЪЫЬЭЮЯ"));
  hal->println(F("абвгдежзийклмноп"));
  hal->println(F("рстуфхцчшщъыьэюя"));
  hal->println(F("Ёё123ABCabc!@#\xBC\xBD"));
  hal->println(F("10\x83 10\x8A\x82 10\x81\x80 2\x85"));
*/
  hal->display();  
  
  /*
  nokia->setFont(SmallFont);

  uint8_t fontHeight = FontUtils::getFontHeight(SmallFont);
  
  String adcString = F("ADC: ");
  adcString += adcValue;

  nokia->print(adcString.c_str(),0,fontHeight + 2); 
  */
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawTemperature(HalDC* hal)
{
  /*
  HalDCDescriptor* nokia = hal->getDC();
  
  // отрисовка температуры
  nokia->setFont(SmallFont);


  // рисуем температуру
  String tempString = F("DS18B20: ");
  
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

    nokia->print(tempString.c_str(),0,0);  
  }
  */
  
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

