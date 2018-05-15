#include "DS3231.h"
#include "CONFIG.h"
//--------------------------------------------------------------------------------------------------------------------------------------
char DS3231Clock::workBuff[12] = {0};
DS3231Clock RealtimeClock;
//--------------------------------------------------------------------------------------------------------------------------------------
const uint8_t daysArray [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };
//--------------------------------------------------------------------------------------------------------------------------------------
uint32_t DS3231Time::unixtime(void)
{
  uint32_t u;

  u = time2long(date2days(this->year, this->month, this->dayOfMonth), this->hour, this->minute, this->second);
  u += 946684800; // + 01.01.2000 00:00:00 unixtime

  return u;
}
//--------------------------------------------------------------------------------------------------------------------------------------
DS3231Time DS3231Time::addDays(long days)
{
  uint32_t ut = unixtime();
  long diff = days*86400;
  ut += diff;  
  return fromUnixtime(ut);
}
//--------------------------------------------------------------------------------------------------------------------------------------
DS3231Time DS3231Time::fromUnixtime(uint32_t time)
{
  DS3231Time result;

  uint8_t year;
  uint8_t month, monthLength;
  unsigned long days;

  result.second = time % 60;
  time /= 60; // now it is minutes
  result.minute = time % 60;
  time /= 60; // now it is hours
  result.hour = time % 24;
  time /= 24; // now it is days
  
  year = 0;  
  days = 0;
  while((unsigned)(days += (isLeapYear(year) ? 366 : 365)) <= time) {
    year++;
  }
  result.year = year + 1970; // year is offset from 1970 
  
  days -= isLeapYear(year) ? 366 : 365;
  time  -= days; // now it is days in this year, starting at 0
  
  days=0;
  month=0;
  monthLength=0;
  for (month=0; month<12; month++) 
  {
    if (month==1) 
    { // february
      if (isLeapYear(year)) 
      {
        monthLength=29;
      } 
      else 
      {
        monthLength=28;
      }
    } 
    else 
    {
      monthLength = pgm_read_byte(daysArray + month); //monthDays[month];
    }
    
    if (time >= monthLength) 
    {
      time -= monthLength;
    } 
    else 
    {
        break;
    }
  }
  result.month = month + 1;  // jan is month 1  
  result.dayOfMonth = time + 1;     // day of month  

    int dow;
    byte mArr[12] = {6,2,2,5,0,3,5,1,4,6,2,4};
    dow = (result.year % 100);
    dow = dow*1.25;
    dow += result.dayOfMonth;
    dow += mArr[result.month-1];
    
    if (isLeapYear(result.year) && (result.month<3))
     dow -= 1;
     
    while (dow>7)
     dow -= 7;

   result.dayOfWeek = dow;

  return result;
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool DS3231Time::isLeapYear(uint16_t year)
{
  return (year % 4 == 0);
}
//--------------------------------------------------------------------------------------------------------------------------------------
uint16_t DS3231Time::date2days(uint16_t _year, uint8_t _month, uint8_t _day)
{
    _year = _year - 2000;

    uint16_t days16 = _day;

    for (uint8_t i = 1; i < _month; ++i)
    {
        days16 += pgm_read_byte(daysArray + i - 1);
    }

    if ((_month == 2) && isLeapYear(_year))
    {
        ++days16;
    }

    return days16 + 365 * _year + (_year + 3) / 4 - 1;
}
//--------------------------------------------------------------------------------------------------------------------------------------
long DS3231Time::time2long(uint16_t days, uint8_t hours, uint8_t minutes, uint8_t seconds)
{    
    return ((days * 24L + hours) * 60 + minutes) * 60 + seconds;
}
//--------------------------------------------------------------------------------------------------------------------------------------
DS3231Clock::DS3231Clock()
{
  wire = &Wire;
}
//--------------------------------------------------------------------------------------------------------------------------------------
RTCAlarmTime DS3231Clock::getAlarm1(void)
{
    uint8_t values[4];
    RTCAlarmTime a;

    wire->beginTransmission(DS3231Address);
    #if ARDUINO >= 100
        wire->write(DS3231_REG_ALARM_1);
    #else
        wire->send(DS3231_REG_ALARM_1);
    #endif
    wire->endTransmission();

    wire->requestFrom(DS3231Address, 4);

    while(!wire->available()) {};

    for (int i = 3; i >= 0; i--)
    {
        #if ARDUINO >= 100
            values[i] = bcd2dec(wire->read() & 0b01111111);
        #else
            values[i] = bcd2dec(wire->receive() & 0b01111111);
        #endif
    }

    wire->endTransmission();

    a.day = values[0];
    a.hour = values[1];
    a.minute = values[2];
    a.second = values[3];

    return a;
}
//--------------------------------------------------------------------------------------------------------------------------------------
DS3231_alarm1_t DS3231Clock::getAlarmType1(void)
{
    uint8_t values[4];
    uint8_t mode = 0;

    wire->beginTransmission(DS3231Address);
    #if ARDUINO >= 100
        wire->write(DS3231_REG_ALARM_1);
    #else
        wire->send(DS3231_REG_ALARM_1);
    #endif
    wire->endTransmission();

    wire->requestFrom(DS3231Address, 4);

    while(!wire->available()) {};

    for (int i = 3; i >= 0; i--)
    {
        #if ARDUINO >= 100
            values[i] = bcd2dec(wire->read());
        #else
            values[i] = bcd2dec(wire->receive());
        #endif
    }

    wire->endTransmission();

    mode |= ((values[3] & 0b01000000) >> 6);
    mode |= ((values[2] & 0b01000000) >> 5);
    mode |= ((values[1] & 0b01000000) >> 4);
    mode |= ((values[0] & 0b01000000) >> 3);
    mode |= ((values[0] & 0b00100000) >> 1);

    return (DS3231_alarm1_t)mode;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void DS3231Clock::setAlarm1(uint8_t dydw, uint8_t hour, uint8_t minute, uint8_t second, DS3231_alarm1_t mode, bool armed)
{
    second = dec2bcd(second);
    minute = dec2bcd(minute);
    hour = dec2bcd(hour);
    dydw = dec2bcd(dydw);

    switch(mode)
    {
        case DS3231_EVERY_SECOND:
            second |= 0b10000000;
            minute |= 0b10000000;
            hour |= 0b10000000;
            dydw |= 0b10000000;
            break;

        case DS3231_MATCH_S:
            second &= 0b01111111;
            minute |= 0b10000000;
            hour |= 0b10000000;
            dydw |= 0b10000000;
            break;

        case DS3231_MATCH_M_S:
            second &= 0b01111111;
            minute &= 0b01111111;
            hour |= 0b10000000;
            dydw |= 0b10000000;
            break;

        case DS3231_MATCH_H_M_S:
            second &= 0b01111111;
            minute &= 0b01111111;
            hour &= 0b01111111;
            dydw |= 0b10000000;
            break;

        case DS3231_MATCH_DT_H_M_S:
            second &= 0b01111111;
            minute &= 0b01111111;
            hour &= 0b01111111;
            dydw &= 0b01111111;
            break;

        case DS3231_MATCH_DY_H_M_S:
            second &= 0b01111111;
            minute &= 0b01111111;
            hour &= 0b01111111;
            dydw &= 0b01111111;
            dydw |= 0b01000000;
            break;
    }

    wire->beginTransmission(DS3231Address);
    #if ARDUINO >= 100
        wire->write(DS3231_REG_ALARM_1);
        wire->write(second);
        wire->write(minute);
        wire->write(hour);
        wire->write(dydw);
    #else
        wire->send(DS3231_REG_ALARM_1);
        wire->send(second);
        wire->send(minute);
        wire->send(hour);
        wire->send(dydw);
    #endif

    wire->endTransmission();

    armAlarm1(armed);

    clearAlarm1();
}
//-------------------------------------------------------------------------------------------------------------------------------------
bool DS3231Clock::isAlarm1(bool clear)
{
    uint8_t alarm;

    alarm = readRegister8(DS3231_REG_STATUS);
    alarm &= 0b00000001;

    if (alarm && clear)
    {
        clearAlarm1();
    }

    return alarm;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DS3231Clock::armAlarm1(bool armed)
{
    uint8_t value;
    value = readRegister8(DS3231_REG_CONTROL);

    if (armed)
    {
        value |= 0b00000001;
    } else
    {
        value &= 0b11111110;
    }

    writeRegister8(DS3231_REG_CONTROL, value);
}
//-------------------------------------------------------------------------------------------------------------------------------------
bool DS3231Clock::isArmed1(void)
{
    uint8_t value;
    value = readRegister8(DS3231_REG_CONTROL);
    value &= 0b00000001;
    return value;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DS3231Clock::clearAlarm1(void)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_STATUS);
    value &= 0b11111110;

    writeRegister8(DS3231_REG_STATUS, value);
}
//-------------------------------------------------------------------------------------------------------------------------------------
RTCAlarmTime DS3231Clock::getAlarm2(void)
{
    uint8_t values[3];
    RTCAlarmTime a;

    wire->beginTransmission(DS3231Address);
    #if ARDUINO >= 100
        wire->write(DS3231_REG_ALARM_2);
    #else
        wire->send(DS3231_REG_ALARM_2);
    #endif
    wire->endTransmission();

    wire->requestFrom(DS3231Address, 3);

    while(!wire->available()) {};

    for (int i = 2; i >= 0; i--)
    {
        #if ARDUINO >= 100
            values[i] = bcd2dec(wire->read() & 0b01111111);
        #else
            values[i] = bcd2dec(wire->receive() & 0b01111111);
        #endif
    }

    wire->endTransmission();

    a.day = values[0];
    a.hour = values[1];
    a.minute = values[2];
    a.second = 0;

    return a;
}
//-------------------------------------------------------------------------------------------------------------------------------------
DS3231_alarm2_t DS3231Clock::getAlarmType2(void)
{
    uint8_t values[3];
    uint8_t mode = 0;

    wire->beginTransmission(DS3231Address);
    #if ARDUINO >= 100
        wire->write(DS3231_REG_ALARM_2);
    #else
        wire->send(DS3231_REG_ALARM_2);
    #endif
    wire->endTransmission();

    wire->requestFrom(DS3231Address, 3);

    while(!wire->available()) {};

    for (int i = 2; i >= 0; i--)
    {
        #if ARDUINO >= 100
            values[i] = bcd2dec(wire->read());
        #else
            values[i] = bcd2dec(wire->receive());
        #endif
    }

    wire->endTransmission();

    mode |= ((values[2] & 0b01000000) >> 5);
    mode |= ((values[1] & 0b01000000) >> 4);
    mode |= ((values[0] & 0b01000000) >> 3);
    mode |= ((values[0] & 0b00100000) >> 1);

    return (DS3231_alarm2_t)mode;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DS3231Clock::setAlarm2(uint8_t dydw, uint8_t hour, uint8_t minute, DS3231_alarm2_t mode, bool armed)
{
    minute = dec2bcd(minute);
    hour = dec2bcd(hour);
    dydw = dec2bcd(dydw);

    switch(mode)
    {
        case DS3231_EVERY_MINUTE:
            minute |= 0b10000000;
            hour |= 0b10000000;
            dydw |= 0b10000000;
            break;

        case DS3231_MATCH_M:
            minute &= 0b01111111;
            hour |= 0b10000000;
            dydw |= 0b10000000;
            break;

        case DS3231_MATCH_H_M:
            minute &= 0b01111111;
            hour &= 0b01111111;
            dydw |= 0b10000000;
            break;

        case DS3231_MATCH_DT_H_M:
            minute &= 0b01111111;
            hour &= 0b01111111;
            dydw &= 0b01111111;
            break;

        case DS3231_MATCH_DY_H_M:
            minute &= 0b01111111;
            hour &= 0b01111111;
            dydw &= 0b01111111;
            dydw |= 0b01000000;
            break;
    }

   wire->beginTransmission(DS3231Address);
    #if ARDUINO >= 100
        wire->write(DS3231_REG_ALARM_2);
        wire->write(minute);
        wire->write(hour);
        wire->write(dydw);
    #else
        wire->send(DS3231_REG_ALARM_2);
        wire->send(minute);
        wire->send(hour);
        wire->send(dydw);
    #endif

    wire->endTransmission();

    armAlarm2(armed);

    clearAlarm2();
}
//--------------------------------------------------------------------------------------------------------------------------------------
void DS3231Clock::armAlarm2(bool armed)
{
    uint8_t value;
    value = readRegister8(DS3231_REG_CONTROL);

    if (armed)
    {
        value |= 0b00000010;
    } else
    {
        value &= 0b11111101;
    }

    writeRegister8(DS3231_REG_CONTROL, value);
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool DS3231Clock::isArmed2(void)
{
    uint8_t value;
    value = readRegister8(DS3231_REG_CONTROL);
    value &= 0b00000010;
    value >>= 1;
    return value;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void DS3231Clock::clearAlarm2(void)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_STATUS);
    value &= 0b11111101;

    writeRegister8(DS3231_REG_STATUS, value);
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool DS3231Clock::isAlarm2(bool clear)
{
    uint8_t alarm;

    alarm = readRegister8(DS3231_REG_STATUS);
    alarm &= 0b00000010;

    if (alarm && clear)
    {
        clearAlarm2();
    }

    return alarm;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void DS3231Clock::writeRegister8(uint8_t reg, uint8_t value)
{
    wire->beginTransmission(DS3231Address);
    #if ARDUINO >= 100
        wire->write(reg);
        wire->write(value);
    #else
        wire->send(reg);
        wire->send(value);
    #endif
    wire->endTransmission();
}
//--------------------------------------------------------------------------------------------------------------------------------------
uint8_t DS3231Clock::readRegister8(uint8_t reg)
{
    uint8_t value;
    wire->beginTransmission(DS3231Address);
    #if ARDUINO >= 100
        wire->write(reg);
    #else
        wire->send(reg);
    #endif
    wire->endTransmission();

    wire->requestFrom(DS3231Address, 1);
    while(!wire->available()) {};
    #if ARDUINO >= 100
        value = wire->read();
    #else
        value = wire->receive();
    #endif
    wire->endTransmission();

    return value;
}
//--------------------------------------------------------------------------------------------------------------------------------------
uint8_t DS3231Clock::dec2bcd(uint8_t val)
{
  return( (val/10*16) + (val%10) );
}
//--------------------------------------------------------------------------------------------------------------------------------------
uint8_t DS3231Clock::bcd2dec(uint8_t val)
{
  return( (val/16*10) + (val%16) );
}
//--------------------------------------------------------------------------------------------------------------------------------------
void DS3231Clock::setTime(const DS3231Time& time)
{
  setTime(time.second, time.minute, time.hour, time.dayOfWeek, time.dayOfMonth, time.month,time.year);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void DS3231Clock::setTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint16_t year)
{

  while(year > 100) // приводим к диапазону 0-99
    year -= 100;
 
  wire->beginTransmission(DS3231Address);
  
  wire->write(0); // указываем, что начинаем писать с регистра секунд
  wire->write(dec2bcd(second)); // пишем секунды
  wire->write(dec2bcd(minute)); // пишем минуты
  wire->write(dec2bcd(hour)); // пишем часы
  wire->write(dec2bcd(dayOfWeek)); // пишем день недели
  wire->write(dec2bcd(dayOfMonth)); // пишем дату
  wire->write(dec2bcd(month)); // пишем месяц
  wire->write(dec2bcd(year)); // пишем год
  
  wire->endTransmission();

  delay(10); // немного подождём для надёжности
}
//--------------------------------------------------------------------------------------------------------------------------------------
DS3231Temperature DS3231Clock::getTemperature()
{
 DS3231Temperature res = {0};
  
 union int16_byte {
       int i;
       byte b[2];
   } rtcTemp;
     
  wire->beginTransmission(DS3231Address);
  wire->write(DS3231_REG_TEMPERATURE);
  if(wire->endTransmission() != 0) // ошибка
    return res;

  if(wire->requestFrom(DS3231Address, 2) == 2)
  {
    rtcTemp.b[1] = wire->read();
    rtcTemp.b[0] = wire->read();

    long tempC100 = (rtcTemp.i >> 6) * 25;

    res.Value = tempC100/100;
    res.Fract = abs(tempC100 % 100);
    
  }
  
  return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void DS3231Clock::enableOutput(bool enabled)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_CONTROL);

    value &= 0b11111011;
    value |= (!enabled << 2);

    writeRegister8(DS3231_REG_CONTROL, value);
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool DS3231Clock::isOutput(void)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_CONTROL);

    value &= 0b00000100;
    value >>= 2;

    return !value;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void DS3231Clock::setOutput(DS3231_sqw_t mode)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_CONTROL);

    value &= 0b11100111;
    value |= (mode << 3);

    writeRegister8(DS3231_REG_CONTROL, value);
}
//--------------------------------------------------------------------------------------------------------------------------------------
DS3231_sqw_t DS3231Clock::getOutput(void)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_CONTROL);

    value &= 0b00011000;
    value >>= 3;

    return (DS3231_sqw_t)value;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void DS3231Clock::enable32kHz(bool enabled)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_STATUS);

    value &= 0b11110111;
    value |= (enabled << 3);

    writeRegister8(DS3231_REG_STATUS, value);
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool DS3231Clock::is32kHz(void)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_STATUS);

    value &= 0b00001000;
    value >>= 3;

    return value;
}
//--------------------------------------------------------------------------------------------------------------------------------------
DS3231Time DS3231Clock::getTime()
{
  static DS3231Time t = {0};
  static uint32_t timeMillis = 0;
  static bool first = true;

  // чтобы часто не дёргать I2C
  if(first || (millis() - timeMillis > 1000))
  {
    first = false;
    timeMillis = millis();
    
    wire->beginTransmission(DS3231Address);
    wire->write(DS3231_REG_TIME); // говорим, что мы собираемся читать с регистра 0
    
    if(wire->endTransmission() != 0) // ошибка
      return t;
    
    if(wire->requestFrom(DS3231Address, 7) == 7) // читаем 7 байт, начиная с регистра 0
    {
        t.second = bcd2dec(wire->read() & 0x7F);
        t.minute = bcd2dec(wire->read());
        t.hour = bcd2dec(wire->read() & 0x3F);
        t.dayOfWeek = bcd2dec(wire->read());
        t.dayOfMonth = bcd2dec(wire->read());
        t.month = bcd2dec(wire->read());
        t.year = bcd2dec(wire->read());     
        t.year += 2000; // приводим время к нормальному формату
    } // if
  }
  return t;
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* DS3231Clock::getTimeStr(const DS3231Time& t)
{
  char* writePtr = workBuff;
  
  if(t.hour < 10)
    *writePtr++ = '0';
  else
    *writePtr++ = (t.hour/10) + '0';

  *writePtr++ = (t.hour % 10) + '0';

 *writePtr++ = ':';

 if(t.minute < 10)
  *writePtr++ = '0';
 else
  *writePtr++ = (t.minute/10) + '0';

 *writePtr++ = (t.minute % 10) + '0';

 *writePtr++ = ':';

 if(t.second < 10)
  *writePtr++ = '0';
 else
  *writePtr++ = (t.second/10) + '0';

 *writePtr++ = (t.second % 10) + '0';

 *writePtr = 0;

 return workBuff;
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* DS3231Clock::getDateStr(const DS3231Time& t)
{
  char* writePtr = workBuff;
  if(t.dayOfMonth < 10)
    *writePtr++ = '0';
  else
    *writePtr++ = (t.dayOfMonth/10) + '0';
  *writePtr++ = (t.dayOfMonth % 10) + '0';

  *writePtr++ = '.';

  if(t.month < 10)
    *writePtr++ = '0';
  else
    *writePtr++ = (t.month/10) + '0';
  *writePtr++ = (t.month % 10) + '0';

  *writePtr++ = '.';

  *writePtr++ = (t.year/1000) + '0';
  *writePtr++ = (t.year % 1000)/100 + '0';
  *writePtr++ = (t.year % 100)/10 + '0';
  *writePtr++ = (t.year % 10) + '0';  

  *writePtr = 0;

  return workBuff;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void DS3231Clock::begin(uint8_t wireNumber)
{
#if defined (__arm__) && defined (__SAM3X8E__)  
  if(wireNumber == 1)
    wire = &Wire1;
 else
#endif 
  wire = &Wire;
     
  wire->begin();
    
}
//--------------------------------------------------------------------------------------------------------------------------------------

