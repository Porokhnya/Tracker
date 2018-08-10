#include <Arduino.h>
#include <wiring_private.h>
#include "CONFIG.h"
#include "ScreenHAL.h"
#include "DS3231.h"               // подключаем часы
#include "AT24CX.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// подключаем наши экраны
#include "MainScreen.h"              // Главный экран
#include "Buttons.h"              // наши железные кнопки
#include "FileUtils.h"
#include "Settings.h"
#include "CoreCommandBuffer.h"
#include "Logger.h"



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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Uart Serial2(&sercom2, 3, 4, SERCOM_RX_PAD_1, UART_TX_PAD_0); // Подключить Serial2
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SERCOM2_Handler() // Подключить Serial2
{
  Serial2.IrqHandler();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() 
{
  Serial.begin(SERIAL_SPEED);
  Serial1.begin(115200); // WiFi
  delay(1000);    // Подождать вывод в SerialUSB
  // раскомментировать для отладочной информации !!!
  //while(!Serial);

  Wire.begin();

  DBGLN(F("Init RTC..."));
  RealtimeClock.begin(RTC_WIRE_NUMBER);           // запускаем их на шине I2C 1 (SDA1, SCL1)
  // Отключить вывод импульсов 32 кгц на выходе
  RealtimeClock.enable32kHz(false);


  DBGLN(F("INIT settings..."));
  Settings.begin();
  Logger.begin();
  DBGLN(F("Settings inited."));

  // настраиваем железные кнопки
  Buttons.begin();

  Serial2.begin(SERIAL_SPEED); // Возможно для принтера 

  //Assign pins 3 & 4 SERCOM functionality
  pinPeripheral(3, PIO_SERCOM_ALT);    // Настройка Serial2
  pinPeripheral(4, PIO_SERCOM_ALT);    // Настройка Serial2  
  delay(1000);
  
  
 // RealtimeClock.setTime(5,16,9,5,27,7,2018);

  DBGLN(F("INIT SD..."));
  SDInit::InitSD();
  DBGLN(F("SD inited."));
  
  DBGLN(F("Init screen..."));
  Screen.setup();
  
  DBGLN(F("Add main screen...")); 
  Screen.addScreen(MainScreen::create());           // первый экран покажется по умолчанию
  DBGLN(F("Main screen added.")); 

  DBGLN(F("Add menu screen #1...")); 
  Screen.addScreen(MenuScreen1::create());
  DBGLN(F("Menu screen #1 added.")); 

  DBGLN(F("Add menu screen #2..."));
  Screen.addScreen(MenuScreen2::create());
  DBGLN(F("Menu screen #2 added."));

   DBGLN(F("Add menu screen #3..."));
  Screen.addScreen(MenuScreen3::create());
  DBGLN(F("Menu screen #3 added."));

   DBGLN(F("Add menu screen #4..."));
  Screen.addScreen(MenuScreen4::create());
  DBGLN(F("Menu screen #4 added."));

    // переключаемся на первый экран
  Screen.switchToScreen("Main");


  screenIdleTimer = millis();
  Screen.onAction(screenAction);


  DBGLN(F("Inited."));

  Serial.print(F("TRACKER "));
  Serial.println(SOFTWARE_VERSION);

  //
  //// тестовый код создания кучи файлов на SD
  //const char* buff = "DUMMY STRING\r\n";
  //for(int i=0;i<20;i++)
  //{    
  //  Logger.write((uint8_t*)buff,strlen(buff));
  //  Logger.newLogFile();
  //}
  //
  

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


  /*
  // обрабатываем входящие команды
  CommandHandler.handleCommands();
  */

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

