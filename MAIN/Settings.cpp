//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Settings.h"
#include "CONFIG.h"
#include "Buttons.h"
#include "DS3231.h"
#include "Logger.h"
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
  bWantCheckAlarm = false;
  alarmTimer = 0;
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

  setupDS3231Alarm();
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

    setupDS3231Alarm();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setupDS3231Alarm()
{
  // выключаем все будильники
  RealtimeClock.armAlarm1(false);
  RealtimeClock.armAlarm2(false);
  RealtimeClock.clearAlarm1();
  RealtimeClock.clearAlarm2();

  // обнуляем счётчик минут
  alarmTimer = 0;
  
  if(!bLoggingEnabled) // логгирование выключено, ничего делать не надо
    return;

  // настраиваем будильник на часах на каждую минуту
  RealtimeClock.setAlarm2(0, 0, 0, DS3231_EVERY_MINUTE);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::alarmFunction()
{
  // просто выставляем флаг, что будильник сработал
  Settings.bWantCheckAlarm = true;
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

  attachInterrupt(RTC_ALARM_PIN, alarmFunction, FALLING); // прерывание вызывается только при смене значения на порту с LOW на HIGH

  // настраиваем будильник часов
  setupDS3231Alarm();
  
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
void SettingsClass::checkIsAlarm()
{
  // сперва очищаем будильник по-любому
  RealtimeClock.clearAlarm2();

  // теперь проверяем - не пришло ли время логгировать?
  alarmTimer++;

  uint16_t logInterval = getLoggingInterval();

  if(alarmTimer >= logInterval)
  {
    // пришло время собирать информацию и писать на SD !!!
    alarmTimer = 0;

    // выставили флаг - и внутри update всё сделается
    bWantToLogFlag = true;
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::logDataToSD()
{
  //ТУТ ЛОГГИРОВАНИЕ ИНФОРМАЦИИ С ДАТЧИКОВ НА SD
  String dataLine;

  // в первом столбце - дата/время логгирования
  DS3231Time tm = RealtimeClock.getTime();
  String dateStr = RealtimeClock.getDateStr(tm);
  dateStr += ' ';
  dateStr  += RealtimeClock.getTimeStr(tm);

  dataLine = Logger.formatCSV(dateStr);
  dataLine += COMMA_DELIMITER;

  // во втором столбце - показания аналогового датчика
  dataLine += Logger.formatCSV(String(analogSensorValue));
  dataLine += COMMA_DELIMITER;

  // в третьем столбце - температура с Si7021
  String sTemp = String(si7021Data.temperature);
  sTemp += DECIMAL_SEPARATOR;
  if(si7021Data.temperatureFract < 10)
    sTemp += '0';
  sTemp += si7021Data.temperatureFract;
  dataLine += Logger.formatCSV(sTemp);
  dataLine += COMMA_DELIMITER;  

  // в четвертом столбце - влажность с Si7021
  sTemp = String(si7021Data.humidity);
  sTemp += DECIMAL_SEPARATOR;
  if(si7021Data.humidityFract < 10)
    sTemp += '0';
  sTemp += si7021Data.humidityFract;
  dataLine += Logger.formatCSV(sTemp);
  dataLine += COMMA_DELIMITER;   

  //TODO: ОСТАЛЬНЫЕ ДАННЫЕ ДЛЯ ЛОГА - ЗДЕСЬ !!!


  dataLine += NEWLINE;
  Logger.write((uint8_t*)dataLine.c_str(),dataLine.length());
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::update()
{  
  if(millis() - sensorsUpdateTimer > SENSORS_UPDATE_FREQUENCY)
  {
    sensorsUpdateTimer = millis();
    updateDataFromSensors();
  }

  if(bWantCheckAlarm)
  {
    bWantCheckAlarm = false;
    checkIsAlarm();
  }

  if(bWantToLogFlag)
  {
    bWantToLogFlag = false;

    if(bLoggingEnabled)
    {
        updateDataFromSensors();    
        logDataToSD();
      
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

