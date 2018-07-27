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
  memset(&lastSensorData,0,sizeof(lastSensorData));
  lastSensorData.temperature = NO_TEMPERATURE_DATA;
  lastSensorData.humidity = NO_TEMPERATURE_DATA;
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
   
   // выводим код нажатой клавиши (! отлажено, вывод номера кнопки на экран уже не нужен).
   //String pk = F("KEY: "); pk += Settings.getPressedKey();
   //uint8_t fontHeight = hal->getFontHeight(SCREEN_SMALL_FONT);
   //hal->print(pk.c_str(), 0, fontHeight*4 + 2*4);
   //hal->updateDisplay();
    
	// обновление экрана
  static uint32_t tempUpdateTimer = 0;
  bool wantDrawTemp = false, wantDrawADC = false, wantDrawTime = false;
    
  if(millis() - tempUpdateTimer > SENSORS_UPDATE_FREQUENCY)
  {
    tempUpdateTimer = millis();
    
    Si7021Data thisData = Settings.readSensor();
    
    if(memcmp(&thisData,&lastSensorData,sizeof(Si7021Data)))
    {
      memcpy(&lastSensorData,&thisData,sizeof(Si7021Data));      
      wantDrawTemp = true;
    }

    uint16_t thisADCVal = Settings.getAnalogSensorValue();
    if(thisADCVal != adcValue)
    {
      adcValue = thisADCVal;      
      wantDrawADC = true;
    }
	
  } // if(millis() - ....

  DS3231Time tm = RealtimeClock.getTime();
  if (oldsecond != tm.second)
  {
    oldsecond = tm.second;
    wantDrawTime = true;
  }  

  if(wantDrawTemp || wantDrawADC || wantDrawTime)
  {
    hal->clearScreen();
    
    drawTemperature(hal);
    drawADC(hal);
    drawTime(hal);
    
    hal->updateDisplay();
  }  
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawADC(HalDC* hal)
{  
  // отрисовка показаний ADC
  
  hal->setFont(SCREEN_SMALL_FONT);
  hal->setColor(SCREEN_TEXT_COLOR);

  uint8_t fontHeight = hal->getFontHeight(SCREEN_SMALL_FONT);
  
  String adcString = F("Темп2: ");
  adcString += adcValue;
  hal->print(adcString.c_str(),0,fontHeight*2 + 2*2);
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawTemperature(HalDC* hal)
{  
  // отрисовка температуры
  hal->setFont(SCREEN_SMALL_FONT);
  hal->setColor(SCREEN_TEXT_COLOR);

  // рисуем температуру
  String tempString = F("Темп1: ");
  uint8_t fontHeight = hal->getFontHeight(SCREEN_SMALL_FONT);
  
  
  if(lastSensorData.temperature == NO_TEMPERATURE_DATA) // нет температуры
  {
    tempString += F("<нет>");
  }
  else // есть температура
  {   
    tempString += lastSensorData.temperature;
    tempString += ".";

    if(lastSensorData.temperatureFract < 10)
      tempString += '0';

    tempString += lastSensorData.temperatureFract;
  }
  hal->print(tempString.c_str(), 0, 0);

  tempString = F("Влажн: ");

 if(lastSensorData.humidity == NO_TEMPERATURE_DATA) // нет влажности
  {
    tempString += F("<нет>");
  }
  else // есть влажность
  {   
    tempString += lastSensorData.humidity;
    tempString += ".";

    if(lastSensorData.humidityFract < 10)
      tempString += '0';

    tempString += lastSensorData.humidityFract;
    tempString += '%';
  }
  hal->print(tempString.c_str(), 0, fontHeight + 2);

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawTime(HalDC* hal)
{
	DS3231Time tm = RealtimeClock.getTime();
	
		hal->setFont(SCREEN_SMALL_FONT);
		hal->setColor(SCREEN_TEXT_COLOR);

		// получаем компоненты даты в виде строк
		String strDate = RealtimeClock.getDateStr(tm);
		String strTime = RealtimeClock.getTimeStr(tm);
		// печатаем их
		uint8_t fontHeight = hal->getFontHeight(SCREEN_SMALL_FONT);
		hal->print(strDate.c_str(), 0, fontHeight * 3 + 2 * 4);
		hal->print(strTime.c_str(), 0, fontHeight * 4 + 2*4);
	
		//SerialUSB.print(strDate);
		//SerialUSB.print(" : ");
		//SerialUSB.println(strTime);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::doDraw(HalDC* hal)
{

  hal->clearScreen();
  
  //temp = Settings.getDS18B20Temperature();  
   drawTemperature(hal);

   adcValue = Settings.getAnalogSensorValue();
   drawADC(hal);
   drawTime(hal);

   hal->updateDisplay();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::onButtonPressed(HalDC* hal, int pressedButton)
{
  // обрабатываем нажатия на кнопки

  #if DISPLAY_USED == DISPLAY_ILI9341

  // Для TFT-экрана кнопки начинаются с нуля
  /*
    if (pressedButton == 0)
    {
      hal->switchToScreen("MenuScreen1"); // переключаемся на экран настроек
    }   
   */

  #elif DISPLAY_USED == DISPLAY_NOKIA5110

  // Для Nokia кнопки идут с 1
  switch(pressedButton)
  {
      case 1: // по нажатию кнопки 1 переключаемся на первый экран настроек
        hal->switchToScreen("MenuScreen1");
      break;
  }
  
  #else
    #error "Unsupported display!"  
  #endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MenuScreen1
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MenuScreen1::MenuScreen1() : AbstractHALScreen("MenuScreen1")
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen1::onDeactivate()
{
  // станем неактивными
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen1::onActivate()
{
  // мы активизируемся
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen1::doSetup(HalDC* hal)
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
void MenuScreen1::doUpdate(HalDC* hal)
{
  if(!isActive())
    return;   
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen1::doDraw(HalDC* hal)
{

  hal->setFont(SCREEN_SMALL_FONT);
  hal->setColor(SCREEN_TEXT_COLOR);

  hal->print("Экран #1", 0, 0);    

  hal->updateDisplay();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen1::onButtonPressed(HalDC* hal, int pressedButton)
{
  // обрабатываем нажатия на кнопки

  #if DISPLAY_USED == DISPLAY_ILI9341

  // Для TFT-экрана кнопки начинаются с нуля
  /*
    if (pressedButton == 0)
    {
      hal->switchToScreen("Main"); // переключаемся на экран настроек
    }   
   */

  #elif DISPLAY_USED == DISPLAY_NOKIA5110

  // Для Nokia кнопки идут с 1
  switch(pressedButton)
  {
      case 1: // по нажатию кнопки 1 переключаемся на главный экран обратно
        hal->switchToScreen("Main");
      break;
  }
  
  #else
    #error "Unsupported display!"  
  #endif
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

