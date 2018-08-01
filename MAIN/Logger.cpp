//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Logger.h"
#include "DS3231.h"
#include "CONFIG.h"
#include "Settings.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LoggerClass Logger;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LoggerClass::LoggerClass()
{
  _COMMA = COMMA_DELIMITER;
  _NEWLINE = NEWLINE;
  lastDayOfMonth = 0;
  todayLogFileNumber = 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoggerClass::begin()
{
  // получаем сохранённые в EEPROM значения текущего имени файла на сегодня
  lastDayOfMonth = Settings.getLoggerDayOfMonth();
  todayLogFileNumber = Settings.getLoggerLogFileNumber();

  DS3231Time tm = RealtimeClock.getTime();

  if(tm.dayOfMonth != lastDayOfMonth) // начался новый день, сбрасываем номер файла за сегодня в 1
  {
    lastDayOfMonth = tm.dayOfMonth;
    todayLogFileNumber = 1;
    
    Settings.setLoggerDayOfMonth(lastDayOfMonth);
    Settings.setLoggerLogFileNumber(todayLogFileNumber);
  }  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String LoggerClass::formatCSV(const String& src)
{
  String fnd = F("\"");
  String rpl = fnd + fnd;
  String input = src;
  input.replace(fnd,rpl); // заменяем кавычки на двойные
  
 if(input.indexOf(_COMMA) != -1 ||
    input.indexOf(F("\"")) != -1 ||
    input.indexOf(F(";")) != -1 ||
    input.indexOf(F(",")) != -1 || // прописываем запятую принудительно, т.к. пользователь может переопределить COMMA_DELIMITER
    input.indexOf(_NEWLINE) != -1
 )
 { // нашли запрещённые символы - надо обрамить в двойные кавычки строку
  
  String s; s.reserve(input.length() + 2);
  s += fnd;
  s += input;
  s += fnd;
  
  return s;
 }

  return input;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoggerClass::newLogFile()
{
  // создаём новый номер файла за сегодня
  closeWorkFile();

  todayLogFileNumber++;

  Settings.setLoggerLogFileNumber(todayLogFileNumber);    
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool LoggerClass::openWorkFile()
{
  closeWorkFile();


 // пишем в лог-файл дату/время срабатывания системы
  SD.mkdir(LOGS_DIRECTORY);

  DS3231Time tm = RealtimeClock.getTime();

  if(tm.dayOfMonth != lastDayOfMonth) // начался новый день, сбрасываем номер файла за сегодня в 1
  {
    lastDayOfMonth = tm.dayOfMonth;
    todayLogFileNumber = 1;
    
    Settings.setLoggerDayOfMonth(lastDayOfMonth);
    Settings.setLoggerLogFileNumber(todayLogFileNumber);    
  }

  // формируем имя файла вида "ггммддnn.csv" (год,месяц,день,номер за сегодня)
  String logFileName;
  
  logFileName = LOGS_DIRECTORY;
  if(!logFileName.endsWith("/"))
    logFileName += "/";

  uint16_t year = tm.year - 2000;
  logFileName += year;
  
  if(tm.month < 10)
    logFileName += '0';
  logFileName += tm.month;

 if(tm.dayOfMonth < 10)
  logFileName += '0';
 logFileName += tm.dayOfMonth;

  // прибавляем номер за сегодня
 if(todayLogFileNumber < 10)
  logFileName += '0';
 logFileName += todayLogFileNumber; 

  logFileName += F(".CSV");

  //DBG(F("WRITE INFO TO: "));
  //DBGLN(logFileName);

  workFile.open(logFileName.c_str(),FILE_WRITE);  
  return workFile.isOpen();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoggerClass::closeWorkFile()
{
  if(workFile.isOpen())
    workFile.close();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoggerClass::write(uint8_t* data,size_t dataLength)
{
  if(!openWorkFile())
    return;

  workFile.write(data,dataLength);

  closeWorkFile();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

