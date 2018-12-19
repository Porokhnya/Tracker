#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <wiring_private.h>
#include "AT24CX.h"
#include "DS3231.h"
#include "MCP23017.h"
#include <Adafruit_Si7021.h>
#include "LinkList.h"
#include "CONFIG.h"
#include "CoreButton.h"
#include "TinyVector.h"
#include "CoreTransport.h"
#include "DS18B20Query.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  powerViaUSB = 10,
  batteryPower = 20
  
} PowerType;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct
{
  int raw;
  float voltage;
  
} VoltageData;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define NO_TEMPERATURE_DATA -127
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct
{
  int8_t temperature;
  uint8_t temperatureFract;
  
  int8_t humidity;
  uint8_t humidityFract;
  
} Si7021Data;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct
{
  int8_t value;
  uint8_t fract;
  
} Temperature;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// передача данных по Wi-Fi
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  dataSi7021Temperature,
  dataSi7021Humidity,
  dataDoorState,
  dataADC
} WiFiReportDataType; // тип данных, отправляемых по Wi-Fi
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct
{
  WiFiReportDataType dataType; // тип данных
  DS3231Time checkpoint; // время измерения
  bool insideBordersFlag; // флаг, что измерения попадают в порог
  String* formattedData; // форматированная строка с данными показаний
  
} WiFiReportItem; // данные, которые надо отослать по Wi-Fi
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef Vector<WiFiReportItem> WiFiReportList; // список показаний к отсылу
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  wifiWaitInterval, // ждём наступления интервала до отсыла
  wifiWaitConnection, // ждём установки соединения с устройством
  wifiSendData, // отсылаем данные на устройство
  wifiWaitClientConnected,
  wifiWaitSendDataDone, // ждём окончания отсыла данных
  
} WiFiReportState; // состояние конечного автомата отсыла данных по Wi-Fi
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SettingsClass : public IClientEventsSubscriber
{
  public:

  virtual void OnClientConnect(CoreTransportClient& client, bool connected, int16_t errorCode); // событие "Статус соединения клиента"
  virtual void OnClientDataWritten(CoreTransportClient& client, int16_t errorCode); // событие "Данные из клиента записаны в поток"
  virtual void OnClientDataAvailable(CoreTransportClient& client, uint8_t* data, size_t dataSize, bool isDone); // событие "Для клиента поступили данные", флаг - все ли данные приняты

    SettingsClass();

    void begin();
    void update();

    // работа с ESP    
    String getStationID();
    void setStationID(const String& val);
    
    String getStationPassword();
    void setStationPassword(const String& val);
    
    String getRouterID();
    void setRouterID(const String& val);

    String getRouterPassword();
    void setRouterPassword(const String& val);

    bool shouldConnectToRouter();

    // управление питанием ESP
    void espPower(bool bOn);
    
    // возвращает значение температуры/влажности с датчика Si7021
    Si7021Data readSensor() { return si7021Data; }

    // возвращает значение с аналогового датчика
    Temperature getDS18B20SensorTemperature() { return dsSensorTemperature; }

    // управление подсветкой экрана
    void displayBacklight(bool bOn);
    bool isBacklightOn() {return backlightFlag;}

    // пороги
    int8_t getMinTempBorder();
    void setMinTempBorder(int8_t val);
    int8_t getMaxTempBorder();
    void setMaxTempBorder(int8_t val);

    uint8_t getMinHumidityBorder();
    void setMinHumidityBorder(uint8_t val);
    uint8_t getMaxHumidityBorder();
    void setMaxHumidityBorder(uint8_t val);

    uint16_t getMinADCBorder();
    void setMinADCBorder(uint16_t val);
    uint16_t getMaxADCBorder();
    void setMaxADCBorder(uint16_t val);


    // возвращает актуальное значение интервала логгирования, в минутах
    uint8_t getLoggingInterval();
    void setLoggingInterval(uint8_t val);

    // возвращает кол-во измерений всего
    uint32_t getTotalMeasures();

    // увеличивает кол-во измерений всего на единицу
    void incTotalMeasures();

    // возвращает кол-во измерений за сессию
    uint32_t getSessionMeasures();
    // увеличивает кол-во измерений за сессию на единицу
    void incSessionMeasures();
    // сбрасывает кол-во измерений за сессию
    void resetSessionMeasures();

    // возвращает ФИО водителя
    String getDriver();
    void setDriver(const String& val);

    // возвращает гос. номер ТС
    String getVehicleNumber();
    void setVehicleNumber(const String& val);

    // возвращает гос. номер прицепа
    String getTrailerNumber();
    void setTrailerNumber(const String& val);
    

    // включено ли логгирование?
    bool isLoggingEnabled() { return bLoggingEnabled; } 

    // управление активностью логгирования
    void switchLogging(bool bOn);

    void setLoggerDayOfMonth(uint8_t lastDayOfMonth);
    void setLoggerLogFileNumber(uint8_t todayLogFileNumber);    
    uint8_t getLoggerDayOfMonth();
    uint8_t getLoggerLogFileNumber();

    // возвращает тип питания - от батарей или USB
    PowerType getPowerType();
    void turnPowerOff(); // выключает питание контроллера

    uint32_t getLoggingDuration(); // возвращает длительность логгирования
    void setLoggingDuration(uint32_t val);

    String getUUID(const char* passedUUID);

    bool isDoorOpen();

    // Wi-Fi
    uint32_t getWiFiSendInterval();
    void setWiFiSendInterval(uint32_t val);

    
  private:

    bool isTemperatureInsideBorders();
    bool isHumidityInsideBorders();
    bool isADCInsideBorders();

    // Wi-Fi
    WiFiReportState wifiState; // состояние конечного автомата работы с ESP
    uint32_t wifiTimer; // таймер, отсчитывающий время какой-либо операции с Wi-Fi
    uint32_t wifiInterval; // интервал до следующей операции
    bool startProcessWiFiFlag; // флаг, что надо начать работу с ESP
    CoreESPTransport* esp; // транспорт
    size_t itemsInPacket; // кол-во записей, которое попало в текущий пакет данных для отсыла
    WiFiReportList wifiData; // очередь данных для отсыла

    void pushSensorsDataToWiFiQueue();
    void sendWiFiDataPacket();
    String* httpQuery; // строка запроса
    String remoteIP; // куда коннектимся
    CoreTransportClient espClient; // клиент транспорта ESP
    bool anyWriteError;
    void switchToESPWaitMode();
    void makeHTTPQuery();
    void removeSentData();

     bool backlightFlag;

     uint8_t doorState;
     static void doorStateChanged();
     bool wantLogDoorState;
     void logDoorState();

      Button powerButton;      


      uint8_t read8(uint16_t address, uint8_t defaultVal = 0);
      void write8(uint16_t address, uint8_t val);

      uint16_t read16(uint16_t address, uint16_t defaultVal = 0);
      void write16(uint16_t address, uint16_t val);

      uint32_t read32(uint16_t address, uint32_t defaultVal = 0);
      void write32(uint16_t address, uint32_t val);

      String readString(uint16_t address, byte maxlength);
      void writeString(uint16_t address, const String& v, byte maxlength);


      void saveLoggingTimer();
      void accumulateLoggingDuration();
      

      PowerType powerType;
      static void checkPower();

      void setupDS3231Alarm();
      static void alarmFunction();
      bool bWantCheckAlarm;
      void checkIsAlarm();
      uint16_t alarmTimer;


      void logDataToSD();


     void updateDataFromSensors();

     uint8_t loggingInterval;
     bool bLoggingEnabled;
     bool bWantToLogFlag;

    static void test_key();
    static int pressedKey;

    AT24C64* eeprom;
    DS3231Temperature coreTemp;
    uint32_t sensorsUpdateTimer;
    //Temperature analogSensorTemperature;
    DS18B20Support ds18b20;
    Temperature dsSensorTemperature;

    Adafruit_MCP23017 MCP;
    Adafruit_Si7021 si7021;
    Si7021Data si7021Data;
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern SettingsClass Settings;
extern Uart Serial2;
//extern uint8_t KNOWN_LOGGING_INTERVALS[LOGGING_INTERVALS_COUNT];
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
