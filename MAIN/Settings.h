#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <wiring_private.h>
#include "AT24CX.h"
#include "DS3231.h"
#include "MCP23017.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct
{
  int raw;
  float voltage;
  
} VoltageData;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SettingsClass 
{
  public:

    SettingsClass();

    void begin();
    void update();

    //DS18B20Temperature getDS18B20Temperature() { return dsTemp; }
    uint16_t getAnalogSensorValue() { return analogSensorValue; }

    void displayBacklight(bool bOn);
    void espPower(bool bOn);
    
  private:

    AT24C64* eeprom;
    DS3231Temperature coreTemp;
    uint32_t sensorsUpdateTimer;
    uint16_t analogSensorValue;

    Adafruit_MCP23017 MCP;
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern SettingsClass Settings;
extern Uart Serial2;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
