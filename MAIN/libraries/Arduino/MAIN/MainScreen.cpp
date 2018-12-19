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
  ds18b20Value.value = NO_TEMPERATURE_DATA;
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
  lastDoorOpen = Settings.isDoorOpen();

  doorStateVisible = true;
  doorStateBlinkTimer = 0;
  blinkDoorState = false;
  
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
  bool wantDrawTemp = false, wantDrawDS18B20 = false, wantDrawTime = false, wantDrawDoorState = false;
    
  if(millis() - tempUpdateTimer > SENSORS_UPDATE_FREQUENCY)
  {
    tempUpdateTimer = millis();
    
    Si7021Data thisData = Settings.readSensor();
    
    if(memcmp(&thisData,&lastSensorData,sizeof(Si7021Data)))
    {
      memcpy(&lastSensorData,&thisData,sizeof(Si7021Data));      
      wantDrawTemp = true;
    }

    Temperature thisDS18B20Val = Settings.getDS18B20SensorTemperature();
    if(thisDS18B20Val.value != ds18b20Value.value || thisDS18B20Val.fract != ds18b20Value.fract)
    {
      ds18b20Value = thisDS18B20Val;      
      wantDrawDS18B20 = true;
    }
	
  } // if(millis() - ....

  DS3231Time tm = RealtimeClock.getTime();
  if (oldsecond != tm.second)
  {
    oldsecond = tm.second;
    wantDrawTime = true;
  }

  bool curDoorOpen = Settings.isDoorOpen();
  if(lastDoorOpen != curDoorOpen)
  {
    lastDoorOpen = curDoorOpen;
    wantDrawDoorState = true;
  }

  bool curLogActive = Settings.isLoggingEnabled();
  bool wantDrawLogState = false;
  if(curLogActive != lastLogActiveFlag)
  {
    lastLogActiveFlag = curLogActive;
    wantDrawLogState = true;
  }

  bool lastBlinkState = blinkDoorState;

  if(lastLogActiveFlag && lastDoorOpen) // открыто при включенном логгировании - надо мигать
  {
    if(!blinkDoorState)
    {
      blinkDoorState = true;
      doorStateBlinkTimer = millis();
      wantDrawDoorState = true;
      doorStateVisible = true;
    }
  }
  else
    blinkDoorState = false;


  if(blinkDoorState)
  {
    if(millis() - doorStateBlinkTimer > 750)
    {
      doorStateBlinkTimer = millis();
      doorStateVisible = !doorStateVisible;
      wantDrawDoorState = true;
    }
  }

  if(lastBlinkState != blinkDoorState)
  {
    doorStateVisible = true;
    wantDrawDoorState = true;
  }
  

  if(wantDrawTemp || wantDrawDS18B20 || wantDrawTime || wantDrawLogState || wantDrawDoorState)
  {
    hal->clearScreen();
    
    drawTemperature(hal);
    drawDS18B20(hal);
    drawTime(hal);
    drawLogState(hal);

    drawLogDuration(hal);

    drawDoorState(hal);
    
    hal->updateDisplay();
  }  
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawLogDuration(HalDC* hal)
{
    // рисуем продолжительность логгирования - в часах
    String durationCaption;
    uint32_t duration = Settings.getLoggingDuration();

    duration /= 60; // в минуты
    duration /= 60; // в часы
    
    durationCaption = duration;

    hal->setFont(SCREEN_SMALL_FONT);
    hal->setColor(SCREEN_TEXT_COLOR);
  
    uint8_t fontHeight = hal->getFontHeight(SCREEN_SMALL_FONT);
    uint8_t fontWidth = hal->getFontWidth(SCREEN_SMALL_FONT);
  
    uint16_t screenWidth = hal->getScreenWidth();
    uint16_t screenHeight = hal->getScreenHeight();
  
    uint16_t captionWidth = fontWidth*durationCaption.length();

    // рисуем слева от бокса интервала логгирования
    String logCaption = "0";
    if(lastLogActiveFlag)
    {
      logCaption = Settings.getLoggingInterval();
    }
    
    uint16_t drawX = screenWidth - captionWidth - 12 - logCaption.length()*fontWidth;
    uint16_t drawY = screenHeight - fontHeight - 2;
  
    hal->print(durationCaption.c_str(),drawX, drawY);
    
    hal->drawRoundRect(drawX - 2, drawY - 2, drawX + captionWidth + 2, drawY + fontHeight);

  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawDS18B20(HalDC* hal)
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
  uint8_t fontWidth = hal->getFontWidth(SCREEN_SMALL_FONT);

  uint16_t screenWidth = hal->getScreenWidth();
  uint16_t screenHeight = hal->getScreenHeight();

  uint16_t captionWidth = fontWidth*logCaption.length();
  uint16_t drawX = screenWidth - captionWidth - 4;
  uint16_t drawY = screenHeight - fontHeight - 2;


  hal->print(logCaption.c_str(),drawX, drawY);
    
  hal->drawRoundRect(drawX - 2, drawY - 2, drawX + captionWidth + 2, drawY + fontHeight);



}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawDoorState(HalDC* hal)
{
  String toDraw = " ";
  int addX = 0, addY = 0;
  if(lastDoorOpen) // дверь открыта
  {
    if(Settings.isLoggingEnabled())
    {
      toDraw = char(0xac); // открыто при включенной записи
    }
    else
    {
      toDraw = char(0xac); // открыто (закрашенное знакоместо)
    }
  }
  else
  {
    toDraw = 'X'; // закрыто (незакрашенный круг по центру)
  }

  hal->setFont(SCREEN_SMALL_FONT);
  hal->setColor(SCREEN_TEXT_COLOR);

  uint8_t fontHeight = hal->getFontHeight(SCREEN_SMALL_FONT);
  uint8_t fontWidth = hal->getFontWidth(SCREEN_SMALL_FONT);

  uint16_t screenWidth = hal->getScreenWidth();
  uint16_t screenHeight = hal->getScreenHeight();

  uint16_t captionWidth = fontWidth*toDraw.length();
  uint16_t drawX = screenWidth - captionWidth - 4;
  uint16_t drawY = screenHeight - fontHeight*2 - 2*4;

  if(!doorStateVisible)
    toDraw = " ";

  hal->print(toDraw.c_str(),drawX + addX, drawY + addY);
    
  if(lastDoorOpen)
  {
    // тут надо проверять - моргаем ли?
    if(doorStateVisible)
      hal->drawRoundRect(drawX - 2, drawY - 2, drawX + captionWidth + 2, drawY + fontHeight);  
    else
      hal->clrRoundRect(drawX - 2, drawY - 2, drawX + captionWidth + 2, drawY + fontHeight);  
  }
  else
    hal->drawRoundRect(drawX - 2, drawY - 2, drawX + captionWidth + 2, drawY + fontHeight);  
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
    /*
    displayString += DECIMAL_SEPARATOR;

    if(lastSensorData.temperatureFract < 10)
      displayString += '0';

    displayString += lastSensorData.temperatureFract;
    */
    displayString += ';'; // градус
  }

  uint16_t stringWidth = displayString.length()*fontWidth;

  uint16_t drawX = 0;//(screenWidth - stringWidth)/2;
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
    /*
    displayString += DECIMAL_SEPARATOR;

    if(lastSensorData.humidityFract < 10)
      displayString += '0';

    displayString += lastSensorData.humidityFract;
    */
    percents = '%';
  }

  drawY += fontHeight;
  stringWidth = displayString.length()*fontWidth;
  drawX = 0;//(screenWidth - stringWidth - ( hasHumidity ? fontWidth : 0 ))/2;
  hal->print(displayString.c_str(), drawX, drawY);

  drawX += stringWidth;
  hal->setFont(SCREEN_SMALL_FONT);
  hal->print(percents.c_str(), drawX, drawY);   
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
  
   drawTemperature(hal);

   ds18b20Value = Settings.getDS18B20SensorTemperature();
   drawDS18B20(hal);
   drawTime(hal);
   drawLogState(hal);
   drawLogDuration(hal);
   drawDoorState(hal);

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
  		  hal->switchToScreen("BordersScreen");
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
  /*
  for(uint8_t i=0;i<LOGGING_INTERVALS_COUNT;i++)
    intervalsRingBuffer.push_back(i);
  */
  
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
        if(!Settings.isLoggingEnabled()) // Запретить изменение интервала если лог включен
        {
          /*
          Link<uint8_t>* leaf = intervalsRingBuffer.head();
          uint8_t idx = Settings.getLoggingIntervalIndex();
          
          while(leaf->data != idx)
            leaf = leaf->next;
  
          leaf = leaf->next;
          Settings.setLoggingIntervalIndex(leaf->data);
          drawGUI(hal);
          */
          
          int16_t interval = Settings.getLoggingInterval();
          interval += LOGGING_INTERVAL_STEP;
          if(interval > LOGGING_MAX_INTERVAL)
            interval = LOGGING_MIN_INTERVAL;
          
          Settings.setLoggingInterval(interval);
          drawGUI(hal);
          
        }
      } 
      break;
      
	    case BUTTON_2: // по нажатию кнопки 2 выбираем параметры старта записи вниз
      {
        if(!Settings.isLoggingEnabled()) // Запретить изменение интервала если лог включен
        {
          /*
          Link<uint8_t>* leaf = intervalsRingBuffer.tail();
          uint8_t idx = Settings.getLoggingIntervalIndex();
          
          while(leaf->data != idx)
            leaf = leaf->prev;
  
          leaf = leaf->prev;
          
          Settings.setLoggingIntervalIndex(leaf->data);
          drawGUI(hal);
          */
          int16_t interval = Settings.getLoggingInterval();
          interval -= LOGGING_INTERVAL_STEP;
          if(interval < LOGGING_MIN_INTERVAL)
            interval = LOGGING_MAX_INTERVAL;
          
          Settings.setLoggingInterval(interval);
          drawGUI(hal);           
        }     
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
// ExportToSerialScreen
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ExportToSerialScreen* exportToSerialScreen = NULL;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ExportToSerialScreen::ExportToSerialScreen() : AbstractHALScreen("ExportToSerialScreen")
{
  exportToSerialScreen = this;
  selectedFile = NULL;
  isExportDone = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportToSerialScreen::startExport()
{
  isExportDone = false;
  
  if(!selectedFile)
  {
    isExportDone = true;
    return;
  }

  // тут вывод файла в Serial
  String fileName = LOGS_DIRECTORY;
  fileName += '/';
  fileName += selectedFile->getName(LOGS_DIRECTORY);
  FileUtils::SendToStream(&Serial,fileName);
  // вывод в Serial закончен



  // вызываем окошко c сообщением
  Vector<const char*> lines;
  lines.push_back("Экспорт файла");
  lines.push_back("завершен.");
  lines.push_back("");
  lines.push_back("Любая кнопка");
  lines.push_back("для выхода.");

  MessageBox->show(lines,"ExportLogsScreen");
 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportToSerialScreen::setFile(FileEntry* entry)
{
  selectedFile = entry;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportToSerialScreen::onDeactivate()
{
  // станем неактивными

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportToSerialScreen::onActivate()
{
  // мы активизируемся

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportToSerialScreen::doSetup(HalDC* hal)
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
void ExportToSerialScreen::doUpdate(HalDC* hal)
{
  if (!isActive())
    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportToSerialScreen::doDraw(HalDC* hal)
{

  hal->setFont(SCREEN_SMALL_FONT);
  hal->setColor(SCREEN_TEXT_COLOR);

  uint8_t fontHeight = hal->getFontHeight(SCREEN_SMALL_FONT);

  hal->print("Идёт экспорт", 0, 0);
  hal->print("файла, ждите...", 0, fontHeight + 2);

  hal->updateDisplay();

  startExport();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportToSerialScreen::onButtonPressed(HalDC* hal, int pressedButton)
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

/*
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
*/

#else
#error "Unsupported display!"  
#endif

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ExportLogsScreen
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ExportLogsScreen* exportLogsScreen = NULL;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ExportLogsScreen::ExportLogsScreen() : AbstractHALScreen("ExportLogsScreen")
{
  exportLogsScreen = this;
  totalFilesCount = 0;
  files = NULL;
  selectedFileNumber = 0;
  currentPage = 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::selectFile(HalDC* hal, int8_t step)
{
  if(!files || !totalFilesCount) // нечего рисовать
    return;
    
  // прибавляем смещение
  selectedFileNumber += step;

  // проверяем попадание в границы диапазона
  if(selectedFileNumber < 0)
    selectedFileNumber = 0;

  if(selectedFileNumber >= totalFilesCount)
    selectedFileNumber = totalFilesCount - 1;
  
  // рисуем файлы текущей страницы
  drawGUI(hal);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::drawFiles(HalDC* hal)
{
  if(!files || !totalFilesCount) // нечего рисовать
    return;


  int fontHeight = hal->getFontHeight(SCREEN_SMALL_FONT);
  int lineSpacing = 2;

   // вычисляем общее кол-во страниц
  int totalPages = totalFilesCount/SCREEN_FILES_COUNT;
  if(totalFilesCount % SCREEN_FILES_COUNT)
    totalPages++;

  // вычисляем текущую страницу, на которой находится выбранный файл
  currentPage = selectedFileNumber/SCREEN_FILES_COUNT;
        
  // выбираем диапазон, с которого нам выводить файлы
  int startIndex = currentPage*SCREEN_FILES_COUNT;
  int endIndex = startIndex + SCREEN_FILES_COUNT;

  if(endIndex > totalFilesCount)
    endIndex = totalFilesCount;
  
  // выводим файлы одной страницы, отмечая выбранный галочкой
  String lineToDraw;
  int drawX = 0;
  int drawY = fontHeight + lineSpacing;
  
  for(int i=startIndex;i<endIndex;i++)
  {
      FileEntry* entry = files[i];

      if(i == selectedFileNumber)
        lineToDraw = char(0x10); // толстая стрелка вправо
      else
        lineToDraw = ' ';

      lineToDraw += entry->getName(LOGS_DIRECTORY);

      hal->print(lineToDraw.c_str(), drawX, drawY);
      drawY += fontHeight + lineSpacing;
      
      /*
      filesNames[buttonCounter] = entry->getName(linkedDir);
      
      filesButtons->relabelButton(buttonCounter,filesNames[buttonCounter].c_str());
      filesButtons->showButton(buttonCounter,isActive());
      
      buttonCounter++;
      */
    } // for  
   
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::rescanFiles()
{
  if(!hasSD)
    return;

   int lastFilesCount = totalFilesCount;
   String dirName = LOGS_DIRECTORY;
   totalFilesCount = FileUtils::CountFiles(dirName);

   if(lastFilesCount != totalFilesCount)
   {
    clearFiles();

    files = new FileEntry*[totalFilesCount];

    SdFile file, root;
    
    for(int i=0;i<totalFilesCount;i++)
    {
      files[i] = new  FileEntry;
    } // for

    root.open(dirName.c_str(),O_READ);

    int cntr = 0;
    while (file.openNext(&root, O_READ)) 
    {
      if(cntr < totalFilesCount)
      {
        files[cntr]->dirIndex = file.dirIndex();
      }
      
      file.close();
      cntr++;
    }      
    root.close();

     
   } // if(lastFilesCount != totalFilesCount)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::clearFiles()
{
  if(!files)
    return;
    
  for(int i=0;i<totalFilesCount;i++)
    delete files[i];

  delete [] files;
  files = NULL;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::setMode(ExportMode mode)
{
  exportMode = mode;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::onDeactivate()
{
  // станем неактивными

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::onActivate()
{
  // мы активизируемся

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::doSetup(HalDC* hal)
{
  // первоначальная настройка экрана

  // добавляем экран экспорта в Serial
  hal->addScreen(ExportToSerialScreen::create());

  //TODO: ТУТ ДОБАВЛЯТЬ ЭКРАНЫ ЭКСПОРТА по WiFi И НА ПРИНТЕР!!!

  hasSD = SDInit::InitSD();

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
void ExportLogsScreen::doUpdate(HalDC* hal)
{
  if (!isActive())
    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::drawGUI(HalDC* hal)
{
  hal->clearScreen();
  
  hal->setFont(SCREEN_SMALL_FONT);
  hal->setColor(SCREEN_TEXT_COLOR);

  String header = "";
    
  switch(exportMode)
  {
    case exportToSerial:
      header += "В COM-порт";
    break;
    
    case exportToWiFi:
      header += "По WiFi";
    break;

    case exportToPrinter:
      header += "На принтер";
    break;
    
  }
  hal->print(header.c_str(), 0, 0);

  // тестовое кол-во файлов в папке логов
  //hal->print(String(totalFilesCount).c_str(),0,20);

  
  drawFiles(hal);

  hal->updateDisplay();  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::doDraw(HalDC* hal)
{
  drawGUI(hal);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::exportSelectedFile(HalDC* hal)
{
  if(!files || !totalFilesCount) // нечего выводить
    return;

  // получаем выбранный файл
  FileEntry* entry = files[selectedFileNumber];
    
  switch(exportMode)
  {
    case exportToSerial:
    {
      exportToSerialScreen->setFile(entry);
      hal->switchToScreen(exportToSerialScreen);
    }
    break;

    case exportToWiFi:
    {
      //TODO: Тут вызов экрана экспорта по WiFi !!!
    }
    break;
    
    case exportToPrinter:
    {
      //TODO: Тут вызов экрана экспорта на принтер !!!      
    }
    break;
    
  } // switch
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::onButtonPressed(HalDC* hal, int pressedButton)
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
    case BUTTON_1: // листаем файлы вниз
    {
      selectFile(hal, 1);
    }
    break;
      
    case BUTTON_2: // листаем файлы вверх
    {
      selectFile(hal, -1);
    }
    break;
    
    case BUTTON_3: // выводим файл на экспорт
    {
      exportSelectedFile(hal);
    }
    break;
    
    case BUTTON_4: // возврат на предыдущий экран - выбор, куда экспортируем
    {
      hal->switchToScreen("MenuScreen2");
    }
    break;

  }

#else
#error "Unsupported display!"  
#endif

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MenuScreen2
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MenuScreen2::MenuScreen2() : AbstractHALScreen("MenuScreen2")
{
  /*
  ignoreKeys = false;
  exportActive = false;
  drawMode = dmStartScreen;
  */
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

  // добавляем экран вывода списка файлов для экспорта
  hal->addScreen(ExportLogsScreen::create());

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
void MenuScreen2::drawStartScreen(HalDC* hal)
{
  uint8_t fontHeight = hal->getFontHeight(SCREEN_SMALL_FONT);

  int drawX = 0, drawY = 0;

  hal->print("Экспорт:", drawX, drawY);

  drawY += fontHeight + 2;  
  hal->print(" 1 - Serial", drawX, drawY);

  drawY += fontHeight + 2;  
  hal->print(" 2 - WiFi", drawX, drawY);

  drawY += fontHeight + 2;  
  hal->print(" 3 - Принтер", drawX, drawY);

  drawY += fontHeight + 2;  
  hal->print(" 4 - Выход", drawX, drawY);

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen2::drawGUI(HalDC* hal)
{
  hal->clearScreen();
  
  hal->setFont(SCREEN_SMALL_FONT);
  hal->setColor(SCREEN_TEXT_COLOR);


  drawStartScreen(hal);
   
  hal->updateDisplay();
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen2::doDraw(HalDC* hal)
{
  drawGUI(hal);
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
  	case BUTTON_1: // экспорт в Serial
	  {
      exportLogsScreen->setMode(exportToSerial);
      exportLogsScreen->rescanFiles();
      hal->switchToScreen(exportLogsScreen);
	  }
    break;
      
  	case BUTTON_2: // экспорт по WiFi
    {
      exportLogsScreen->setMode(exportToWiFi);
      exportLogsScreen->rescanFiles();
      hal->switchToScreen(exportLogsScreen);
      
    }
  	break;
      
  	case BUTTON_3: // экспорт на принтер  
    {
      exportLogsScreen->setMode(exportToPrinter);
      exportLogsScreen->rescanFiles();
      hal->switchToScreen(exportLogsScreen);
      
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
// EditBorderScreen
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
EditBorderScreen::EditBorderScreen(const char* screenName, WhichBorder b) : AbstractHALScreen(screenName)
{
  whichBorder = b;
  editedVal = 0;
  changeStep = 1;
  caption = "";
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::onDeactivate()
{
  // станем неактивными

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::onActivate()
{
  // мы активизируемся
  reloadValues();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::reloadValues()
{
  switch(whichBorder)
  {
    case temperatureBorder:
    {
      minVal = TEMPERATURE_MIN_BORDER;
      maxVal = TEMPERATURE_MAX_BORDER;
      changeStep = TEMPERATURE_BORDER_CHANGE_STEP;
      currentMinVal = Settings.getMinTempBorder();
      currentMaxVal = Settings.getMaxTempBorder();
      caption = "Порог темп-ры";
    }
    break;

    case humidityBorder:
    {
      minVal = HUMIDITY_MIN_BORDER;
      maxVal = HUMIDITY_MAX_BORDER;
      changeStep = HUMIDITY_BORDER_CHANGE_STEP;
      currentMinVal = Settings.getMinHumidityBorder();
      currentMaxVal = Settings.getMaxHumidityBorder();
      caption = "Порог влажн.";
    }
    break;

    case adcBorder:
    {
      minVal = ADC_MIN_BORDER;
      maxVal = ADC_MAX_BORDER;
      changeStep = ADC_BORDER_CHANGE_STEP;
      currentMinVal = Settings.getMinADCBorder();
      currentMaxVal = Settings.getMaxADCBorder();
      caption = "Порог АЦП";
    }
    break;
    
  }  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::doSetup(HalDC* hal)
{
  // первоначальная настройка экрана
  reloadValues();

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
void EditBorderScreen::saveValues()
{
 switch(whichBorder)
  {
    case temperatureBorder:
    {
      Settings.setMinTempBorder(currentMinVal);
      Settings.setMaxTempBorder(currentMaxVal);
    }
    break;

    case humidityBorder:
    {
      Settings.setMinHumidityBorder(currentMinVal);
      Settings.setMaxHumidityBorder(currentMaxVal);
    }
    break;

    case adcBorder:
    {
      Settings.setMinADCBorder(currentMinVal);
      Settings.setMaxADCBorder(currentMaxVal);
    }
    break;
    
  }  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::doUpdate(HalDC* hal)
{
  if (!isActive())
    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::drawGUI(HalDC* hal)
{
  hal->setFont(SCREEN_SMALL_FONT);
  hal->setColor(SCREEN_TEXT_COLOR);
  
  uint8_t fontHeight = hal->getFontHeight(SCREEN_SMALL_FONT);
  uint8_t fontWidth = hal->getFontWidth(SCREEN_SMALL_FONT);
  int screenWidth = hal->getScreenWidth();
  int screenHeight = hal->getScreenHeight();
  
  int drawX = 0, drawY = 0, spacing = 2;

  hal->print(caption, drawX, drawY);

  drawY += fontHeight*2 + spacing*4;
  drawX = spacing;

  const int MAX_VAL_LEN = 5;

  // считаем, что у нас на каждое значение - максимум 4 символа
  int boxContentWidth = MAX_VAL_LEN*fontWidth;

  // рисуем левый бокс с текущим значением
  String toDraw;
  
  toDraw = currentMinVal;
  while(toDraw.length() < MAX_VAL_LEN)
    toDraw += ' ';

  hal->print("min", drawX, drawY - fontHeight - spacing);
  hal->print(toDraw.c_str(),drawX, drawY);

  // если левый бокс - текущий, выделяем его
  if(editedVal == 0)
  {
    hal->drawRoundRect(drawX - spacing, drawY - spacing, drawX + boxContentWidth + spacing, drawY + fontHeight);
  }
  else
    hal->clrRoundRect(drawX - spacing, drawY - spacing, drawX + boxContentWidth + spacing, drawY + fontHeight);
  

  // теперь вычисляем позицию по X для правого значения
  drawX = screenWidth - boxContentWidth - spacing*2;

  // рисуем правый бокс
  toDraw = currentMaxVal;
  while(toDraw.length() < MAX_VAL_LEN)
    toDraw += ' ';

  hal->print("max", drawX, drawY - fontHeight - spacing);
  hal->print(toDraw.c_str(),drawX, drawY);

  // если правый бокс - текущий, то рисуем вокруг него рамку
  if(editedVal == 1)
  {
    hal->drawRoundRect(drawX - spacing, drawY - spacing, drawX + boxContentWidth + spacing, drawY + fontHeight);
  }
  else
    hal->clrRoundRect(drawX - spacing, drawY - spacing, drawX + boxContentWidth + spacing, drawY + fontHeight);


  hal->print("3 - выбор",0, screenHeight - fontHeight);

  hal->updateDisplay();
   
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::doDraw(HalDC* hal)
{
  drawGUI(hal);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::onButtonPressed(HalDC* hal, int pressedButton)
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
    case BUTTON_1: // текущее значение - вверх
    {
      if(editedVal == 0)
      {
        // редактируем минимальное значение
        currentMinVal += changeStep;        
      }
      else
      {
        // редактируем максимальное значение
        currentMaxVal += changeStep;        
      }

      if(currentMinVal > maxVal)
        currentMinVal = minVal;
      if(currentMinVal < minVal)
        currentMinVal = maxVal;

      if(currentMaxVal > maxVal)
        currentMaxVal = minVal;
      if(currentMaxVal < minVal)
        currentMaxVal = maxVal;      

      saveValues();

      drawGUI(hal);
    }
    break;
      
    case BUTTON_2: // текущее значение - вниз
    {
      if(editedVal == 0)
      {
        // редактируем минимальное значение
        currentMinVal -= changeStep;        
      }
      else
      {
        // редактируем максимальное значение
        currentMaxVal -= changeStep;
      }

      if(currentMinVal > maxVal)
        currentMinVal = minVal;
      if(currentMinVal < minVal)
        currentMinVal = maxVal;

      if(currentMaxVal > maxVal)
        currentMaxVal = minVal;
      if(currentMaxVal < minVal)
        currentMaxVal = maxVal;

      saveValues();
      
      drawGUI(hal);
    }
    break;
    
    case BUTTON_3:
    {
      if(editedVal == 0)
        editedVal = 1;
      else
        editedVal = 0;

      drawGUI(hal);
    }
    break;
    
    case BUTTON_4:
      hal->switchToScreen("BordersScreen");
    break;
  }

#else
#error "Unsupported display!"  
#endif

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// BordersScreen
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BordersScreen::BordersScreen() : AbstractHALScreen("BordersScreen")
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BordersScreen::onDeactivate()
{
	// станем неактивными

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BordersScreen::onActivate()
{
	// мы активизируемся

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BordersScreen::doSetup(HalDC* hal)
{
	// первоначальная настройка экрана

#if DISPLAY_USED == DISPLAY_ILI9341

	screenButtons->addButton(5, 275, 190, 40, "ПРОБА");
	// screenButtons->addButton(200, 275, 35, 40, "z", BUTTON_SYMBOL); // кнопка Часы 

#elif DISPLAY_USED == DISPLAY_NOKIA5110

	//TODO: Тут дополнительная инициализация Nokia 5110, если надо
  Screen.addScreen(EditBorderScreen::create("TemperatureBorderScreen",temperatureBorder));
  Screen.addScreen(EditBorderScreen::create("HumidityBorderScreen",humidityBorder));
  Screen.addScreen(EditBorderScreen::create("ADCBorderScreen",adcBorder));

#else
#error "Unsupported display!"  
#endif

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BordersScreen::doUpdate(HalDC* hal)
{
	if (!isActive())
		return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BordersScreen::doDraw(HalDC* hal)
{

	hal->setFont(SCREEN_SMALL_FONT);
	hal->setColor(SCREEN_TEXT_COLOR);
  uint8_t fontHeight = hal->getFontHeight(SCREEN_SMALL_FONT);

  int drawX = 0, drawY = 0;

  hal->print("Пороги:", drawX, drawY);

  drawY += fontHeight + 2;  
  hal->print(" 1 - Темп-тура", drawX, drawY);

  drawY += fontHeight + 2;  
  hal->print(" 2 - Влажность", drawX, drawY);

  drawY += fontHeight + 2;  
  hal->print(" 3 - АЦП", drawX, drawY);

  drawY += fontHeight + 2;  
  hal->print(" 4 - Выход", drawX, drawY);	


	hal->updateDisplay();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BordersScreen::onButtonPressed(HalDC* hal, int pressedButton)
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
  	case BUTTON_1: 
  			hal->switchToScreen("TemperatureBorderScreen");
  	break;
      
  	case BUTTON_2: 
        hal->switchToScreen("HumidityBorderScreen");
  	break;
    
  	case BUTTON_3: 
        hal->switchToScreen("ADCBorderScreen");
  	break;
    
  	case BUTTON_4:
  		hal->switchToScreen("Main");
  	break;
	}

#else
#error "Unsupported display!"  
#endif

}
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
