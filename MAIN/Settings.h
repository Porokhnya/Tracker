#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <wiring_private.h>
#include "AT24CX.h"
#include "DS3231.h"
#include "MCP23017.h"
#include <Adafruit_Si7021.h>
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

    Si7021Data readSensor() { return si7021Data; }
    uint16_t getAnalogSensorValue() { return analogSensorValue; }

    void displayBacklight(bool bOn);
    void espPower(bool bOn);

    int getPressedKey() {return pressedKey; }
    void resetPressedKey(){pressedKey = 0;}
    
  private:

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
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
