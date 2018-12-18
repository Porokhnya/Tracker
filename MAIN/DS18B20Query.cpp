#include "DS18B20Query.h"
#include <OneWire_TRACKER>
//#include <OneWire.h>
#include "Settings.h"
//--------------------------------------------------------------------------------------------------------------------------------------
void DS18B20Support::begin(uint8_t _pin) 
{
  pin = _pin;
  pinMode(pin,INPUT);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void DS18B20Support::setResolution(DS18B20Resolution res)
{
  if(!pin)
    return;

  OneWire ow(pin);
 

  if(!ow.reset()) // нет датчика
    return;  

   ow.write(0xCC); // пофиг на адреса (SKIP ROM)
   ow.write(0x4E); // запускаем запись в scratchpad

   ow.write(0); // верхний температурный порог 
   ow.write(0); // нижний температурный порог
   ow.write(res); // разрешение датчика

   ow.reset();
   ow.write(0xCC); // пофиг на адреса (SKIP ROM)
   ow.write(0x48); // COPY SCRATCHPAD
   delay(10);
   ow.reset();
   
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool DS18B20Support::readTemperature(DS18B20Temperature* result, uint8_t sensorNumber)
{
  result->Whole = NO_TEMPERATURE_DATA; // нет данных с датчика
  result->Fract = 0;

  if(!pin)
    return false;


  OneWire ow(pin);

  if(!ow.reset()) // нет датчика
    return false;

  byte data[9];
   
  ow.write(0xCC); // пофиг на адреса (SKIP ROM)
  ow.write(0x44); // запускаем преобразование

  ow.reset();
  
  
  byte addr[8];
  ow.reset_search();
  
  for(uint8_t i=0;i<=sensorNumber;i++)
  {
    if(!ow.search(addr))
    {
      
      return false;
    }
  } // for

 if (OneWire::crc8(addr, 7) != addr[7]) 
 {
      return false;
  } 

  uint8_t type_s;

  switch (addr[0]) 
  {
    case 0x10:
   //   Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
   //   Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
  //    Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
  //    Serial.println("Device is not a DS18x20 family device.");
      return false;
  }   

  ow.reset();
  ow.select(addr);
  
  ow.write(0xBE); // читаем scratchpad датчика на пине

  for(uint8_t i=0;i<9;i++)
    data[i] = ow.read();


 if (OneWire::crc8( data, 8) != data[8]) // проверяем контрольную сумму
      return false;

 // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) 
  {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) 
    {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } 
  else 
  {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  
  float celsius = ((float) raw / 16.0)*100.0;
  int32_t tc_100 = celsius;
  
  result->Whole = tc_100/100;
  result->Fract = abs(tc_100 % 100);


  if(result->Whole < -55 || result->Whole > 125)
  {
    result->Whole = NO_TEMPERATURE_DATA;
    result->Fract = 0;
  }

  return true;
    
}
//--------------------------------------------------------------------------------------------------------------------------------------

