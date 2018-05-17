#include <Arduino.h>
#include "CONFIG.h"
#include "ScreenHAL.h"
#include "DS3231.h"               // подключаем часы
#include "AT24CX.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// подключаем наши экраны
#include "MainScreen.h"              // Главный экран
#include "Buttons.h"              // наши железные кнопки
#include "InfoDiodes.h"           // информационные диоды
#include "FileUtils.h"
#include "Settings.h"
#include "CoreCommandBuffer.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t screenIdleTimer = 0;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void screenAction(AbstractHALScreen* screen)
{
   // какое-то действие на экране произошло.
   // тут просто сбрасываем таймер ничегонеделанья.
   screenIdleTimer = millis();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define Serial SERIAL_PORT_USBVIRTUAL     // Подключить USB порт

void setup() 
{
  Serial.begin(SERIAL_SPEED);
 // while(!Serial);

   DBGLN(F("INIT settings..."));
  Settings.begin();
  DBGLN(F("Settings inited."));
  
  DBGLN(F("Init RTC..."));
  RealtimeClock.begin(RTC_WIRE_NUMBER);           // запускаем их на шине I2C 1 (SDA1, SCL1)
 // RealtimeClock.setTime(0,1,11,1,7,2,2018);

  DBGLN(F("INIT SD..."));
  SDInit::InitSD();
  DBGLN(F("SD inited."));
  

  DBGLN(F("Init screen..."));
  Screen.setup();

  DBGLN(F("Add main screen...")); 
  Screen.addScreen(MainScreen::create());           // первый экран покажется по умолчанию
  
  // переключаемся на первый экран
  Screen.switchToScreen("Main");

  // настраиваем информационные диоды
  InfoDiodes.begin();

  // настраиваем железные кнопки
  Buttons.begin();

  screenIdleTimer = millis();
  Screen.onAction(screenAction);

  DBGLN(F("Inited."));

  Serial.print(F("TRACKER "));
  Serial.println(SOFTWARE_VERSION);

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() 
{

  Settings.update();
  
  // обновляем кнопки
  Buttons.update();
  Screen.update();


  // проверяем, какой экран активен. Если активен главный экран - сбрасываем таймер ожидания. Иначе - проверяем, не истекло ли время ничегонеделанья.
  AbstractHALScreen* activeScreen = Screen.getActiveScreen();
  if(activeScreen == mainScreen)
  {
    screenIdleTimer = millis();
  }
  else
  {
      if(millis() - screenIdleTimer > RESET_TO_MAIN_SCREEN_DELAY)
      {
        screenIdleTimer = millis();
        Screen.switchToScreen(mainScreen);
      }
  } // else


  // обрабатываем входящие команды
  CommandHandler.handleCommands();

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool nestedYield = false;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void yield()
{
  
  if(nestedYield)
    return;
    
 nestedYield = true;
   // обновляем кнопки   
   Buttons.update();

 nestedYield = false;
 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

