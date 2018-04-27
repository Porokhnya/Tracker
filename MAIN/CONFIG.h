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
// настройки экрана
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define LCD_SCK_PIN     7
#define LCD_MOSI_PIN    6
#define LCD_DC_PIN      5
#define LCD_RST_PIN     4
#define LCD_CS_PIN      3
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки часов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define RTC_WIRE Wire // какой I2C используется для часов (указать Wire1, если используется второй I2C)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки SD
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SD_CS_PIN 10                 // пин CS для SD-карты
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки светодиодов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define LED_ON_LEVEL HIGH // уровень включения светодиодов

#define LED_READY  37                // Индикация светодиодом "Готов"
#define LED_FAILURE  36              // Индикация светодиодом "Авария"
#define LED_TEST  38                 // Индикация светодиодом "Тест"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка кнопок
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define BUTTON_RED  39                  // Кнопка №1, красная
#define BUTTON_BLUE1  40                  // Кнопка №2, синяя
#define BUTTON_BLUE2  41                  // Кнопка №3, синяя
#define BUTTON_YELLOW  9                   // Кнопка №4, жёлтая
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
#define RESET_TO_MAIN_SCREEN_DELAY 60000
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

