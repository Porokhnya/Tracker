#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define _DEBUG // закомментировать для выключения отладочной информации
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define Serial SerialUSB     // Подключить USB порт
#define SERIAL_SPEED 19200  // скорость работы с Serial
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
#define RTC_ALARM_PIN 6 // номер пина, с которого приходят прерывания будильника
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки датчиков
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SENSORS_UPDATE_FREQUENCY 2000 // частота обновления показаний с датчиков для главного экрана, миллисекунд
#define ANALOG_SENSOR_PIN A1 // номер аналогового пина для второго датчика
#define LOGGING_INTERVAL_INDEX 0 // индекс интервала логгирования по умолчанию

#define LOGGING_INTERVALS_COUNT 6 // кол-во известных интервалов
#define LOGGING_INTERVALS 5, 10, 20, 30, 40, 60 // допустимые интервалы настроек логгирования, минут (указываются через запятую, кол-вом LOGGING_INTERVALS_COUNT !!!)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки SD
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SD_CS_PIN 10                 // пин CS для SD-карты
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки EEPROM
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// заголовки записей, которым нужны префиксы
#define RECORD_HEADER1 0xD1
#define RECORD_HEADER2 0xFE
#define RECORD_HEADER3 0x23

#define LOGGING_INTERVAL_ADDRESS 50 // адрес хранения интервала сбора информации с датчиков, в минутах (1 байт)
#define LOGGING_ENABLED_ADDRESS 51 // адрес хранения флага - активно ли логгирование (1 байт)
#define LOGGER_DOM_ADDRESS 52 // адрес хранения текущего дня месяца, 1 байт (нужно для логгера, чтобы пре перезапуске он работал с нужным файлом)
#define LOGGER_FN_ADDRESS 53 // адрес хранения номера файла за сегодня, 1 байт (нужно для логгера, чтобы при перезапуске он работал с нужным файлом)
#define LOGGING_DURATION_ADDRESS 54 // адрес хранения времени начала логгирования, unixtime, 4 байта
#define LOGGING_DURATION_VALUE_ADDRESS 58 // адрес хранения времени (в секундах) накопительного значения логгирования за всё время жизни контроллера, 4 байта
#define UUID_STORE_ADDRESS  65 // адрес хранения уникального идентификатора контроллера (32 байта идентификатор + 3 байта - префикс = 35 байт)

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка логов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define LOGS_DIRECTORY "/LOG" // папка, в которой лежат логи
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
// через какое время, в миллисекундах, гасить подсветку дисплея при питании от батарей
#define BACKLIGHT_OFF_DELAY 300000
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// клавиатура
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define Key_line_In11 11 // Линия приема сигнала от 1 ряда кнопок
#define Key_line_In12 12 // Линия приема сигнала от 2 ряда кнопок

#define Key_line_Out0 0 // МСР 0 Линия выдачи сигнала на кнопки
#define Key_line_Out1 1 // МСР 1 Линия выдачи сигнала на кнопки
#define Key_line_Out2 2 // МСР 2 Линия выдачи сигнала на кнопки
#define Key_line_Out3 3 // МСР 3 Линия выдачи сигнала на кнопки
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Настройка линий питания
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define  LCD_led 4                                  // МСР 4 Вывод подсветки питания дисплея
#define  PWR_On_Out 5                               // МСР 5 Вывод поддержки включения питания. Отключение питания контроллера                        
#define  PWR_ESP 6                                  // МСР 6 Вывод управления питанием модуля WiFi
#define  LED 13                                     // Настрока светодиода индикации                    

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Привязки кнопок к их кодам
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define BUTTON_1 1
#define BUTTON_2 2
#define BUTTON_3 3
#define BUTTON_4 4
#define BUTTON_5 5
#define BUTTON_6 6
#define BUTTON_7 7
#define BUTTON_POWER 38 // Вход признака включения питания

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define DECIMAL_SEPARATOR "." // разделитель целой/дробной частей числа
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------







//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _DEBUG
  #define DBG(s) { Serial.print((s)); }
  #define DBGLN(s) { Serial.print((s)); Serial.println(); }
#else
  #define DBG(s) (void) 0
  #define DBGLN(s) (void) 0
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define CORE_COMMAND_GET F("GET=") // префикс для команды получения данных из ядра
#define CORE_COMMAND_SET F("SET=") // префикс для команды сохранения данных в ядро
#define CORE_COMMAND_ANSWER_OK F("OK=") // какой префикс будет посылаться в ответ на команду получения данных и её успешной отработке
#define CORE_COMMAND_ANSWER_ERROR F("ER=") // какой префикс будет посылаться в ответ на команду получения данных и её неуспешной отработке
#define CORE_COMMAND_PARAM_DELIMITER '|' // разделитель параметров
#define CORE_END_OF_DATA F("[END]")
#define CORE_COMMAND_DONE F("DONE")
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

