//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Settings.h"
#include "CONFIG.h"
#include "Buttons.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SettingsClass Settings;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int SettingsClass::pressedKey = 0;
uint8_t KNOWN_LOGGING_INTERVALS[ LOGGING_INTERVALS_COUNT ] = { LOGGING_INTERVALS };
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::test_key()
{
  pressedKey = 0;
  Settings.MCP.digitalWrite(Key_line_Out0, HIGH);
  Settings.MCP.digitalWrite(Key_line_Out1, HIGH);
  Settings.MCP.digitalWrite(Key_line_Out2, HIGH);
  Settings.MCP.digitalWrite(Key_line_Out3, HIGH);

  for (int i = 0; i < 4; i++)
  {
      Settings.MCP.digitalWrite(i, LOW);
      
          if (digitalRead(Key_line_In11) == LOW)
          {
              pressedKey = 4 - i;
			  while (digitalRead(Key_line_In11) == LOW) {}
              break;
          }
     // Settings.MCP.digitalWrite(i, HIGH);
  }
  Settings.MCP.digitalWrite(Key_line_Out0, HIGH);
  Settings.MCP.digitalWrite(Key_line_Out1, HIGH);
  Settings.MCP.digitalWrite(Key_line_Out2, HIGH);
  Settings.MCP.digitalWrite(Key_line_Out3, HIGH);
  for (int i = 0; i < 4; i++)
  {
      Settings.MCP.digitalWrite(i, LOW);
      
          if (digitalRead(Key_line_In12) == LOW)
          {
              pressedKey = 7 - i;
			  while (digitalRead(Key_line_In12) == LOW) {}
              break;
          }
  }

  Settings.MCP.digitalWrite(Key_line_Out0, LOW);
  Settings.MCP.digitalWrite(Key_line_Out1, LOW);
  Settings.MCP.digitalWrite(Key_line_Out2, LOW);
  Settings.MCP.digitalWrite(Key_line_Out3, LOW);

  //SerialUSB.println(pressedKey);
  
  Buttons.onKeyPressed(pressedKey);
  pressedKey = 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SettingsClass::SettingsClass()
{
  eeprom = NULL;
  analogSensorValue = 0;
  loggingInterval = LOGGING_INTERVAL_INDEX;
  bLoggingEnabled = true;
  bWantToLogFlag = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t SettingsClass::getLoggingInterval()
{  
  if(loggingInterval >= LOGGING_INTERVALS_COUNT)
  {
    loggingInterval = 0;
    setLoggingIntervalIndex(loggingInterval);
  }

  return KNOWN_LOGGING_INTERVALS[loggingInterval];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setLoggingIntervalIndex(uint8_t val)
{
  loggingInterval = val;

  eeprom->write(LOGGING_INTERVAL_ADDRESS, val);

  uint8_t actualLoggingInterval = getLoggingInterval();
  
  //TODO: Тут перенастройка часов на новый интервал, в переменной actualLoggingInterval лежит интервал в минутах !!!!
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::displayBacklight(bool bOn)
{
  MCP.digitalWrite(LCD_led, bOn ? LOW : HIGH);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::espPower(bool bOn)
{
  MCP.digitalWrite(PWR_ESP,bOn ? LOW : HIGH);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::switchLogging(bool bOn)
{
    bLoggingEnabled = bOn;
    eeprom->write(LOGGING_ENABLED_ADDRESS, bLoggingEnabled ? 1 : 0);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::begin()
{
 
  eeprom = new AT24C64();

  // читаем значение интервала между логгированием на SD
  loggingInterval = eeprom->read(LOGGING_INTERVAL_ADDRESS);
  if(loggingInterval == 0xFF)
  {
    loggingInterval = LOGGING_INTERVAL_INDEX;
    eeprom->write(LOGGING_INTERVAL_ADDRESS, loggingInterval);
  }


  // смотрим - активно ли логгирование?
  uint8_t lEn = eeprom->read(LOGGING_ENABLED_ADDRESS);
  if(lEn == 0xFF)
  {
    lEn = 1;
    eeprom->write(LOGGING_ENABLED_ADDRESS, lEn);
  }
        
  bLoggingEnabled = lEn == 1;
  
  analogSensorValue = 0;
  sensorsUpdateTimer = millis() + SENSORS_UPDATE_FREQUENCY;
  memset(&si7021Data,0,sizeof(si7021Data));
  
  si7021Data.temperature = NO_TEMPERATURE_DATA;
  si7021Data.humidity = NO_TEMPERATURE_DATA;

  // подключаем MCP на адрес 1
  MCP.begin(1);

  // настраиваем каналы клавиатуры
  MCP.pinMode(Key_line_Out0, OUTPUT); // Настроить кнопки
  MCP.pinMode(Key_line_Out1, OUTPUT); // Настроить кнопки
  MCP.pinMode(Key_line_Out2, OUTPUT); // Настроить кнопки
  MCP.pinMode(Key_line_Out3, OUTPUT); // Настроить кнопки
  MCP.digitalWrite(Key_line_Out0, LOW);
  MCP.digitalWrite(Key_line_Out1, LOW);
  MCP.digitalWrite(Key_line_Out2, LOW);
  MCP.digitalWrite(Key_line_Out3, LOW);
  
  //----------------------------- Подключить прерывание от кнопок ----------------------
  attachInterrupt(digitalPinToInterrupt(Key_line_In11), test_key, FALLING);
  attachInterrupt(digitalPinToInterrupt(Key_line_In12), test_key, FALLING);


  // настраиваем канал подсветки дисплея
  MCP.pinMode(LCD_led,OUTPUT);

  // включаем подсветку дисплея
  displayBacklight(true);


  // настраиваем "подхват питания"
  MCP.pinMode(PWR_On_Out,OUTPUT);

  // настраиваем управление питанием ESP
  MCP.pinMode(PWR_ESP,OUTPUT);

  // выключаем питание ESP
  espPower(false);


  // подключаем Si7021
  si7021.begin();
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::updateDataFromSensors()
{
    analogSensorValue = analogRead(ANALOG_SENSOR_PIN);

    float t = si7021.readTemperature()*100.0;
    int32_t iT = t;
    si7021Data.temperature = iT/100;
    si7021Data.temperatureFract = iT%100;

    t = si7021.readHumidity()*100.0;
    iT = t;
    si7021Data.humidity = iT/100;
    si7021Data.humidityFract = iT%100;  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::update()
{  
  if(millis() - sensorsUpdateTimer > SENSORS_UPDATE_FREQUENCY)
  {
    sensorsUpdateTimer = millis();
    updateDataFromSensors();
  }

  if(bWantToLogFlag)
  {
    bWantToLogFlag = false;

    if(bLoggingEnabled)
    {
        updateDataFromSensors();
        
      //TODO: ТУТ ЛОГГИРОВАНИЕ ИНФОРМАЦИИ С ДАТЧИКОВ НА SD !!!
      
    } // if(bLoggingEnabled)
    
  } // if(bWantToLogFlag)
  
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

