#include "DS18B20.h"
#include <OneWire_TRACKER.h>
//--------------------------------------------------------------------------------------------------------------------------------------
void DS18B20::begin(uint8_t _pin) 
{
  pin = _pin;
  pinMode(pin,INPUT);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void DS18B20::setResolution(DS18B20Resolution res)
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
bool DS18B20::readTemperature(DS18B20Temperature& result,DSSensorType type)
{
  result.Value = NO_TEMPERATURE_DATA; // нет данных с датчика
  result.Fract = 0;

  if(!pin)
    return false;

  OneWire ow(pin);

  if(!ow.reset()) // нет датчика
    return false;

  byte data[9] = {0};
   
  ow.write(0xCC); // пофиг на адреса (SKIP ROM)
  ow.write(0x44); // запускаем преобразование

  ow.reset();
  ow.write(0xCC); // пофиг на адреса (SKIP ROM)
  ow.write(0xBE); // читаем scratchpad датчика на пине

  for(uint8_t i=0;i<9;i++)
    data[i] = ow.read();


 if (OneWire::crc8( data, 8) != data[8]) // проверяем контрольную сумму
      return false;
  
  int16_t loByte = data[0];
  int16_t hiByte = data[1];

  int16_t temp = (hiByte << 8) + loByte;
  
  bool negative = (temp & 0x8000);
  
  if(negative)
    temp = (temp ^ 0xFFFF) + 1;

  int16_t tc_100 = 0;
  
  switch(type)
  {
    case DS18B20_SENSOR:
      tc_100 = (6 * temp) + temp/4;
    break;

    case DS18S20_SENSOR:
      tc_100 = (temp*100)/2;
    break;
  }
   

  result.Value = tc_100/100;
  result.Fract = abs(tc_100 % 100);
  
  if(result.Value < -55 || result.Value > 125)
  {
    result.Value = NO_TEMPERATURE_DATA;
    result.Fract = 0;
  }

  return true;
    
}
//--------------------------------------------------------------------------------------------------------------------------------------

