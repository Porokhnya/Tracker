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
  lastLogActiveFlag = Settings.isLoggingEnabled();
  
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

  bool curLogActive = Settings.isLoggingEnabled();
  bool wantDrawLogState = false;
  if(curLogActive != lastLogActiveFlag)
  {
    lastLogActiveFlag = curLogActive;
    wantDrawLogState = true;
  }

  if(wantDrawTemp || wantDrawADC || wantDrawTime || wantDrawLogState)
  {
    hal->clearScreen();
    
    drawTemperature(hal);
    drawADC(hal);
    drawTime(hal);
    drawLogState(hal);
    
    hal->updateDisplay();
  }  
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawADC(HalDC* hal)
{  
  // отрисовка показаний ADC
  /*
  hal->setFont(SCREEN_SMALL_FONT);
  hal->setColor(SCREEN_TEXT_COLOR);

  uint8_t fontHeight = hal->getFontHeight(SCREEN_SMALL_FONT);
  
  String adcString = F("Темп2: ");
  adcString += adcValue;
  hal->print(adcString.c_str(),0,fontHeight*2 + 2*2);
  */
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawLogState(HalDC* hal)
{
  String logCaption = "0";
  if(lastLogActiveFlag)
  {
    logCaption = Settings.getLoggingInterval();
  }

  hal->setFont(SCREEN_SMALL_FONT);
  hal->setColor(SCREEN_TEXT_COLOR);

  uint8_t fontHeight = hal->getFontHeight(SCREEN_SMALL_FONT);
  uint8_t fontWidth = hal->getFontHeight(SCREEN_SMALL_FONT);

  uint16_t screenWidth = hal->getScreenWidth();
  uint16_t screenHeight = hal->getScreenHeight();

  uint16_t captionWidth = fontWidth*logCaption.length();
  uint16_t drawX = screenWidth - captionWidth - 2;
  uint16_t drawY = screenHeight - fontHeight - 2;


  hal->print(logCaption.c_str(),drawX, drawY);

  int add = 2;
  if(logCaption.length() < 2)
    add = 0;
    
  hal->drawRoundRect(drawX - 2, drawY - 2, drawX + captionWidth - add, drawY + fontHeight);

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawTemperature(HalDC* hal)
{ 
  // отрисовка температуры/влажности
  String displayString;

  uint16_t screenWidth = hal->getScreenWidth();
  uint16_t screenHeight = hal->getScreenHeight();   

  hal->setFont(MediumNumbers);
  hal->setColor(SCREEN_TEXT_COLOR); 

  uint8_t fontHeight = hal->getFontHeight(MediumNumbers);
  uint8_t fontWidth = hal->getFontWidth(MediumNumbers);

  if(lastSensorData.temperature == NO_TEMPERATURE_DATA) // нет температуры
  {
    displayString = "";
  }
  else // есть температура
  {   
    displayString += lastSensorData.temperature;
    displayString += DECIMAL_SEPARATOR;

    if(lastSensorData.temperatureFract < 10)
      displayString += '0';

    displayString += lastSensorData.temperatureFract;
    displayString += ';'; // градус
  }


  uint16_t stringWidth = displayString.length()*fontWidth;

  uint16_t drawX = (screenWidth - stringWidth)/2;
  uint16_t drawY = 0;

  hal->print(displayString.c_str(), drawX, drawY);



  String percents;
  bool hasHumidity = false;
  if(lastSensorData.humidity == NO_TEMPERATURE_DATA) // нет влажности
  {
    displayString = "";
  }
  else // есть влажность
  {   
    hasHumidity = true;
    displayString = lastSensorData.humidity;
    displayString += DECIMAL_SEPARATOR;

    if(lastSensorData.humidityFract < 10)
      displayString += '0';

    displayString += lastSensorData.humidityFract;
    percents = '%';
  }

  drawY += fontHeight;
  stringWidth = displayString.length()*fontWidth;
  drawX = (screenWidth - stringWidth - ( hasHumidity ? fontWidth : 0 ))/2;
  hal->print(displayString.c_str(), drawX, drawY);

  drawX += stringWidth;
  hal->setFont(SCREEN_SMALL_FONT);
  hal->print(percents.c_str(), drawX, drawY);

  
   
/*  
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
    tempString += DECIMAL_SEPARATOR;

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
    tempString += DECIMAL_SEPARATOR;

    if(lastSensorData.humidityFract < 10)
      tempString += '0';

    tempString += lastSensorData.humidityFract;
    tempString += '%';
  }
  hal->print(tempString.c_str(), 0, fontHeight + 2);
*/
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawTime(HalDC* hal)
{
	DS3231Time tm = RealtimeClock.getTime();
	
		hal->setFont(SCREEN_SMALL_FONT);
		hal->setColor(SCREEN_TEXT_COLOR);

    uint8_t fontHeight = hal->getFontHeight(SCREEN_SMALL_FONT);
    uint16_t screenWidth = hal->getScreenWidth();
    uint16_t screenHeight = hal->getScreenHeight();  

		// получаем компоненты даты в виде строк
		//String strDate = RealtimeClock.getDateStr(tm);
		//String strTime = RealtimeClock.getTimeStr(tm);

    String strDate;

/*
    if(tm.dayOfMonth < 10)
      strDate += '0';
    strDate += tm.dayOfMonth;
    strDate += '.';
  
   if(tm.month < 10)
      strDate += '0';
    strDate += tm.month;
    strDate += ' ';  
*/  
    if(tm.hour < 10)
      strDate += '0';
    strDate += tm.hour;
    strDate += ':';   
  
    if(tm.minute < 10)
      strDate += '0';
    strDate += tm.minute;
      
		// печатаем их
    int drawX = 0;
    int drawY = screenHeight - fontHeight;
    
		hal->print(strDate.c_str(), drawX, drawY);
		//hal->print(strTime.c_str(), 0, fontHeight * 4 + 2*4);
	
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
   drawLogState(hal);

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
      case BUTTON_1: // по нажатию кнопки 1 переключаемся на первый экран настроек
        hal->switchToScreen("MenuScreen1");
      break;
      
  	  case BUTTON_2: // по нажатию кнопки 2 переключаемся на первый экран настроек
  		  hal->switchToScreen("MenuScreen2");
  		break;
      
  	  case BUTTON_3: // по нажатию кнопки 3 переключаемся на первый экран настроек
  		  hal->switchToScreen("MenuScreen3");
  		break;
      
  	  case BUTTON_4: // по нажатию кнопки 4 переключаемся на первый экран настроек
  	    hal->switchToScreen("MenuScreen4");
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
  for(uint8_t i=0;i<LOGGING_INTERVALS_COUNT;i++)
    intervalsRingBuffer.push_back(i);
  
  
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
  drawGUI(hal); 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen1::drawGUI(HalDC* hal)
{
  hal->clearScreen();
  
  hal->setFont(SCREEN_SMALL_FONT);
  hal->setColor(SCREEN_TEXT_COLOR);

  uint8_t fontHeightSmall = hal->getFontHeight(SCREEN_SMALL_FONT);
  uint8_t fontWidthSmall = hal->getFontWidth(SCREEN_SMALL_FONT);

  int drawX = 0, drawY = 0;

  uint16_t screenWidth = hal->getScreenWidth();
  uint16_t screenHeight = hal->getScreenHeight();

  hal->print("Интервал, мин:", drawX, drawY);

  drawY += fontHeightSmall + 8;

  hal->setFont(MediumNumbers);
  uint8_t fontHeightBig = hal->getFontHeight(MediumNumbers);
  uint8_t fontWidthBig = hal->getFontWidth(MediumNumbers);

  // получаем текущий интервал
  String strInterval = String(Settings.getLoggingInterval());

  // вычисляем, с какой позиции выводить интервал, чтобы вывести его по центру экрана
  drawX = (screenWidth - (strInterval.length()*fontWidthBig))/2;
  hal->print(strInterval.c_str(), drawX, drawY);

  drawY = screenHeight - fontHeightSmall;

  // рисуем статус логгирования
  hal->setFont(SCREEN_SMALL_FONT);

  String logCaption;

  if(Settings.isLoggingEnabled())
  {
    // логгирование активно
    logCaption = "Лог активен";    
  }
  else
  {
    // логгирование неактивно
    logCaption = "Лог неактивен";
  }

  int strLen = hal->print(logCaption.c_str(),0,0,0,true);
  drawX = (screenWidth - strLen*fontWidthSmall)/2;

  hal->print(logCaption.c_str(), drawX, drawY);

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
      case BUTTON_1: // по нажатию кнопки 1 выбираем параметры старта записи вверх
      {
        Link<uint8_t>* leaf = intervalsRingBuffer.head();
        uint8_t idx = Settings.getLoggingIntervalIndex();
        
        while(leaf->data != idx)
          leaf = leaf->next;

        leaf = leaf->next;
        
        Settings.setLoggingIntervalIndex(leaf->data);
        drawGUI(hal);
      } 
      break;
      
	    case BUTTON_2: // по нажатию кнопки 2 выбираем параметры старта записи вниз
      {
        Link<uint8_t>* leaf = intervalsRingBuffer.tail();
        uint8_t idx = Settings.getLoggingIntervalIndex();
        
        while(leaf->data != idx)
          leaf = leaf->prev;

        leaf = leaf->prev;
        
        Settings.setLoggingIntervalIndex(leaf->data);
        drawGUI(hal);      
      }
		  break;
      
	    case BUTTON_3: // по нажатию кнопки 3 вкл/выкл режим логгирования
      {
        Settings.switchLogging(!Settings.isLoggingEnabled());
        drawGUI(hal);
      }
		  break;
      
	    case BUTTON_4: // по нажатию кнопки 4 переключаемся на главный экран обратно без ввода команды (типа передумали)
		    hal->switchToScreen("Main");
		  break;
  }
  
  #else
    #error "Unsupported display!"  
  #endif
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MenuScreen2
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MenuScreen2::MenuScreen2() : AbstractHALScreen("MenuScreen2")
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen2::onDeactivate()
{
	// станем неактивными

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen2::onActivate()
{
	// мы активизируемся

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen2::doSetup(HalDC* hal)
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
void MenuScreen2::doUpdate(HalDC* hal)
{
	if (!isActive())
		return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen2::doDraw(HalDC* hal)
{

	hal->setFont(SCREEN_SMALL_FONT);
	hal->setColor(SCREEN_TEXT_COLOR);

	hal->print("Экран #2", 0, 0);

	hal->updateDisplay();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen2::onButtonPressed(HalDC* hal, int pressedButton)
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
	switch (pressedButton)
	{
  	case BUTTON_1: // по нажатию кнопки 1 выбираем параметры старта записи вверх
  			//hal->switchToScreen("Main");
  	break;
      
  	case BUTTON_2: // по нажатию кнопки 2 выбираем параметры старта записи вниз
  			//hal->switchToScreen("Main");
  	break;
      
  	case BUTTON_3: // по нажатию кнопки 3 запускаем процесс, переходим на главный экран
  
  			// Здесь вводим процедуру старта записи на SD
  
  		hal->switchToScreen("Main");  // переключаемся на главный экран обратно после старта процедуры
  	break;
    
  	case BUTTON_4: // по нажатию кнопки 4 переключаемся на главный экран обратно без ввода команды (типа передумали)
  		hal->switchToScreen("Main");
  	break;
	}

#else
#error "Unsupported display!"  
#endif

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MenuScreen3
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MenuScreen3::MenuScreen3() : AbstractHALScreen("MenuScreen3")
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen3::onDeactivate()
{
	// станем неактивными

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen3::onActivate()
{
	// мы активизируемся

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen3::doSetup(HalDC* hal)
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
void MenuScreen3::doUpdate(HalDC* hal)
{
	if (!isActive())
		return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen3::doDraw(HalDC* hal)
{

	hal->setFont(SCREEN_SMALL_FONT);
	hal->setColor(SCREEN_TEXT_COLOR);

	hal->print("Экран #3", 0, 0);

	hal->updateDisplay();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen3::onButtonPressed(HalDC* hal, int pressedButton)
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
	switch (pressedButton)
	{
  	case BUTTON_1: // по нажатию кнопки 1 выбираем параметры старта записи вверх
  			//hal->switchToScreen("Main");
  	break;
      
  	case BUTTON_2: // по нажатию кнопки 2 выбираем параметры старта записи вниз
  			//hal->switchToScreen("Main");
  	break;
    
  	case BUTTON_3: // по нажатию кнопки 3 запускаем процесс, переходим на главный экран
  
  			// Здесь вводим процедуру старта записи на SD
  
  		hal->switchToScreen("Main");  // переключаемся на главный экран обратно после старта процедуры
  	break;
    
  	case BUTTON_4: // по нажатию кнопки 4 переключаемся на главный экран обратно без ввода команды (типа передумали)
  		hal->switchToScreen("Main");
  	break;
	}

#else
#error "Unsupported display!"  
#endif

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MenuScreen4
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MenuScreen4::MenuScreen4() : AbstractHALScreen("MenuScreen4")
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen4::onDeactivate()
{
	// станем неактивными

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen4::onActivate()
{
	// мы активизируемся

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen4::doSetup(HalDC* hal)
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
void MenuScreen4::doUpdate(HalDC* hal)
{
	if (!isActive())
		return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen4::doDraw(HalDC* hal)
{

	hal->setFont(SCREEN_SMALL_FONT);
	hal->setColor(SCREEN_TEXT_COLOR);

	hal->print("Экран #4", 0, 0);

	hal->updateDisplay();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen4::onButtonPressed(HalDC* hal, int pressedButton)
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
	switch (pressedButton)
	{
  	case BUTTON_1: // по нажатию кнопки 1 выбираем параметры старта записи вверх
  			//hal->switchToScreen("Main");
  	break;
      
  	case BUTTON_2: // по нажатию кнопки 2 выбираем параметры старта записи вниз
  			//hal->switchToScreen("Main");
  	break;
    
  	case BUTTON_3: // по нажатию кнопки 3 запускаем процесс, переходим на главный экран
  
  			// Здесь вводим процедуру старта записи на SD
  
  		hal->switchToScreen("Main");  // переключаемся на главный экран обратно после старта процедуры
  	break;
    
  	case BUTTON_4: // по нажатию кнопки 4 переключаемся на главный экран обратно без ввода команды (типа передумали)
  		hal->switchToScreen("Main");
  	break;
	}

#else
#error "Unsupported display!"  
#endif

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
