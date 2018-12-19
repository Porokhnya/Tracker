#pragma once

#include <Wire.h>
#include <Arduino.h>
//--------------------------------------------------------------------------------------------------------------------------------------
struct DS3231Time // данные по текущему времени
{
  uint8_t second; // секунда (0-59)
  uint8_t minute; // минута (0-59)
  uint8_t hour; // час (0-23)
  uint8_t dayOfWeek; // день недели (1 - понедельник и т.д.)
  uint8_t dayOfMonth; // день месяца (0-31)
  uint8_t month; // месяц(1-12)
  uint16_t year; // формат - ХХХХ

  static bool isLeapYear(uint16_t year);
  uint32_t unixtime(void);
  static DS3231Time fromUnixtime(uint32_t timeInput);
  
  uint16_t date2days(uint16_t _year, uint8_t _month, uint8_t _day);
  long time2long(uint16_t days, uint8_t hours, uint8_t minutes, uint8_t seconds);

  DS3231Time addDays(long days);
  
}; 
//--------------------------------------------------------------------------------------------------------------------------------------
struct DS3231Temperature // структура показаний с модуля часов
{
  int8_t Value; // значение градусов (-128 - 127)
  uint8_t Fract; // сотые доли градуса (значение после запятой)
};
//--------------------------------------------------------------------------------------------------------------------------------------
enum { DS3231Address = 0x68 }; // адрес датчика
//--------------------------------------------------------------------------------------------------------------------------------------
#define DS3231_REG_TIME             (0x00)
#define DS3231_REG_ALARM_1          (0x07)
#define DS3231_REG_ALARM_2          (0x0B)
#define DS3231_REG_CONTROL          (0x0E)
#define DS3231_REG_STATUS           (0x0F)
#define DS3231_REG_TEMPERATURE      (0x11)
//--------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
    DS3231_EVERY_SECOND   = 0b00001111,
    DS3231_MATCH_S        = 0b00001110,
    DS3231_MATCH_M_S      = 0b00001100,
    DS3231_MATCH_H_M_S    = 0b00001000,
    DS3231_MATCH_DT_H_M_S = 0b00000000,
    DS3231_MATCH_DY_H_M_S = 0b00010000
} DS3231_alarm1_t;
//--------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
    DS3231_EVERY_MINUTE   = 0b00001110,
    DS3231_MATCH_M        = 0b00001100,
    DS3231_MATCH_H_M      = 0b00001000,
    DS3231_MATCH_DT_H_M   = 0b00000000,
    DS3231_MATCH_DY_H_M   = 0b00010000
} DS3231_alarm2_t;
//--------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
    DS3231_1HZ          = 0x00,
    DS3231_4096HZ       = 0x01,
    DS3231_8192HZ       = 0x02,
    DS3231_32768HZ      = 0x03
} DS3231_sqw_t;
//--------------------------------------------------------------------------------------------------------------------------------------
struct RTCAlarmTime
{
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};
//--------------------------------------------------------------------------------------------------------------------------------------
class DS3231Clock
{

  private:

    uint8_t dec2bcd(uint8_t val);
    uint8_t bcd2dec(uint8_t val);

    static char workBuff[12]; // буфер под дату/время

    TwoWire* wire;

    void writeRegister8(uint8_t reg, uint8_t value);
    uint8_t readRegister8(uint8_t reg);    
  
  public:
    DS3231Clock();

    void setTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint16_t year);
    void setTime(const DS3231Time& time);

    const char* getTimeStr(const DS3231Time& t);
    const char* getDateStr(const DS3231Time& t);

    DS3231Time getTime();
    DS3231Temperature getTemperature();


    DS3231_sqw_t getOutput(void);
    void setOutput(DS3231_sqw_t mode);
    void enableOutput(bool enabled);
    bool isOutput(void);
    void enable32kHz(bool enabled);
    bool is32kHz(void);

    void setAlarm1(uint8_t dydw, uint8_t hour, uint8_t minute, uint8_t second, DS3231_alarm1_t mode, bool armed = true);
    RTCAlarmTime getAlarm1(void);
    DS3231_alarm1_t getAlarmType1(void);
    bool isAlarm1(bool clear = true);
    void armAlarm1(bool armed);
    bool isArmed1(void);
    void clearAlarm1(void);
  
    void setAlarm2(uint8_t dydw, uint8_t hour, uint8_t minute, DS3231_alarm2_t mode, bool armed = true);
    RTCAlarmTime getAlarm2(void);
    DS3231_alarm2_t getAlarmType2(void);
    bool isAlarm2(bool clear = true);
    void armAlarm2(bool armed);
    bool isArmed2(void);
    void clearAlarm2(void);
 
    void begin(uint8_t wireNumber=0);
};
//--------------------------------------------------------------------------------------------------------------------------------------
extern DS3231Clock RealtimeClock;
//--------------------------------------------------------------------------------------------------------------------------------------

