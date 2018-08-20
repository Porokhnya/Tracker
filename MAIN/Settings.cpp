//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Settings.h"
#include "CONFIG.h"
#include "Buttons.h"
#include "DS3231.h"
#include "Logger.h"
#include "ScreenHAL.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SettingsClass Settings;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int SettingsClass::pressedKey = 0;
//uint8_t KNOWN_LOGGING_INTERVALS[ LOGGING_INTERVALS_COUNT ] = { LOGGING_INTERVALS };
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
  loggingInterval = 60;//LOGGING_INTERVAL_INDEX;
  bLoggingEnabled = true;
  bWantToLogFlag = false;
  bWantCheckAlarm = false;
  alarmTimer = 0;
  backlightFlag = true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef ESP_SUPPORT_ENABLED
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String SettingsClass::getStationID()
{
  String result = readString(STATION_ID_ADDRESS,20);
  if(!result.length())
    result = DEFAULT_STATION_ID;

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setStationID(const String& val)
{
  writeString(STATION_ID_ADDRESS,val,20);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String SettingsClass::getStationPassword()
{
  String result = readString(STATION_PASSWORD_ADDRESS,20);
  if(!result.length())
    result = DEFAULT_STATION_PASSWORD;

  return result;  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setStationPassword(const String& val)
{
  writeString(STATION_PASSWORD_ADDRESS,val,20);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String SettingsClass::getRouterID()
{
  String result = readString(ROUTER_ID_ADDRESS,20);
  if(!result.length())
    result = DEFAULT_ROUTER_ID;

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setRouterID(const String& val)
{
  writeString(ROUTER_ID_ADDRESS,val,20);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String SettingsClass::getRouterPassword()
{
  String result = readString(ROUTER_PASSWORD_ADDRESS,20);
  if(!result.length())
    result = DEFAULT_ROUTER_PASSWORD;

  return result;  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setRouterPassword(const String& val)
{
  writeString(ROUTER_PASSWORD_ADDRESS,val,20);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SettingsClass::shouldConnectToRouter()
{
  return getRouterID().length() && getRouterPassword().length();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::espPower(bool bOn)
{
  MCP.digitalWrite(PWR_ESP,bOn ? LOW : HIGH);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif // ESP_SUPPORT_ENABLED
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String SettingsClass::readString(uint16_t address, byte maxlength)
{
  String result;
  for(byte i=0;i<maxlength;i++)
  {
    byte b = read8(address++,0);
    if(b == 0)
      break;

    result += (char) b;
  }

  return result;  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::writeString(uint16_t address, const String& v, byte maxlength)
{

  for(byte i=0;i<maxlength;i++)
  {
    if(i >= v.length())
      break;
      
    write8(address++,v[i]);
  }

  // пишем завершающий ноль
  write8(address++,'\0');
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::write8(uint16_t address, uint8_t val)
{
  eeprom->write(address,val); 
}
//--------------------------------------------------------------------------------------------------------------------------------------
uint8_t SettingsClass::read8(uint16_t address, uint8_t defaultVal)
{
  uint8_t val = eeprom->read(address);
  if(val == 0xFF)
    val = defaultVal;

 return val;
}
//--------------------------------------------------------------------------------------------------------------------------------------
String SettingsClass::getUUID(const char* passedUUID)
{
    String savedUUID;
    uint16_t addr = UUID_STORE_ADDRESS;
    uint8_t header1 = eeprom->read(addr); addr++;
    uint8_t header2 = eeprom->read(addr); addr++;
    uint8_t header3 = eeprom->read(addr); addr++;

    if(!(header1 == RECORD_HEADER1 && header2 == RECORD_HEADER2 && header3 == RECORD_HEADER3))
    {
      savedUUID = passedUUID;

      addr = UUID_STORE_ADDRESS;
      eeprom->write(addr,RECORD_HEADER1); addr++;
      eeprom->write(addr,RECORD_HEADER2); addr++;
      eeprom->write(addr,RECORD_HEADER3); addr++;

      uint8_t written = 0;
      for(size_t i=0;i<savedUUID.length();i++)
      {
        eeprom->write(addr,savedUUID[i]); 
        addr++;
        written++;
      }

      for(int i=written;i<32;i++)
      {
         eeprom->write(addr,'\0'); 
         addr++;
      }

      return savedUUID;
    }

    // есть сохранённый GUID, читаем его
    for(int i=0;i<32;i++)
    {
      savedUUID += (char) eeprom->read(addr); addr++;
    }

    return savedUUID;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setLoggingDuration(uint32_t val)
{
  write32(LOGGING_DURATION_VALUE_ADDRESS, val);
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
  /*  
  if(loggingInterval >= LOGGING_INTERVALS_COUNT)
  {
    loggingInterval = 0;
    setLoggingIntervalIndex(loggingInterval);
  }

  return KNOWN_LOGGING_INTERVALS[loggingInterval];
  */
  return loggingInterval;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setLoggingInterval(uint8_t val)
{
  loggingInterval = val;

  eeprom->write(LOGGING_INTERVAL_ADDRESS, val);

  setupDS3231Alarm();  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
void SettingsClass::setLoggingIntervalIndex(uint8_t val)
{
  loggingInterval = val;

  eeprom->write(LOGGING_INTERVAL_ADDRESS, val);

  setupDS3231Alarm();
}
*/
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
  backlightFlag = bOn;
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
bool SettingsClass::isDoorOpen()
{
  return (doorState == DOOR_OPEN);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::begin()
{

  // подключаем MCP на адрес 1
  MCP.begin(1);


  // настраиваем индикатор типа питания
  pinMode(BUTTON_POWER,INPUT);
  digitalWrite(BUTTON_POWER, HIGH);  // Подключаем к кнопке включения питания подтягиваючий резистор 

  pinMode(LED, OUTPUT);              // Настрока светодиода индикации  
  digitalWrite(LED, LOW);

  // настраиваем концевик двери
  pinMode(DOOR_ENDSTOP_PIN, INPUT);
  // читаем его состояние
  doorState = digitalRead(DOOR_ENDSTOP_PIN);

  // вешаем на прерывание
  attachInterrupt(digitalPinToInterrupt(DOOR_ENDSTOP_PIN),doorStateChanged,CHANGE);


  // настраиваем "подхват питания"
  DBGLN(F("Setup power hook..."));
  MCP.pinMode(PWR_On_Out,OUTPUT);
  // Для поддержания нулевого уровня на затворе ключа в первую очередь необходимо установить нулевой уровень на выводе 5 MCP23017 
  MCP.digitalWrite(PWR_On_Out, HIGH);

  // проверяем тип питания
  checkPower();
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
  

  #ifdef ESP_SUPPORT_ENABLED
  
  // настраиваем управление питанием ESP
  MCP.pinMode(PWR_ESP,OUTPUT);
  // выключаем питание ESP
  espPower(false);
  
  #endif // ESP_SUPPORT_ENABLED

  // подключаем Si7021
  si7021.begin();


  powerButton.begin(BUTTON_POWER);
  
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
PowerType SettingsClass::getPowerType()
{
  return powerType;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::checkPower()
{
  if(!digitalRead(BUTTON_POWER))
  {
    Settings.powerType = batteryPower;
    DBGLN(F("BATTERY POWER !!!"));
	digitalWrite(LED, LOW);            // Индикация определения источника питания
  }
 else
 {
   Settings.powerType = powerViaUSB;  
    DBGLN(F("POWER  VIA USB !!!"));
	digitalWrite(LED, HIGH);            // Индикация определения источника питания
 }
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

  // увеличиваем кол-во измерений всего
  incTotalMeasures();
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::doorStateChanged()
{
  Settings.wantLogDoorState = true; 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::logDoorState()
{
     if(isLoggingEnabled())
     {
          // В состоянии включенной записи измерения температуры, при закрывании двери, записывается время закрытия.
          // В состоянии включенной записи измерения температуры и открывании двери, записывается время открывания.
          String action;
      
          if(isDoorOpen()) // сейчас дверь открыта, а была - закрыта, т.е. - было открывание
          {
            action = F("DOOR_OPENED");
          }
          else // сейчас зверь закрыта, а была - открыта, т.е. - было закрывание
          {
            action = F("DOOR_CLOSED");
          }

          // записываем событие в лог-файл
          String dataLine;
        
          // в первом столбце - дата/время логгирования
          DS3231Time tm = RealtimeClock.getTime();
          String dateStr = RealtimeClock.getDateStr(tm);
          dateStr += ' ';
          dateStr  += RealtimeClock.getTimeStr(tm);
        
          dataLine = Logger.formatCSV(dateStr);
          dataLine += COMMA_DELIMITER;
        
          // во втором столбце - действие с дверью
          dataLine += Logger.formatCSV(action);
          dataLine += COMMA_DELIMITER;   

          dataLine += NEWLINE;
          Logger.write((uint8_t*)dataLine.c_str(),dataLine.length());
          
     } // if(isLoggingEnabled())  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
void doPowerOff(void* param)
{
   Settings.turnPowerOff();
}
*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::update()
{ 
  // проверяем состояние концевика двери
  //uint8_t currentDoorState = digitalRead(DOOR_ENDSTOP_PIN);
  //if(doorState != currentDoorState)
  if(wantLogDoorState) // была смена уровня на концевике двери, надо записать это дело в лог-файл
  {
    wantLogDoorState = false;
    // состояние концевика двери изменилось
    doorState = digitalRead(DOOR_ENDSTOP_PIN);//currentDoorState;
    logDoorState();
  }
  
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


    powerButton.update();
    if(powerButton.isRetention())
    {
      DBGLN(F("POWER KEY DETECTED, TURN POWER OFF!!!"));
      Vector<const char*> lines;
      lines.push_back("Устройство");
      lines.push_back("готово");
      lines.push_back("к отключению.");
      MessageBox->show(lines,NULL);
      
      Settings.turnPowerOff();
    }
    /*
    if(powerButton.isClicked())
    {
     // DBGLN(F("POWER BUTTON CLICKED!!!"));
      
      // нажали кнопку отключения питания
      // закомментировал проверку типа питания, чтобы отключалось по-любому
      //if(getPowerType() == batteryPower)
      {
        static bool bFirst = true;

          if(!bFirst)
          {
            // питаемся от батареек, здесь можно выключать питание
            // но это нужно делать только после ВТОРОГО нажатия кнопки, т.к. первым - контроллер включается!

            DBGLN(F("POWER KEY DETECTED, TURN POWER OFF!!!"));
        
            // TODO: ТУТ СДЕЛАТЬ ТО, ЧТО НАДО. СЕЙЧАС РУБИТСЯ ПИТАНИЕ после настраиваемой задержки.
            CoreDelayedEvent.raise(TURN_POWER_OFF_DELAY,doPowerOff,NULL);
          }
          bFirst = false;
      }
       
    }  // if(powerButton.isClicked())
      */
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::getTotalMeasures()
{
  return read32(TOTAL_MEASURES_ADDRESS);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::incTotalMeasures()
{
  uint32_t val = getTotalMeasures();
  val++;
  write32(TOTAL_MEASURES_ADDRESS,val);
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
uint16_t SettingsClass::read16(uint16_t address)
{
  uint16_t result = 0;
  uint8_t* writePtr = (uint8_t*)&result;
  eeprom->read(address,writePtr,sizeof(uint16_t));

  if(result == 0xFFFF)
    result = 0;

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::write16(uint16_t address, uint16_t val)
{
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(address,writePtr,sizeof(uint16_t));  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

