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
#define LED 13
#define Serial SERIAL_PORT_USBVIRTUAL     // Подключить USB порт
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
  Serial2.begin(19200); // Возможно для принтера

  //Assign pins 3 & 4 SERCOM functionality
  pinPeripheral(3, PIO_SERCOM_ALT);    // Настройка Serial2
  pinPeripheral(4, PIO_SERCOM_ALT);    // Настройка Serial2  
  delay(500);
 // while(!Serial);

 Wire.begin();

  // настраиваем железные кнопки
  Buttons.begin();

  DBGLN(F("INIT settings..."));
  Settings.begin();
  DBGLN(F("Settings inited."));
  
  DBGLN(F("Init RTC..."));
  RealtimeClock.begin(RTC_WIRE_NUMBER);           // запускаем их на шине I2C 1 (SDA1, SCL1)
  //RealtimeClock.setTime(0,30,0,5,27,7,2018);

  DBGLN(F("INIT SD..."));
  SDInit::InitSD();
  DBGLN(F("SD inited."));
  
  DBGLN(F("Init screen..."));
  Screen.setup();
  
  DBGLN(F("Add main screen...")); 
  Screen.addScreen(MainScreen::create());           // первый экран покажется по умолчанию

  
  // переключаемся на первый экран
  Screen.switchToScreen("Main");


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


  /*
  // обрабатываем входящие команды
  CommandHandler.handleCommands();
  */

  //Settings.resetPressedKey();

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

