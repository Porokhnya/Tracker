//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Settings.h"
#include "CONFIG.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SettingsClass Settings;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SettingsClass::SettingsClass()
{
  eeprom = NULL;
  analogSensorValue = 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::displayBacklight(bool bOn)
{
  MCP.digitalWrite(4, bOn ? LOW : HIGH);  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::espPower(bool bOn)
{
  MCP.digitalWrite(6,bOn ? LOW : HIGH);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::begin()
{
  eeprom = new AT24C64();
  analogSensorValue = 0;
  sensorsUpdateTimer = millis() + SENSORS_UPDATE_FREQUENCY;

  // подключаем MCP на адрес 1
  MCP.begin(1);

  // настраиваем каналы клавиатуры
  MCP.pinMode(0,INPUT);
  MCP.pinMode(1,INPUT);
  MCP.pinMode(2,INPUT);
  MCP.pinMode(3,INPUT);

  // настраиваем канал подсветки дисплея
  MCP.pinMode(4,OUTPUT);

  // включаем подсветку дисплея
  displayBacklight(true);


  // настраиваем "подхват питания"
  MCP.pinMode(5,OUTPUT);

  // настраиваем управление питанием ESP
  MCP.pinMode(6,OUTPUT);

  // выключаем питание ESP
  espPower(false);

  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::update()
{
  if(millis() - sensorsUpdateTimer > SENSORS_UPDATE_FREQUENCY)
  {
    sensorsUpdateTimer = millis();
    analogSensorValue = analogRead(ANALOG_SENSOR_PIN);
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
uint16_t SettingsClass::getChannelPulses(uint8_t channelNum)
{
  uint16_t addr = 0;
  switch(channelNum)
  {
    case 0:
    addr = COUNT_PULSES_STORE_ADDRESS1;
    break;
    
    case 1:
    addr = COUNT_PULSES_STORE_ADDRESS2;
    break;
    
    case 2:
    addr = COUNT_PULSES_STORE_ADDRESS3;
    break;
  }

  uint16_t result = 0;
  uint8_t* writePtr = (uint8_t*)&result;
  eeprom->read(addr,writePtr,sizeof(uint16_t));

  if(result == 0xFFFF)
  {
    result = 0;
    setChannelPulses(channelNum,result);
  }

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setChannelPulses(uint8_t channelNum, uint16_t val)
{
  uint16_t addr = 0;
  switch(channelNum)
  {
    case 0:
    addr = COUNT_PULSES_STORE_ADDRESS1;
    break;
    
    case 1:
    addr = COUNT_PULSES_STORE_ADDRESS2;
    break;
    
    case 2:
    addr = COUNT_PULSES_STORE_ADDRESS3;
    break;
  }

    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(addr,writePtr,sizeof(uint16_t));
}
*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

