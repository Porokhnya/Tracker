//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Settings.h"
#include "CONFIG.h"
#include "Buttons.h"
#include "DS3231.h"
#include "Logger.h"
#include "ScreenHAL.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// URI utils
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void hexchar(unsigned char c, unsigned char &hex1, unsigned char &hex2)
{
    hex1 = c / 16;
    hex2 = c % 16;
    hex1 += hex1 <= 9 ? '0' : 'a' - 10;
    hex2 += hex2 <= 9 ? '0' : 'a' - 10;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String urlencode(const String& s)
{
    const char *str = s.c_str();
    String result;
    for (size_t i = 0, l = s.length(); i < l; i++)
    {
        char c = str[i];
        if ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            c == '-' || c == '_' || c == '.' || c == '!' || c == '~' ||
            c == '*' || c == '\'' || c == '(' || c == ')')
        {
            result += c;
        }
        else if (c == ' ')
        {
            result += '+';
        }
        else
        {
            result += '%';
            unsigned char d1, d2;
            hexchar(c, d1, d2);
            result += char(d1);
            result += char(d2);
        }
    }

    return result;
}
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
  analogSensorTemperature.value = NO_TEMPERATURE_DATA;
  loggingInterval = 60;
  bLoggingEnabled = true;
  bWantToLogFlag = false;
  bWantCheckAlarm = false;
  alarmTimer = 0;
  backlightFlag = true;
  startProcessWiFiFlag = false;
  esp = NULL;
  httpQuery = NULL;

  wifiTimer = 0;
}
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
      // создаём новый лог-файл на сегодня
      Logger.newLogFile();
      // выставляем таймер начала логгирования на текущее значение unixtime
      saveLoggingTimer();

      // сбрасываем кол-во измерений за сессию
      resetSessionMeasures();      

      // записываем 0 как общее время логгирования, карточка "Время подсчета лога должно сбрасываться при открытии нового файла лога, а не считать общее время всех логов."
      setLoggingDuration(0);
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

      // сохраняем накопленное значение
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
uint32_t SettingsClass::getWiFiSendInterval()
{
  return read32(WIFI_SEND_INTERVAL_ADDRESS,WIFI_SEND_INTERVAL);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setWiFiSendInterval(uint32_t val)
{
  write32(WIFI_SEND_INTERVAL_ADDRESS,val);
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


  // читаем текущий интервал отсыла показаний по Wi-Fi
   wifiInterval = getWiFiSendInterval();


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
  
  analogSensorTemperature.value = NO_TEMPERATURE_DATA;
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
    float rawADC = analogRead(ANALOG_SENSOR_PIN);
    int32_t adcTemp = 100.0 * ((rawADC * 1.0)/ADC_COEFF);
    analogSensorTemperature.value = adcTemp/100;
    analogSensorTemperature.fract = abs(adcTemp%100);
    

    float t = si7021.readTemperature()*100.0;
    int32_t iT = t;
    si7021Data.temperature = iT/100;
    si7021Data.temperatureFract = abs(iT%100);

    t = si7021.readHumidity()*100.0;
    iT = t;
    si7021Data.humidity = iT/100;
    si7021Data.humidityFract = iT%100;  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::checkIsAlarm()
{

  // вызывается каждую минуту
  
  // сперва очищаем будильник по-любому
  RealtimeClock.clearAlarm2();

  // проверяем, что у нас там с Wi-Fi
  if(wifiState == wifiWaitInterval)
  {
    // ничего с Wi-Fi не делаем, ждём истечения интервала до следующего отсыла
    wifiTimer++;
    if(wifiTimer >= wifiInterval)
    {
      DBGLN(F("WANT TO SEND DATA TO WI-FI!"));
      wifiTimer = 0;
      wifiState = wifiWaitConnection;
      startProcessWiFiFlag = true;
      
    }
  }

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
bool SettingsClass::isTemperatureInsideBorders()
{
  int16_t minTempBorder = getMinTempBorder();
  minTempBorder *= 100;

  int16_t maxTempBorder = getMaxTempBorder();
  maxTempBorder *= 100;

  int16_t toCompare = si7021Data.temperature;
  toCompare *= 100;
  if(toCompare < 0)
    toCompare -= si7021Data.temperatureFract;
  else
    toCompare += si7021Data.temperatureFract;

  return (toCompare >= minTempBorder && toCompare <= maxTempBorder);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SettingsClass::isHumidityInsideBorders()
{
  uint16_t minHumidityBorder = getMinHumidityBorder();
  minHumidityBorder *= 100;
  
  uint16_t maxHumidityBorder = getMaxHumidityBorder();
  maxHumidityBorder *= 100;

  uint16_t toCompareHum = si7021Data.humidity;
  toCompareHum *= 100;
  toCompareHum += si7021Data.humidityFract;

  return (toCompareHum >= minHumidityBorder && toCompareHum <= maxHumidityBorder);
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SettingsClass::isADCInsideBorders()
{
  uint16_t minADCBorder = getMinADCBorder();
  uint16_t maxADCBorder = getMaxADCBorder();
  return (analogSensorTemperature.value >= minADCBorder && analogSensorTemperature.value <= maxADCBorder);
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


  String okFlag;

  if(isADCInsideBorders())
  {
    okFlag = "GOOD";    
  }
  else
  {
    okFlag = "BAD";
  }
  
  // во втором столбце - показания аналогового датчика
  String sTemp = String(analogSensorTemperature.value);
  sTemp += DECIMAL_SEPARATOR;
  if(analogSensorTemperature.fract < 10)
    sTemp += '0';
  sTemp += analogSensorTemperature.fract;
  dataLine += Logger.formatCSV(sTemp);
  dataLine += COMMA_DELIMITER;

  dataLine += Logger.formatCSV(okFlag);
  dataLine += COMMA_DELIMITER;


  // в третьем столбце - температура с Si7021
  sTemp = String(si7021Data.temperature);
  sTemp += DECIMAL_SEPARATOR;
  if(si7021Data.temperatureFract < 10)
    sTemp += '0';
  sTemp += si7021Data.temperatureFract;
  dataLine += Logger.formatCSV(sTemp);
  dataLine += COMMA_DELIMITER;


  if(isTemperatureInsideBorders())
  {
    okFlag = "GOOD";    
  }
  else
  {
    okFlag = "BAD";
  }

  dataLine += Logger.formatCSV(okFlag);
  dataLine += COMMA_DELIMITER;

  // в четвертом столбце - влажность с Si7021
  sTemp = String(si7021Data.humidity);
  sTemp += DECIMAL_SEPARATOR;
  if(si7021Data.humidityFract < 10)
    sTemp += '0';
  sTemp += si7021Data.humidityFract;
  dataLine += Logger.formatCSV(sTemp);
  dataLine += COMMA_DELIMITER;

  if(isHumidityInsideBorders())
  {
    okFlag = "GOOD";    
  }
  else
  {
    okFlag = "BAD";
  }
  
  dataLine += Logger.formatCSV(okFlag);
  dataLine += COMMA_DELIMITER;      
  

  //TODO: ОСТАЛЬНЫЕ ДАННЫЕ ДЛЯ ЛОГА - ЗДЕСЬ !!!


  dataLine += NEWLINE;
  Logger.write((uint8_t*)dataLine.c_str(),dataLine.length());

  // увеличиваем кол-во измерений всего
  incTotalMeasures();

  // увеличиваем кол-во измерений за сессию
  incSessionMeasures();
    
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
void SettingsClass::switchToESPWaitMode()
{
  wifiTimer = 0;
  esp->unsubscribe(this);
  esp->Destroy();
  esp = NULL;
  wifiState = wifiWaitInterval;
  itemsInPacket = 0;       
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::update()
{ 
  // проверяем состояние концевика двери
  if(wantLogDoorState) // была смена уровня на концевике двери, надо записать это дело в лог-файл
  {
    wantLogDoorState = false;
    // состояние концевика двери изменилось
    doorState = digitalRead(DOOR_ENDSTOP_PIN);
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
    

  // проверяем, надо ли начать отсылать данные по Wi-Fi
  if(startProcessWiFiFlag)
  {
    DBGLN(F("Start process Wi-Fi!"));
    startProcessWiFiFlag = false;
    if(!esp)
    {
      DBGLN(F("Create ESP transport."));
      esp = CoreESPTransport::Create();
      // подписываемся на события клиентов
      esp->subscribe(this);
    }
    wifiTimer = millis(); // запоминаем время начала работы с Wi-Fi
  }

  // тут смотрим - какое состояние работы с Wi-Fi сейчас?
  switch(wifiState)
  {
    case   wifiWaitInterval:// ждём наступления интервала до отсыла
    {
      
    }
    break; // wifiWaitInterval
    
    case wifiWaitConnection: // ждём установки соединения с устройством
    {
      if(esp->ready())
      {
        if(esp->isConnectedToRouter())
        {
            // ESP готов к отсылу данных
            DBGLN(F("ESP ready for send data, get remote IP..."));
    
            // для начала - получаем IP
            String staIP;
            if(esp->getIP(remoteIP,staIP))
            {
              DBG(F("Remote IP is: "));
              DBGLN(remoteIP);

              #ifdef WIFI_DEBUG_CONNECT_TO
                DBG(F("Connection redefined to: "));
                DBGLN(WIFI_DEBUG_CONNECT_TO);
    
                remoteIP = WIFI_DEBUG_CONNECT_TO;
              #endif

              // переключаемся на отсыл данных
              wifiState = wifiSendData;

              // собираем данные с датчиков и помещаем их в очередь
              updateDataFromSensors();
              pushSensorsDataToWiFiQueue();
              
            }          
          else
          {
            // ничего не получилось, не можем продолжать
            DBGLN(F("Unable to request remote IP, switch to wait mode!"));
            switchToESPWaitMode();
          } // else
          
        } // if(esp->isConnectedToRouter())
        else
        {
          DBGLN(F("ESP not connected to endpoint, switch to wait mode..."));
          switchToESPWaitMode();
        }

      } // ready()
      else
      {
        // ESP не готов принимать данные, проверяем - не слишком ли долго мы ждём?
        if(millis() - wifiTimer > WIFI_CONNECT_TIMEOUT)
        {
          // всё, не удалось соединиться - убиваем ESP, переходим на режим ожидания следующего интервала
          DBGLN(F("Connect timeout, can't send data over Wi-Fi, switch to wait mode..."));
          switchToESPWaitMode();
        }
      }
    }
    break; // wifiWaitConnection
  
    case wifiSendData: // отсылаем данные на устройство
    {
      // отсылаем следующий пакет
      sendWiFiDataPacket();
    }
    break; // wifiSendData

    case wifiWaitClientConnected:
    {
      if(millis() - wifiTimer > WIFI_CONNECT_TIMEOUT)
      {
          DBGLN(F("Client connect timeout, can't send data over Wi-Fi, switch to wait mode..."));
          switchToESPWaitMode();        
      }
    }
    break;
    
    case wifiWaitSendDataDone: // ждём окончания отсыла данных
    {
      if(millis() - wifiTimer > WIFI_WRITE_TIMEOUT)
      {
          DBGLN(F("Client write timeout, can't send data over Wi-Fi, switch to wait mode..."));
          switchToESPWaitMode();        
      }
      
    }
    break; // wifiWaitSendDataDone
    
  } // switch
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::pushSensorsDataToWiFiQueue()
{

  while(wifiData.size() > MAX_WIFI_LIST_RECORDS)
  {
    // удаляем старые записи
    WiFiReportItem ri = wifiData[0];
    delete ri.formattedData;
    
    for(size_t i=1;i<wifiData.size();i++)
    {
      wifiData[i-1] = wifiData[i];
    }
    wifiData.pop();
  }
  
  DS3231Time tm = RealtimeClock.getTime();


  // помещаем в очередь показания температуры
  WiFiReportItem temperatureItem;
  temperatureItem.dataType = dataSi7021Temperature;
  temperatureItem.checkpoint = tm;
  temperatureItem.insideBordersFlag = isTemperatureInsideBorders();
  temperatureItem.formattedData = new String();
  *(temperatureItem.formattedData) += si7021Data.temperature;
  *(temperatureItem.formattedData) += DECIMAL_SEPARATOR;
  if(si7021Data.temperatureFract < 10)
    *(temperatureItem.formattedData) += '0';
  *(temperatureItem.formattedData) += si7021Data.temperatureFract;

  wifiData.push_back(temperatureItem);

  // помещаем в очередь показания влажности
  WiFiReportItem humidityItem;
  humidityItem.dataType = dataSi7021Humidity;
  humidityItem.checkpoint = tm;
  humidityItem.insideBordersFlag = isHumidityInsideBorders();
  humidityItem.formattedData = new String();
  *(humidityItem.formattedData) += si7021Data.humidity;
  *(humidityItem.formattedData) += DECIMAL_SEPARATOR;
  if(si7021Data.humidityFract < 10)
    *(humidityItem.formattedData) += '0';
  *(humidityItem.formattedData) += si7021Data.humidityFract;

  wifiData.push_back(humidityItem);

  // помещаем в очередь показания АЦП
  WiFiReportItem adcItem;
  adcItem.dataType = dataADC;
  adcItem.checkpoint = tm;
  adcItem.insideBordersFlag = isADCInsideBorders();
  adcItem.formattedData = new String();
  *(adcItem.formattedData) += analogSensorTemperature.value;
  *(adcItem.formattedData) += DECIMAL_SEPARATOR;
  if(analogSensorTemperature.fract < 10)
    *(adcItem.formattedData) += '0';
  *(adcItem.formattedData) += analogSensorTemperature.fract;
  
  wifiData.push_back(adcItem);

  // помещаем в очередь состояние двери
  WiFiReportItem doorStateItem;
  doorStateItem.dataType = dataDoorState;
  doorStateItem.checkpoint = tm;
  doorStateItem.insideBordersFlag = true;
  doorStateItem.formattedData = new String();
  *(doorStateItem.formattedData) = isDoorOpen() ? "OPEN" : "CLOSE";

  wifiData.push_back(doorStateItem);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::sendWiFiDataPacket()
{
  //Тут пытаемся отослать данные
  DBG(F("Make connection to "));
  DBG(remoteIP);
  DBGLN(F(" at port 80..."));

  wifiTimer = millis();
  wifiState = wifiWaitClientConnected;

  // говорим, что клиент будет работать через ESP
  espClient.accept(esp);
  espClient.connect(remoteIP.c_str(),80);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::OnClientConnect(CoreTransportClient& client, bool connected, int16_t errorCode)
{
  DBGLN(F("OnClientConnect..."));
  if(!connected)
  {
    // отсоединились или ошибка соединения!
    if(errorCode != CT_ERROR_NONE)
    {
      // ошибка соединения
      DBGLN(F("Connection error, switch to wait mode!"));
      switchToESPWaitMode();
    }
    else
    {
      if(anyWriteError)
      {
        DBGLN(F("Write error detected, switch to wait mode!"));
        switchToESPWaitMode();
      } // anyWriteError
      else
      {
          // всё нормально, отсоединились
          DBGLN(F("Disconnected normally, check for next packet..."));
    
          // очищаем уже отосланные данные
          removeSentData();
          
          if(wifiData.size())
          {
            DBGLN(F("Has some data left, transmit next packet!"));
            wifiState = wifiSendData;
          }
          else
          {
            // уже нет данных
            DBGLN(F("All data was sent, switch to wait mode..."));
            switchToESPWaitMode();
          }
      } // else not anyWriteError
    } // else
  } // if(!connected)
  else
  {
    DBGLN(F("Connected, make query and send it..."));
    
    makeHTTPQuery();
    
    // соединились, можно отсылать данные
    // не забываем переключаться в режим ожидания окончания отсыла данных
    wifiTimer = millis();
    wifiState = wifiWaitSendDataDone;
    anyWriteError = false;
    espClient.write((uint8_t*) httpQuery->c_str(),httpQuery->length());

    // чистим за собой
    delete httpQuery;
    httpQuery = NULL;
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::OnClientDataWritten(CoreTransportClient& client, int16_t errorCode)
{
  DBGLN(F("OnClientDataWritten..."));
  if(errorCode == CT_ERROR_NONE)
  {
    // всё нормально записано
    DBGLN(F("Data was written."));
  }
  else
  {
    // не удалось записать в клиент
    DBGLN(F("Data write ERROR, disconnect and switch to wait mode!"));
    anyWriteError = true;
  }
    espClient.disconnect();
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::OnClientDataAvailable(CoreTransportClient& client, uint8_t* data, size_t dataSize, bool isDone)
{
  DBGLN(F("OnClientDataAvailable..."));

    #ifdef _DEBUG
      Serial.write(data,dataSize);
    #endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::removeSentData()
{
  size_t to = min(itemsInPacket,wifiData.size());

  for(size_t i=0;i<to;i++)
  {
    delete wifiData[i].formattedData;
  }

  // почистили N записей, теперь переносим неочищенные записи в голову
  WiFiReportList lst;
  for(size_t i=to;i<wifiData.size();i++)
  {
    lst.push_back(wifiData[i]);
  }

  wifiData = lst;
  // не забываем сказать, что мы в этой итерации ещё ничего не отсылали
  itemsInPacket = 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::makeHTTPQuery()
{
  DBGLN(F("Make HTTP-query..."));
  
  // создаём HTTP-запрос
  delete httpQuery;
  httpQuery = new String();
  
  *httpQuery = F("POST ");
  *httpQuery += HTTP_HANDLER;
  *httpQuery += F(" HTTP/1.1\r\nConnection: Keep-Alive\r\nContent-Type: application/x-www-form-urlencoded\r\n");

  #ifdef HTTP_HOST
    *httpQuery += F("Host: ");
    *httpQuery += HTTP_HOST;
    *httpQuery += F("\r\n");
  #endif
  
  *httpQuery += F("Content-Length: ");

  String httpData;

  DS3231Time tm = RealtimeClock.getTime();
  uint32_t ut = tm.unixtime();
  String dateStr = RealtimeClock.getDateStr(tm);
  dateStr += ' ';
  dateStr += RealtimeClock.getTimeStr(tm);

  // передаём общую метку времени для пакета
  httpData += F("tm=");
  httpData += urlencode(dateStr);

  // затем - собираем N показаний из очереди.
  // если показание вышло за границы и его метка времени не совпадает с текущей - выставляем отдельную метку времени.
  // по окончании - не забываем, на каком индексе остановились
  itemsInPacket = min(WIFI_MAX_RECORDS_IN_ONE_PACKET,wifiData.size());

  // добавляем кол-во записей в пакете
  httpData += F("&count=");
  httpData += itemsInPacket;

  // формируем данные пакета
  for(size_t i=0;i<itemsInPacket;i++)
  {
    httpData += F("&type");
    httpData += i;
    httpData += '=';
    httpData += (int) wifiData[i].dataType;

    // проверяем, совпадают ли временные метки
    if(ut != wifiData[i].checkpoint.unixtime())
    {
      // метка не совпадает, добавляем метку записи
      httpData += F("&rtm");
      httpData += i;
      httpData += '=';

      dateStr = RealtimeClock.getDateStr(wifiData[i].checkpoint);
      dateStr += ' ';
      dateStr += RealtimeClock.getTimeStr(wifiData[i].checkpoint);

      httpData += urlencode(dateStr);
    }

      // добавляем флаг соответствия порогам
      httpData += F("&state");
      httpData += i;
      httpData += '=';
      httpData += wifiData[i].insideBordersFlag ? '1' : '0';

      // добавляем заранее сформатированные данные
      httpData += F("&data");
      httpData += i;
      httpData += '=';
      httpData += urlencode(*(wifiData[i].formattedData));
    
  } // for

  // подсчитываем длину данных и пишем в заголовок
  *httpQuery += httpData.length();
  // две пустые строки в конце
  *httpQuery += F("\r\n\r\n");
  // потом - данные
  *httpQuery += httpData;

  DBGLN(F("HTTP query is:"));
  DBGLN(*httpQuery);
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int8_t SettingsClass::getMinTempBorder()
{
  return read8(TEMP_MIN_BORDER_ADDRESS,TEMPERATURE_MIN_BORDER_DEFAULT_VAL);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setMinTempBorder(int8_t val)
{
  write8(TEMP_MIN_BORDER_ADDRESS,val);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int8_t SettingsClass::getMaxTempBorder()
{
  return read8(TEMP_MAX_BORDER_ADDRESS,TEMPERATURE_MAX_BORDER_DEFAULT_VAL); 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setMaxTempBorder(int8_t val)
{
  write8(TEMP_MAX_BORDER_ADDRESS,val);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t SettingsClass::getMinHumidityBorder()
{
  return read8(HUMIDITY_MIN_BORDER_ADDRESS,HUMIDITY_MIN_BORDER_DEFAULT_VAL);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setMinHumidityBorder(uint8_t val)
{
  write8(HUMIDITY_MIN_BORDER_ADDRESS,val); 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t SettingsClass::getMaxHumidityBorder()
{
  return read8(HUMIDITY_MAX_BORDER_ADDRESS,HUMIDITY_MAX_BORDER_DEFAULT_VAL);  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setMaxHumidityBorder(uint8_t val)
{
  write8(HUMIDITY_MAX_BORDER_ADDRESS,val);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t SettingsClass::getMinADCBorder()
{
  return read16(ADC_MIN_BORDER_ADDRESS,ADC_MIN_BORDER_DEFAULT_VAL);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setMinADCBorder(uint16_t val)
{
  write16(ADC_MIN_BORDER_ADDRESS,val);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t SettingsClass::getMaxADCBorder()
{
  return read16(ADC_MAX_BORDER_ADDRESS,ADC_MAX_BORDER_DEFAULT_VAL);  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setMaxADCBorder(uint16_t val)
{
  write16(ADC_MAX_BORDER_ADDRESS,val);  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::getSessionMeasures()
{
  return read32(SESSION_MEASURES_ADDRESS);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::incSessionMeasures()
{
  uint32_t val = getSessionMeasures();
  val++;
  write32(SESSION_MEASURES_ADDRESS,val);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::resetSessionMeasures()
{
  write32(SESSION_MEASURES_ADDRESS,0);
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
String SettingsClass::getDriver()
{
  return readString(DRIVER_ADDRESS,71);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String SettingsClass::getVehicleNumber()
{
  return readString(VEHICLE_NUMBER_ADDRESS,11);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String SettingsClass::getTrailerNumber()
{
  return readString(TRAILER_NUMBER_ADDRESS,11);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::read32(uint16_t address, uint32_t defaultVal)
{
  uint32_t result = 0;
  uint8_t* writePtr = (uint8_t*)&result;
  eeprom->read(address,writePtr,sizeof(uint32_t));

  if(result == 0xFFFFFFFF)
    result = defaultVal;

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::write32(uint16_t address, uint32_t val)
{
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(address,writePtr,sizeof(uint32_t));  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t SettingsClass::read16(uint16_t address, uint16_t defaultVal)
{
  uint16_t result = 0;
  uint8_t* writePtr = (uint8_t*)&result;
  eeprom->read(address,writePtr,sizeof(uint16_t));

  if(result == 0xFFFF)
    result = defaultVal;

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::write16(uint16_t address, uint16_t val)
{
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(address,writePtr,sizeof(uint16_t));  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

