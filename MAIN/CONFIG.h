#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define _DEBUG // закомментировать для выключения отладочной информации
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// скорость работы с Serial
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SERIAL_SPEED 115200 
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// типы поддерживаемых дисплеев
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define DISPLAY_ILI9341 1
#define DISPLAY_NOKIA5110 2
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Какой дисплей используем
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#define DISPLAY_USED DISPLAY_ILI9341
#define DISPLAY_USED DISPLAY_NOKIA5110
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define DISPLAY_INIT_DELAY 100 // сколько миллисекунд ждать после инициализации дисплея
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Настройки дисплея Nokia 5110
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define NOKIA_SCK_PIN 7 
#define NOKIA_MOSI_PIN A4
#define NOKIA_DC_PIN 5 
#define NOKIA_RST_PIN 30
#define NOKIA_CS_PIN 31
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки тачскрина
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TFT_TOUCH_CLK_PIN 6
#define TFT_TOUCH_CS_PIN 5
#define TFT_TOUCH_DIN_PIN 4
#define TFT_TOUCH_DOUT_PIN 3
#define TFT_TOUCH_IRQ_PIN 2
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TFT model
#define TFT_MODEL TFT01_24SP // или ILI9341_S4P пробовать
//DUE
//#define CS 10
//#define RESET 8
//#define DC 9

//SAMD21
//#define CS 9
//#define RESET 7
//#define DC 8
//#define LED 11
//UTFT myGLCD(TFT01_24SP, MOSI, SCK, CS, RESET, DC);
//UTFT myGLCD(TFT01_24SP, SDI/MOSI, SCK, CS, RESET, DC/RS);
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TFT RS pin
#define TFT_RS_PIN MOSI
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TFT WR pin
#define TFT_WR_PIN SCK
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TFT CS pin
#define TFT_CS_PIN 9
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TFT RST pin
#define TFT_RST_PIN 7
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TFT D/C pin
#define TFT_DC_PIN 8
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки часов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define RTC_WIRE_NUMBER 0 // какой I2C используется для часов (указать 1, если используется второй I2C)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки датчиков
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SENSORS_UPDATE_FREQUENCY 2000 // частота чтения с датчиков, миллисекунд
#define DS18B20_PIN 8 // номер пина для датчика DS18B20
#define ANALOG_SENSOR_PIN A1 // номер аналогового пина для второго датчика
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки SD
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SD_CS_PIN 10                 // пин CS для SD-карты
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки светодиодов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define LED_ON_LEVEL HIGH // уровень включения светодиодов

#define LED_READY  37                // Индикация светодиодом "Готов"
// настройка кнопок
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка логов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define LOGS_DIRECTORY "/LOG" // папка, в которой лежат логи
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки обновления состояния исправности индуктивных датчиков
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define INDUCTIVE_SENSORS_UPDATE_INTERVAL 1000 // интервал обновления, в миллисекундах
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// версия ПО
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SOFTWARE_VERSION "v.0.1a"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// время в миллисекундах, после которого идёт переключение на главный экран, если какой-то экран висит долгое время
#define RESET_TO_MAIN_SCREEN_DELAY 300000
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


















#ifdef _DEBUG
  #define DBG(s) { Serial << (s); }
  #define DBGLN(s) { Serial << (s) << ENDL; }
#else
  #define DBG(s) (void) 0
  #define DBGLN(s) (void) 0
#endif
#define ENDL '\n'
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T> inline Stream& operator << (Stream &s, T n) { s.print(n); return s; }
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define CORE_COMMAND_GET F("GET=") // префикс для команды получения данных из ядра
#define CORE_COMMAND_SET F("SET=") // префикс для команды сохранения данных в ядро
#define CORE_COMMAND_ANSWER_OK F("OK=") // какой префикс будет посылаться в ответ на команду получения данных и её успешной отработке
#define CORE_COMMAND_ANSWER_ERROR F("ER=") // какой префикс будет посылаться в ответ на команду получения данных и её неуспешной отработке
#define CORE_COMMAND_PARAM_DELIMITER '|' // разделитель параметров
#define CORE_END_OF_DATA F("[END]")
#define CORE_COMMAND_DONE F("DONE")
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

