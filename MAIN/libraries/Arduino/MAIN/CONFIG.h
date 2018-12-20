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
#define DS18B20_SENSOR_PIN A1 // номер пина для второго датчика (DS18B20)
//#define ADC_COEFF 32.3 // коэффициент перевода значения ADC в температуру
#define LOGGING_INTERVAL_INDEX 0 // индекс интервала логгирования по умолчанию
#define DOOR_ENDSTOP_PIN A2 // пин, на котором висит концевик двери
#define DOOR_OPEN HIGH // уровень, когда дверь открыта
#define INSIDE_TEMPERATURE_CORRECT 1 //коэффициент корректировки датчика DS18B20
#define OUTSIDE_TEMPERATURE_CORRECT 1 //коэффициент корректировки датчика DS18B20
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// интервалы
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define LOGGING_MIN_INTERVAL 1 // минимальный интервал логгирования, минут (0-255)
#define LOGGING_MAX_INTERVAL 120 // максимальный интервал логгирования, минут (0-255)
#define LOGGING_INTERVAL_STEP 1 // шаг изменения интервалов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// пороги
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TEMPERATURE_MIN_BORDER -20 // минимальное значение порога температуры для экрана настроек
#define TEMPERATURE_MIN_BORDER_DEFAULT_VAL 0 // значение минимального порога температуры по умолчанию
#define TEMPERATURE_MAX_BORDER 30 // максимальное значение порога температуры для экрана настроек
#define TEMPERATURE_MAX_BORDER_DEFAULT_VAL 25 // значение максимального порога температуры по умолчанию
#define TEMPERATURE_BORDER_CHANGE_STEP 1 // шаг изменений порога температуры на экране настроек

#define HUMIDITY_MIN_BORDER 0 // минимальное значение порога влажности для экрана настроек
#define HUMIDITY_MIN_BORDER_DEFAULT_VAL 0 // значение минимального порога влажности по умолчанию
#define HUMIDITY_MAX_BORDER 100 // максимальное значение порога влажности для экрана настроек
#define HUMIDITY_MAX_BORDER_DEFAULT_VAL 100 // значение максимального порога влажности по умолчанию
#define HUMIDITY_BORDER_CHANGE_STEP 1 // шаг изменений порога влажности на экране настроек

#define ADC_MIN_BORDER -20 // минимальное значение порога аналогового датчика для экрана настроек
#define ADC_MIN_BORDER_DEFAULT_VAL 2 // значение минимального порога аналогового датчика по умолчанию
#define ADC_MAX_BORDER 1000 // максимальное значение порога аналогового датчика для экрана настроек
#define ADC_MAX_BORDER_DEFAULT_VAL 50 // значение максимального порога аналогового датчика по умолчанию
#define ADC_BORDER_CHANGE_STEP 1 // шаг изменений порога аналогового датчика на экране настроек

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки SD
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SD_CS_PIN 10                 // пин CS для SD-карты
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки ESP
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define WIFI_SERIAL Serial1 // Какой Serial для ESP
#define WIFI_BAUD_RATE 115200 // скорость работы с WIFI
#define WIFI_MAX_ANSWER_TIME 30000 // сколько мс максимально ждать ответа от ESP
#define USE_WIFI_REBOOT // раскомментировать, если надо использовать управление питанием ESP
#define WIFI_AVAILABLE_CHECK_TIME 60000 // через сколько мс проверять ESP на зависание (отсылкой команды AT)
#define WIFI_REBOOT_TIME 1000 // сколько мс держать питание выключенным при перезагрузке ESP
#define WIFI_WAIT_AFTER_REBOOT_TIME 1000 // сколько мс ждать после включения питания ESP до подачи первой команды
#define DEFAULT_STATION_ID F("tracker") // идентификатор станции ESP по умолчанию (19 символов максимум)
#define DEFAULT_STATION_PASSWORD F("12345678") // пароль станции ESP по умолчанию (минимум 8 символов, максимум - 19)
#define DEFAULT_ROUTER_ID F("") // идентификатор роутера, к которому коннектится ESP, по умолчанию (19 символов максимум)
#define DEFAULT_ROUTER_PASSWORD F("") // пароль роутера, к которому коннектится ESP, по умолчанию (19 символов максимум)
#define WIFI_SEND_INTERVAL 1 // интервал по умолчанию, в минутах, отсыла показаний на конечное устройство
#define WIFI_CONNECT_TIMEOUT 120000 // таймаут попытки соединения с устройством через Wi-Fi, миллисекунд
#define WIFI_WRITE_TIMEOUT 10000 // таймаут записи через ESP на принимающее устройство, миллисекунд
#define WIFI_DEBUG_CONNECT_TO F("192.168.43.50") // если раскомментировано - то коннектимся к этому адресу в локальной сети, если закомментировано - коннектимся прямо к точке доступа Wi-Fi
#define WIFI_MAX_RECORDS_IN_ONE_PACKET 10 // максимальное кол-во записей в одном пакете отсыла данных от ESP
#define WIFI_CONNECT_ATTEMPTS 3 // кол-во попыток соединиться с конечным устройством в рамках одной сессии
#define HTTP_HANDLER F("/") // адрес обработчика HTTP-запросов
#define HTTP_HOST F("current") // данные заголовка Host: (закомментировать, если не надо отсылать этот заголовок)
#define MAX_WIFI_LIST_RECORDS 100 // сколько максимально держать записей в очереди на отправку (по превышении этого лимита старые записи будут удаляться)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Служебная информация
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define DEFAULT_DRIVER F("") //ФИО водителя по умолчанию (70 символов)
#define DEFAULT_VEHICLE_NUMBER F("") //Гос. номер машины (10 символов)
#define DEFAULT_TRAILER_NUMBER F("") //Гос.номер прицепа (10 символов)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Калибровочные данные
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
#define STATION_ID_ADDRESS 100 // адрес хранения ID станции ESP (20 байт, включая завершающий 0)
#define STATION_PASSWORD_ADDRESS 120 // адрес хранения пароля станции ESP (20 байт, включая завершающий 0)
#define TOTAL_MEASURES_ADDRESS 140 // адрес хранения общего кол-ва измерений за всё время жизни контроллера (4 байта)
#define ROUTER_ID_ADDRESS 150 // адрес хранения ID роутера, к которому будет коннектиться ESP (20 байт, включая завершающий 0)
#define ROUTER_PASSWORD_ADDRESS 170 // адрес хранения пароля роутера, к которому будет коннектиться ESP (20 байт, включая завершающий 0)
#define TEMP_MIN_BORDER_ADDRESS 200 // адрес хранения минимального порога температуры (1 байт)
#define TEMP_MAX_BORDER_ADDRESS 201 // адрес хранения максимального порога температуры (1 байт)
#define HUMIDITY_MIN_BORDER_ADDRESS 202 // адрес хранения минимального порога влажности (1 байт)
#define HUMIDITY_MAX_BORDER_ADDRESS 203 // адрес хранения максимального порога влажности (1 байт)
#define ADC_MIN_BORDER_ADDRESS 204 // адрес хранения минимального порога аналогового датчика (2 байта)
#define ADC_MAX_BORDER_ADDRESS 206 // адрес хранения максимального порога аналогового датчика (2 байта)
#define WIFI_SEND_INTERVAL_ADDRESS 210 // адрес хранения интервала между отсылами данных по Wi-Fi (4 байта)
#define SESSION_MEASURES_ADDRESS 215 // адрес хранения кол-ва измерений за текущую сессию (4 байта)
#define VEHICLE_NUMBER_ADDRESS 220 // адрес хранения гос. номера ТС (10 символов макс)
#define TRAILER_NUMBER_ADDRESS 231 // адрес хранения гос. номера прицепа (10 символов макс)
#define DRIVER_ADDRESS 242 // адрес хранения ФИО водителя (70 символов макс)
//#define INSIDECORRECT_ADDRESS 320 //калибровочный коэффициент для внутреннего датчика
//#define OUTSIDECORRECT_ADDRESS 325 ////калибровочный коэффициент для внешнего датчика

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка логов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define LOGS_DIRECTORY "/LOG" // папка, в которой лежат логи
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
#define TURN_POWER_OFF_DELAY 2000 // задержка выключения контроллера по нажатию кнопки выключения питания, миллисекунд






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