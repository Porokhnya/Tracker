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
uint32_t SettingsClass::getLoggingDuration()
{
    if(!bLoggingEnabled)
      return 0;

  return read32(LOGGING_DURATION_VALUE_ADDRESS);
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
void SettingsClass::setLoggerDayOfMonth(uint8_t lastDayOfMonth)
{
  eeprom->write(LOGGER_DOM_ADDRESS, lastDayOfMonth);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setLoggerLogFileNumber(uint8_t todayLogFileNumber)
{
  eeprom->write(LOGGER_FN_ADDRESS, todayLogFileNumber);  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t SettingsClass::getLoggerDayOfMonth()
{
  uint8_t val = eeprom->read(LOGGER_DOM_ADDRESS);

  if(val == 0xFF)
  {
    val = 0;
    setLoggerDayOfMonth(val);
  }

  return val;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t SettingsClass::getLoggerLogFileNumber()
{
  uint8_t val = eeprom->read(LOGGER_FN_ADDRESS);

  if(val == 0xFF)
  {
    val = 0;
    setLoggerLogFileNumber(val);
  }

  return val;
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

    if(bLoggingEnabled)
    {
      // выставляем таймер начала логгирования на текущее значение unixtime
      saveLoggingTimer();
    }
    else
    {
      // при каждом выключении логгирования мы должны накапливать уже отработанные секунды
      // логгирования, т.к. при включении логгирования время начала логгирования перезаписывается
      accumulateLoggingDuration();
    }

    // настраиваем будильник на часах
    setupDS3231Alarm();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::accumulateLoggingDuration()
{
    // аккумулируем время логгирования
  
    uint32_t loggingStartedAt = read32(LOGGING_DURATION_ADDRESS);
    if(loggingStartedAt > 0)
    {
      DS3231Time tm = RealtimeClock.getTime();
      uint32_t ut = tm.unixtime();

      uint32_t diff = ut - loggingStartedAt;

      // сохраняем накомленное значение
      uint32_t accumulatedVal = read32(LOGGING_DURATION_VALUE_ADDRESS);
      accumulatedVal += diff;
      write32(LOGGING_DURATION_VALUE_ADDRESS,accumulatedVal);

    }
    
    // перезаписываем время начала логгирования
    saveLoggingTimer();
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::saveLoggingTimer()
{
  // если логгирование активно - выставляем таймер начала логгирования
  if(bLoggingEnabled)
  {
    DS3231Time tm = RealtimeClock.getTime();
    uint32_t ut = tm.unixtime();
    write32(LOGGING_DURATION_ADDRESS,ut);  
  }
  else // иначе пишем туда нолик
  {
    write32(LOGGING_DURATION_ADDRESS,0);      
  }
  
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

  // подключаем MCP на адрес 1
  MCP.begin(1);

  // настраиваем "подхват питания"
  DBGLN(F("Setup power hook..."));
  MCP.pinMode(PWR_On_Out,OUTPUT);
  // Для поддержания нулевого уровня на затворе ключа в первую очередь необходимо установить нулевой уровень на выводе 5 MCP23017 
  MCP.digitalWrite(PWR_On_Out, HIGH);

  // настраиваем индикатор типа питания
  pinMode(PWR_On_In,INPUT);

  // смотрим, какое питание использовано - батарейное или USB?
  /*
   То есть при выполнении программы setup контролируем вывод PWR_On_In. Если на нем присутствует нулевой потенциал, 
   это означает подключен внутренний источник (батарейки), при высоком уровне - питание осуществляется от порта USB.
   */

  // проверяем тип питания
  checkPower();  

 // Постоянно контролируем состояние сигнала на выводе 38 (PWR_On_In)
 attachInterrupt(digitalPinToInterrupt(PWR_On_In), checkPower, CHANGE);
  
 
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

  // ничего не было сохранено - значит, надо сохранить время начала логгирования
  if(read32(LOGGING_DURATION_ADDRESS) < 1)
    saveLoggingTimer();
    
  // аккумулируем общее время логгирования
  accumulateLoggingDuration();


  attachInterrupt(RTC_ALARM_PIN, alarmFunction, FALLING); // прерывание вызывается только при смене значения на порту с LOW на HIGH

  // настраиваем будильник часов
  setupDS3231Alarm();
  
  analogSensorValue = 0;
  sensorsUpdateTimer = millis() + SENSORS_UPDATE_FREQUENCY;
  memset(&si7021Data,0,sizeof(si7021Data));
  
  si7021Data.temperature = NO_TEMPERATURE_DATA;
  si7021Data.humidity = NO_TEMPERATURE_DATA;
  

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
  

  // настраиваем управление питанием ESP
  MCP.pinMode(PWR_ESP,OUTPUT);

  // выключаем питание ESP
  espPower(false);


  // подключаем Si7021
  si7021.begin();
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::turnPowerOff()
{
    // сперва аккумулируем значение времени логгирования
    accumulateLoggingDuration();
  
   // потом выключаем питание контроллера
   MCP.digitalWrite(PWR_On_Out, LOW);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::checkPower()
{
  if(!digitalRead(PWR_On_In))
    Settings.powerType = batteryPower;
 else
   Settings.powerType = powerViaUSB;  
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

  // каждую 10 минут работы МК накапливаем значение общего времени логгирования
  static uint32_t lastMillis = 0;
  if(millis() - lastMillis > 600000)
  {
    accumulateLoggingDuration();
    lastMillis = millis();
  }
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::read32(uint16_t address)
{
  uint32_t result = 0;
  uint8_t* writePtr = (uint8_t*)&result;
  eeprom->read(address,writePtr,sizeof(uint32_t));

  if(result == 0xFFFFFFFF)
    result = 0;

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::write32(uint16_t address, uint32_t val)
{
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(address,writePtr,sizeof(uint32_t));  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
