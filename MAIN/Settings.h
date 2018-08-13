#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <wiring_private.h>
#include "AT24CX.h"
#include "DS3231.h"
#include "MCP23017.h"
#include <Adafruit_Si7021.h>
#include "LinkList.h"
#include "CONFIG.h"
#include "CoreButton.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  powerViaUSB = 10,
  batteryPower = 20
  
} PowerType;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct
{
  int raw;
  float voltage;
  
} VoltageData;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define NO_TEMPERATURE_DATA -127
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct
{
  int8_t temperature;
  uint8_t temperatureFract;
  
  int8_t humidity;
  uint8_t humidityFract;
  
} Si7021Data;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SettingsClass 
{
  public:

    SettingsClass();

    void begin();
    void update();

    // работа с ESP
    bool shouldConnectToRouter() { return false; }
    
    String getStationID();
    void setStationID(const String& val);
    
    String getStationPassword();
    void setStationPassword(const String& val);
    
    String getRouterID() { return ""; }
    String getRouterPassword() { return ""; }

    // возвращает значение температуры/влажности с датчика Si7021
    Si7021Data readSensor() { return si7021Data; }

    // возвращает значение с аналогового датчика
    uint16_t getAnalogSensorValue() { return analogSensorValue; }

    // управление подсветкой экрана
    void displayBacklight(bool bOn);

    // управление питанием ESP
    void espPower(bool bOn);

    // возвращает индекс интервала в массиве известных интервалов логгирования
    uint8_t getLoggingIntervalIndex() { return loggingInterval; }

    // устанавливает индекс активного интервала в массиве известных интервалов логгирования
    void setLoggingIntervalIndex(uint8_t val);

    // возвращает актуальное значение интервала логгирования, в минутах
    uint8_t getLoggingInterval();

    // включено ли логгирование?
    bool isLoggingEnabled() { return bLoggingEnabled; } 

    // управление активностью логгирования
    void switchLogging(bool bOn);

    void setLoggerDayOfMonth(uint8_t lastDayOfMonth);
    void setLoggerLogFileNumber(uint8_t todayLogFileNumber);    
    uint8_t getLoggerDayOfMonth();
    uint8_t getLoggerLogFileNumber();

    // возвращает тип питания - от батарей или USB
    PowerType getPowerType();
    void turnPowerOff(); // выключает питание контроллера

    uint32_t getLoggingDuration(); // возвращает длительность логгирования
    void setLoggingDuration(uint32_t val);

    String getUUID(const char* passedUUID);

    bool isDoorOpen();
    
  private:

      uint8_t doorState;

      Button powerButton;      


      uint8_t read8(uint16_t address, uint8_t defaultVal = 0);
      void write8(uint16_t address, uint8_t val);

      uint16_t read16(uint16_t address);
      void write16(uint16_t address, uint16_t val);

      uint32_t read32(uint16_t address);
      void write32(uint16_t address, uint32_t val);

      String readString(uint16_t address, byte maxlength);
      void writeString(uint16_t address, const String& v, byte maxlength);


      void saveLoggingTimer();
      void accumulateLoggingDuration();
      

      PowerType powerType;
      static void checkPower();

      void setupDS3231Alarm();
      static void alarmFunction();
      bool bWantCheckAlarm;
      void checkIsAlarm();
      uint16_t alarmTimer;


      void logDataToSD();


     void updateDataFromSensors();

     uint8_t loggingInterval;
     bool bLoggingEnabled;
     bool bWantToLogFlag;

    static void test_key();
    static int pressedKey;

    AT24C64* eeprom;
    DS3231Temperature coreTemp;
    uint32_t sensorsUpdateTimer;
    uint16_t analogSensorValue;

    Adafruit_MCP23017 MCP;
    Adafruit_Si7021 si7021;
    Si7021Data si7021Data;
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern SettingsClass Settings;
extern Uart Serial2;
extern uint8_t KNOWN_LOGGING_INTERVALS[LOGGING_INTERVALS_COUNT];
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
