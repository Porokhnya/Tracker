#include "CoreTransport.h"
#include "Settings.h"
#include "CONFIG.h"
//--------------------------------------------------------------------------------------------------------------------------------------
#define CIPSEND_COMMAND F("AT+CIPSENDBUF=")
//--------------------------------------------------------------------------------------------------------------------------------------
// CoreTransportClient
//--------------------------------------------------------------------------------------------------------------------------------------
CoreTransportClient::CoreTransportClient()
{
  socket = NO_CLIENT_ID;
  dataBuffer = NULL;
  dataBufferSize = 0;
  parent = NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------
CoreTransportClient::~CoreTransportClient()
{
  clear();
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransportClient::accept(CoreTransport* _parent)
{
  parent = _parent;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransportClient::clear()
{
    delete [] dataBuffer; 
    dataBuffer = NULL;
    dataBufferSize = 0;
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransportClient::disconnect()
{
  if(!parent)
    return;
  
  if(!connected())
    return;

  parent->doDisconnect(*this);
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransportClient::connect(const char* ip, uint16_t port)
{
  if(!parent)
    return;
  
  if(connected()) // уже присоединены, нельзя коннектится до отсоединения!!!
    return;
          
  parent->doConnect(*this,ip,port);
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreTransportClient::write(uint8_t* buff, size_t sz)
{
  if(!parent)
    return false;
  
    if(!sz || !buff || !connected() || socket == NO_CLIENT_ID)
    {
      DBGLN(F("CoreTransportClient - CAN'T WRITE!"));
      /*
      if(!connected())
      {
        DBGLN(F("NOT CONNECTED!!!"));
      }
      if(socket == NO_CLIENT_ID)
      {
        DBGLN(F("socket == NO_CLIENT_ID!!!"));        
      }
      */
      return false;
    }

  clear();
  dataBufferSize = sz; 
  if(dataBufferSize)
  {
      dataBuffer = new  uint8_t[dataBufferSize];
      memcpy(dataBuffer,buff,dataBufferSize);
  }

    parent->doWrite(*this);
    
   return true;
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreTransportClient::connected() 
{
  if(!parent || socket == NO_CLIENT_ID)
    return false;
    
  return parent->connected(socket);
}
//--------------------------------------------------------------------------------------------------------------------------------------
// CoreTransport
//--------------------------------------------------------------------------------------------------------------------------------------
CoreTransport::CoreTransport(uint8_t clientsPoolSize)
{
  for(uint8_t i=0;i<clientsPoolSize;i++)
  {
    CoreTransportClient* client = new CoreTransportClient();
    client->accept(this);
    client->bind(i);
    
    pool.push_back(client);
    status.push_back(false);
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
CoreTransport::~CoreTransport()
{
  for(size_t i=0;i<pool.size();i++)
  {
    delete pool[i];
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::initPool()
{

  DBGLN(F("ESP: INIT CLIENTS POOL..."));
  
  Vector<CoreTransportClient*> tmp = externalClients;
  for(size_t i=0;i<tmp.size();i++)
  {
    notifyClientConnected(*(tmp[i]),false,CT_ERROR_NONE);
    tmp[i]->release();
  }
  
  for(size_t i=0;i<status.size();i++)
  {
    status[i] = false;
    pool[i]->clear(); // очищаем внутренних клиентов
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreTransport::connected(uint8_t socket)
{
  return status[socket];
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::doWrite(CoreTransportClient& client)
{
  if(!client.connected())
  {
    client.clear();
    return;
  }

   beginWrite(client); 
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::doConnect(CoreTransportClient& client, const char* ip, uint16_t port)
{
  if(client.connected())
    return;

   // запоминаем нашего клиента
   client.accept(this);

  // если внешний клиент - будем следить за его статусом соединения/подсоединения
   if(isExternalClient(client))
    externalClients.push_back(&client);

   beginConnect(client,ip,port); 
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::doDisconnect(CoreTransportClient& client)
{
  if(!client.connected())
    return;

    beginDisconnect(client);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::subscribe(IClientEventsSubscriber* subscriber)
{
  for(size_t i=0;i<subscribers.size();i++)
  {
    if(subscribers[i] == subscriber)
      return;
  }

  subscribers.push_back(subscriber);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::unsubscribe(IClientEventsSubscriber* subscriber)
{
  for(size_t i=0;i<subscribers.size();i++)
  {
    if(subscribers[i] == subscriber)
    {
      for(size_t k=i+1;k<subscribers.size();k++)
      {
        subscribers[k-1] = subscribers[k];
      }
      subscribers.pop();
      break;
    }
  }  
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreTransport::isExternalClient(CoreTransportClient& client)
{
  // если клиент не в нашем пуле - это экземпляр внешнего клиента
  for(size_t i=0;i<pool.size();i++)
  {
    if(pool[i] == &client)
      return false;
  }

  return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::notifyClientConnected(CoreTransportClient& client, bool connected, int16_t errorCode)
{
   // тут надо синхронизировать с пулом клиентов
   if(client.socket != NO_CLIENT_ID)
   {
      status[client.socket] = connected;
   }
  
    for(size_t i=0;i<subscribers.size();i++)
    {
      subscribers[i]->OnClientConnect(client,connected,errorCode);
    }

      // возможно, это внешний клиент, надо проверить - есть ли он в списке слежения
      if(!connected) // пришло что-то типа 1,CLOSED
      {         
        // клиент отсоединился, надо освободить его сокет
        for(size_t i=0;i<externalClients.size();i++)
        {
          if(externalClients[i]->socket == client.socket)
          {
            externalClients[i]->clear();
            
			DBG(F("RELEASE SOCKET ON OUTGOING CLIENT #"));
			DBGLN(String(client.socket));
            
            externalClients[i]->release(); // освобождаем внешнему клиенту сокет
            
            for(size_t k=i+1;k<externalClients.size();k++)
            {
              externalClients[k-1] = externalClients[k];
            }
            externalClients.pop();
            break;
          }
        } // for
      } // if(!connected)
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::notifyDataWritten(CoreTransportClient& client, int16_t errorCode)
{
    for(size_t i=0;i<subscribers.size();i++)
    {
      subscribers[i]->OnClientDataWritten(client,errorCode);
    } 
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::notifyDataAvailable(CoreTransportClient& client, uint8_t* data, size_t dataSize, bool isDone)
{
    for(size_t i=0;i<subscribers.size();i++)
    {
      subscribers[i]->OnClientDataAvailable(client,data,dataSize,isDone);
    }  
}
//--------------------------------------------------------------------------------------------------------------------------------------
CoreTransportClient* CoreTransport::getClient(uint8_t socket)
{
  if(socket != NO_CLIENT_ID)
    return pool[socket];

  for(size_t i=0;i<pool.size();i++)
  {
    if(!pool[i]->connected())
      return pool[i];
  }

  return NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------
// CoreESPTransport
//--------------------------------------------------------------------------------------------------------------------------------------
CoreESPTransport* ESP = NULL;
//--------------------------------------------------------------------------------------------------------------------------------------
uint16_t CoreESPTransport::refsCount = 0;
//--------------------------------------------------------------------------------------------------------------------------------------
CoreESPTransport::CoreESPTransport() : CoreTransport(ESP_MAX_CLIENTS)
{
  recursionGuard = 0;
  flags.waitCipstartConnect = false;
  cipstartConnectClient = NULL;
  workStream = NULL;
  failConnectToRouterAttempts = 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------
CoreESPTransport* CoreESPTransport::ActiveInstance()
{
  return ESP;
}
//--------------------------------------------------------------------------------------------------------------------------------------
CoreESPTransport* CoreESPTransport::Create()
{
  if(!ESP)
  {
    // нет ни одного экземпляра транспорта, создаём и запускаем в работу
    ESP = new CoreESPTransport();
    ESP->begin();
  }
  ++refsCount;

  return ESP;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::Destroy()
{
  if(--refsCount < 1) 
  {
    delete this;
    ESP = NULL;
    // выключаем питание ESP
    Settings.espPower(false);
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::readFromStream()
{
  if(!workStream)
    return;
    
  while(workStream->available())
  {
    receiveBuffer.push_back((uint8_t) workStream->read());
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::sendCommand(const String& command, bool addNewLine)
{

  size_t len = command.length();
  for(size_t i=0;i<len;i++)
  {
    // записали байтик
    workStream->write(command[i]);

    // прочитали, что пришло от ESP
    readFromStream();

  }
    
  if(addNewLine)
  {
    workStream->println();
  }
  
  // прочитали, что пришло от ESP
  readFromStream();

  DBG(F("ESP: ==> "));
  DBGLN(command);

  machineState = espWaitAnswer; // говорим, что надо ждать ответа от ESP
  // запоминаем время отсылки последней команды
  timer = millis();
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreESPTransport::pingGoogle(bool& result)
{
//    if(machineState != espIdle || !workStream || !ready() || initCommandsQueue.size()) // чего-то делаем, не могём
    if(!workStream || !ready()) // не готовы
    {
      return false;
    }

    // ждём, когда освободится очередь
    waitForReady();

    
    flags.specialCommandDone = false;
    clearSpecialCommandResults();
    initCommandsQueue.push_back(cmdPING);

    while(!flags.specialCommandDone)
    {     
      update();
      yield();
      if(flags.wantReconnect || machineState == espReboot)
        break;
    }

    if(!specialCommandResults.size())
    {
      return false;
    }

    result = *(specialCommandResults[0]) == F("OK");

    clearSpecialCommandResults();

    return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreESPTransport::getMAC(String& staMAC, String& apMAC)
{

    //if(machineState != espIdle || !workStream || !ready() || initCommandsQueue.size()) // чего-то делаем, не могём
    if(!workStream || !ready()) // не готовы
    
    {
      return false;
    }

    // ждём, когда освободится очередь
    waitForReady();


    staMAC = '-';
    apMAC = '-';    

    flags.specialCommandDone = false;
    clearSpecialCommandResults();
    initCommandsQueue.push_back(cmdCIFSR);

    while(!flags.specialCommandDone)
    {     
      update();
      yield();
      if(flags.wantReconnect || machineState == espReboot)
        break;
    }

    if(!specialCommandResults.size())
    {
      return false;
    }

    for(size_t i=0;i<specialCommandResults.size();i++)
    {
      String* s = specialCommandResults[i];
      
      int idx = s->indexOf(F("STAMAC,"));
      if(idx != -1)
      {
        const char* stamacPtr = s->c_str() + idx + 7;
        staMAC = stamacPtr;
      }
      else
      {
        idx = s->indexOf(F("APMAC,"));
        if(idx != -1)
        {
          const char* apmacPtr = s->c_str() + idx + 6;
          apMAC = apmacPtr;
        }
      }
      
      
    } // for

    clearSpecialCommandResults();

    return true;            
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::waitForReady()
{
  while(1)
  {
    if(machineState == espIdle && !initCommandsQueue.size())
      break;

    readFromStream();
    update();
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreESPTransport::getIP(String& stationCurrentIP, String& apCurrentIP)
{

    //if(machineState != espIdle || !workStream || !ready() || initCommandsQueue.size()) // чего-то делаем, не могём
    if(!workStream || !ready()) // не готовы
    {
      return false;
    }

    // ждём, когда освободится очередь
    waitForReady();

    stationCurrentIP = '-';
    apCurrentIP = '-';    

    flags.specialCommandDone = false;
    clearSpecialCommandResults();
    initCommandsQueue.push_back(cmdCIFSR);

    while(!flags.specialCommandDone)
    {     
      update();
      yield();
      if(flags.wantReconnect || machineState == espReboot)
        break;
    }

    if(!specialCommandResults.size())
    {
      return false;
    }

    for(size_t i=0;i<specialCommandResults.size();i++)
    {
      String* s = specialCommandResults[i];
      
      int idx = s->indexOf(F("STAIP,"));
      if(idx != -1)
      {
        const char* staipPtr = s->c_str() + idx + 6;
        stationCurrentIP = staipPtr;
      }
      else
      {
        idx = s->indexOf(F("APIP,"));
        if(idx != -1)
        {
          const char* apipPtr = s->c_str() + idx + 5;
          apCurrentIP = apipPtr;
        }
      }
      
      
    } // for

    clearSpecialCommandResults();

    return true;

}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::sendCommand(ESPCommands command)
{
  currentCommand = command;
  
  // тут посылаем команду в ESP
  switch(command)
  {
    case cmdNone:
    case cmdCIPCLOSE: // ничего тут не надо, эти команды формируем не здесь
    case cmdCIPSTART:
    case cmdCIPSEND:
    case cmdWaitSendDone:
    break;

    case cmdPING:
    {
      DBGLN(F("ESP: PING GOOGLE..."));
      sendCommand(F("AT+PING=\"google.com\""));     
    }
    break;


    case cmdCIFSR:
    {
      DBGLN(F("ESP: REQUEST CIFSR..."));
      sendCommand(F("AT+CIFSR"));      
    }
    break;

    case cmdWantReady:
    {
      DBGLN(F("ESP: reset..."));

      // принудительно очищаем очередь клиентов
      clearClientsQueue(true);
      // и говорим, что все слоты свободны
      initPool();
      
      sendCommand(F("AT+RST"));
    }
    break;

    case cmdEchoOff:
    {
      DBGLN(F("ESP: echo OFF..."));
      sendCommand(F("ATE0"));
    }
    break;

    case cmdCWMODE:
    {
      DBGLN(F("ESP: softAP mode..."));
      sendCommand(F("AT+CWMODE_CUR=3"));
    }
    break;

    case cmdCWSAP:
    {
        DBGLN(F("ESP: Creating the access point..."));

      
        String com = F("AT+CWSAP_CUR=\"");
        com += Settings.getStationID();
        com += F("\",\"");
        com += Settings.getStationPassword();
        com += F("\",8,4");
        
        sendCommand(com);      
    }
    break;

    case cmdCWJAP:
    {
        DBGLN(F("ESP: Connecting to the router..."));
        
        String com = F("AT+CWJAP_CUR=\"");
        com += Settings.getRouterID();
        com += F("\",\"");
        com += Settings.getRouterPassword();
        com += F("\"");
        sendCommand(com);      
    }
    break;

    case cmdCWQAP:
    {
      DBGLN(F("ESP: Disconnect from router..."));
      sendCommand(F("AT+CWQAP"));
    }
    break;

    case cmdCIPMODE:
    {
      DBGLN(F("ESP: Set the TCP server mode to 0..."));
      sendCommand(F("AT+CIPMODE=0"));
    }
    break;

    case cmdCIPMUX:
    {
      DBGLN(F("ESP: Allow multiple connections..."));
      sendCommand(F("AT+CIPMUX=1"));
    }
    break;

    case cmdCIPSERVER:
    {
      DBGLN(F("ESP: Starting TCP-server..."));
      sendCommand(F("AT+CIPSERVER=1,1975"));
    }
    break;

    case cmdCheckModemHang:
    {
      DBGLN(F("ESP: check for ESP available..."));
      
      flags.wantReconnect = false;
      sendCommand(F("AT+CWJAP?")); // проверяем, подконнекчены ли к роутеру
    }
    break;
    
  } // switch

}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreESPTransport::isESPBootFound(const String& line)
{
  return (line == F("ready")) || line.startsWith(F("Ai-Thinker Technology"));
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreESPTransport::isKnownAnswer(const String& line, ESPKnownAnswer& result)
{
  result = kaNone;
  
  if(line == F("OK"))
  {
    result = kaOK;
    return true;
  }
  if(line == F("ERROR"))
  {
    result = kaError;
    return true;
  }
  if(line == F("FAIL"))
  {
    result = kaFail;
    return true;
  }
  if(line.endsWith(F("SEND OK")))
  {
    result = kaSendOk;
    return true;
  }
  if(line.endsWith(F("SEND FAIL")))
  {
    result = kaSendFail;
    return true;
  }
  if(line.endsWith(F("ALREADY CONNECTED")))
  {
    result = kaAlreadyConnected;
    return true;
  }
  return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::processConnect(const String& line)
{
     // клиент подсоединился
     
    int idx = line.indexOf(F(",CONNECT"));
    
    if(idx == -1)
      return;
    
    String s = line.substring(0,idx);
    int16_t clientID = s.toInt();
    
    if(clientID >=0 && clientID < ESP_MAX_CLIENTS)
    {
        DBG(F("ESP: client connected - #"));
        DBGLN(String(clientID));

      // тут смотрим - посылали ли мы запрос на коннект?
      if(flags.waitCipstartConnect && cipstartConnectClient != NULL && clientID == cipstartConnectClientID)
      {
        DBGLN(F("ESP: WAIT CIPSTART CONNECT, CATCH OUTGOING CLIENT!"));
        // есть клиент, для которого надо установить ID.
        // тут у нас может возникнуть коллизия, когда придёт коннект с внешнего адреса.
        // признаком этой коллизии является тот факт, что если мы в этой ветке - мы ОБЯЗАНЫ
        // получить один из известных ответов OK, ERROR, ALREADY CONNECTED
        // ДО ТОГО, как придёт статус ID,CONNECT
        cipstartConnectClient->bind(clientID);
        /*
        if(!flags.cipstartConnectKnownAnswerFound)
        {
			    DBGLN(F("ESP: WAIT CIPSTART CONNECT, NO OK FOUND!"));
          
          // не найдено ни одного ответа из известных. Проблема в том, что у внешнего клиента ещё нет слота,
          // но нам надо ему временно выставить слот (мы это сделали выше), потом вызвать событие отсоединения, потом - очистить ему слот
          removeClientFromQueue(cipstartConnectClient);
          notifyClientConnected(*cipstartConnectClient,false,CT_ERROR_CANT_CONNECT);
          cipstartConnectClient->release();

          // при этом, поскольку мы освободили нашего клиента на внешнее соединение и это коллизия,
          // мы должны сообщить, что клиент от ESP подсоединился
          CoreTransportClient* client = getClient(clientID);            
          notifyClientConnected(*client,true,CT_ERROR_NONE);
          
          // поскольку строка ID,CONNECT пришла ДО известного ответа - это коллизия, и мы в ветке cmdCIPSTART,
          // поэтому мы здесь сами должны удалить клиента из очереди и переключиться на ожидание
          machineState = espIdle;
        }
        else
        {
			    DBGLN(F("ESP: WAIT CIPSTART CONNECT, CLIENT CONNECTED!"));
          
          // если вы здесь - ответ OK получен сразу после команды AT+CIPSTART,
          // клиент из очереди удалён, и, раз мы получили ID,CONNECT - мы можем сообщать, что клиент подсоединён
          CoreTransportClient* client = getClient(clientID);    
          notifyClientConnected(*client,true,CT_ERROR_NONE);          
        }
        */
      
          flags.waitCipstartConnect = false;
          // cipstartConnectClient = NULL;
         // cipstartConnectClientID = NO_CLIENT_ID;
          //flags.cipstartConnectKnownAnswerFound = false;
        
      } // if
      else
      {            
        // если мы здесь - то мы не ждём подсоединения клиента на исходящий адрес
        // просто выставляем клиенту флаг, что он подсоединён
        CoreTransportClient* client = getClient(clientID);            
        notifyClientConnected(*client,true,CT_ERROR_NONE);
      }
      
    } // if
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::processDisconnect(const String& line)
{
  // клиент отсоединился
    int idx = line.indexOf(F(",CLOSED"));
    
    if(idx == -1)
      idx = line.indexOf(F(",CONNECT FAIL"));
      
    if(idx == -1)
      return;
      
    String s = line.substring(0,idx);
    int16_t clientID = s.toInt();
    
    if(clientID >=0 && clientID < ESP_MAX_CLIENTS)
    {
        DBG(F("ESP: client disconnected - #"));
        DBGLN(String(clientID));

      // выставляем клиенту флаг, что он отсоединён
      CoreTransportClient* client = getClient(clientID);            
      notifyClientConnected(*client,false,CT_ERROR_NONE);
      
    }

    // тут смотрим - посылали ли мы запрос на коннект?
    if(flags.waitCipstartConnect && cipstartConnectClient != NULL && clientID == cipstartConnectClientID)
    {

        DBG(F("ESP: waitCipstartConnect - #"));
        DBGLN(String(clientID));
      
      // есть клиент, для которого надо установить ID
      cipstartConnectClient->bind(clientID);
      notifyClientConnected(*cipstartConnectClient,false,CT_ERROR_NONE);
      cipstartConnectClient->release();
      removeClientFromQueue(cipstartConnectClient);
      
      flags.waitCipstartConnect = false;
      cipstartConnectClient = NULL;
      cipstartConnectClientID = NO_CLIENT_ID;
      
    } // if            
          
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::processKnownStatusFromESP(const String& line)
{
   // смотрим, подсоединился ли клиент?
   if(line.endsWith(F(",CONNECT")))
   {
    processConnect(line);
   } // if
   else 
   if(line.endsWith(F(",CLOSED")) || line.endsWith(F(",CONNECT FAIL")))
   {
    processDisconnect(line);
   } // if(idx != -1)
   else
   //if(line == F("WIFI CONNECTED"))
   if(line == F("WIFI GOT IP"))
   {
      flags.connectedToRouter = true;
      DBGLN(F("ESP: connected to router!"));
   }
   else
   if(line == F("WIFI DISCONNECT"))
   {
      flags.connectedToRouter = false;
      DBGLN(F("ESP: disconnected from router!"));
   }  
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreESPTransport::checkIPD(const TransportReceiveBuffer& buff)
{
  if(buff.size() < 9) // минимальная длина для IPD, на примере +IPD,1,1:
    return false;

  if(buff[0] == '+' && buff[1] == 'I' && buff[2] == 'P' && buff[3] == 'D')
  {
    size_t to = min(buff.size(),20); // заглядываем вперёд на 20 символов, не больше
    for(size_t i=4;i<to;i++)
    {
      if(buff[i] == ':') // буфер начинается на +IPD и содержит дальше ':', т.е. за ним уже идут данные
        return true;
    }
  }

  return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::update()
{ 
  if(!workStream) // нет рабочего потока
    return;

  if(flags.onIdleTimer) // попросили подождать определённое время, в течение которого просто ничего не надо делать
  {
      if(millis() - idleTimer > idleTime)
      {
        flags.onIdleTimer = false;
      }
  }

  // читаем из потока всё, что там есть
  readFromStream();

  RecursionCounter recGuard(&recursionGuard);

  if(recursionGuard > 1) // рекурсивный вызов - просто вычитываем из потока - и всё.
  {
    DBGLN(F("ESP: RECURSION!"));
    return;
  }
  
  bool hasAnswer = receiveBuffer.size();

  if(!hasAnswer)
  {
      // нет ответа от ESP, проверяем, зависла ли она?
      if(millis() - timer > WIFI_MAX_ANSWER_TIME)
      {
        DBGLN(F("ESP: modem not answering, reboot!"));

        #ifdef USE_WIFI_REBOOT
          // есть пин, который надо использовать при зависании
          Settings.espPower(false);
       #endif
       
        machineState = espReboot;
        timer = millis();
        
      } // if 
  }
  else // есть ответ
  {
   // timer = millis();
  }

  // выставляем флаг, что мы хотя бы раз получили хоть чего-то от ESP
  flags.isAnyAnswerReceived = flags.isAnyAnswerReceived || hasAnswer;

  bool hasAnswerLine = false; // флаг, что мы получили строку ответа от модема    

  String thisCommandLine;

  // тут проверяем, есть ли чего интересующего в буфере?
  if(checkIPD(receiveBuffer))
  {
      
    // в буфере лежит +IPD,ID,DATA_LEN:
      int16_t idx = receiveBuffer.indexOf(','); // ищем первую запятую после +IPD
      uint8_t* ptr = receiveBuffer.pData();
      ptr += idx+1;
      // перешли за запятую, парсим ID клиента
      String connectedClientID;
      while(*ptr != ',')
      {
        connectedClientID += (char) *ptr;
        ptr++;
      }
      ptr++; // за запятую
      String dataLen;
      while(*ptr != ':')
      {
        dataLen += (char) *ptr;
        ptr++;
      }
  
      // получили ID клиента и длину его данных, которые - пока в потоке, и надо их быстро попакетно вычитать
      int ipdClientID = connectedClientID.toInt();
      size_t ipdClientDataLength = dataLen.toInt();

        DBG(F("+IPD DETECTED, CLIENT #"));
        DBG(String(ipdClientID));
        DBG(F(", LENGTH="));
        DBGLN(String(ipdClientDataLength));

      // удаляем +IPD,ID,DATA_LEN:
      receiveBuffer.remove(0,receiveBuffer.indexOf(':')+1);

      // у нас есть длина данных к вычитке, плюс сколько-то их лежит в буфере уже.
      // читать всё - мы не можем, т.к. данные могут быть гигантскими.
      // следовательно, надо читать по пакетам.
      CoreTransportClient* cl = getClient(ipdClientID);

      if(receiveBuffer.size() >= ipdClientDataLength)
      {
        // на время события мы должны обеспечить неизменность буфера, т.к.
        // в обработчике события может быть вызван yield, у указатель на память станет невалидным!
        
        uint8_t* thisBuffer = new uint8_t[ipdClientDataLength];
        memcpy(thisBuffer,receiveBuffer.pData(),ipdClientDataLength);

        receiveBuffer.remove(0,ipdClientDataLength);

        if(!receiveBuffer.size())
          receiveBuffer.clear();
          
        // весь пакет - уже в буфере
        notifyDataAvailable(*cl, thisBuffer, ipdClientDataLength, true);
        delete [] thisBuffer;
                
      }
      else
      {
        // не хватает части пакета в буфере.
        
        // теперь смотрим, сколько у нас данных ещё не послано клиентам
        size_t remainingDataLength = ipdClientDataLength;

        // нам осталось послать remainingDataLength данных клиентам,
        // побив их на пакеты длиной максимум TRANSPORT_MAX_PACKET_LENGTH

        // вычисляем длину одного пакета
        size_t packetLength = min(TRANSPORT_MAX_PACKET_LENGTH,remainingDataLength);

        while(remainingDataLength > 0)
        {
            // читаем, пока не хватает данных для одного пакета
            while(receiveBuffer.size() < packetLength)
            {              
                // данных не хватает, дочитываем
                if(!workStream->available())
                  continue;
    
                receiveBuffer.push_back((uint8_t) workStream->read());
                
            } // while

            // вычитали один пакет, уведомляем клиентов, при этом может пополниться буфер,
            // поэтому сохраняем пакет так, чтобы указатель на него был всегда валидным.
            uint8_t* thisBuffer = new uint8_t[packetLength];
            memcpy(thisBuffer,receiveBuffer.pData(),packetLength);

            receiveBuffer.remove(0,packetLength);
            if(!receiveBuffer.size())
              receiveBuffer.clear();

            notifyDataAvailable(*cl, thisBuffer, packetLength, (remainingDataLength - packetLength) == 0);
            delete [] thisBuffer;
            
            remainingDataLength -= packetLength;
            packetLength = min(TRANSPORT_MAX_PACKET_LENGTH,remainingDataLength);
        } // while

          
      } // else

    
  } // if(checkIPD(receiveBuffer))
  else if(flags.waitForDataWelcome && receiveBuffer[0] == '>')
  {
    flags.waitForDataWelcome = false;
    thisCommandLine = '>';
    hasAnswerLine = true;

    receiveBuffer.remove(0,1);
  }
  else // любые другие ответы от ESP
  {
    // ищем до первого перевода строки
    size_t cntr = 0;
    for(;cntr<receiveBuffer.size();cntr++)
    {
      if(receiveBuffer[cntr] == '\n')
      {          
        hasAnswerLine = true;
        cntr++;
        break;
      }
    } // for

    if(hasAnswerLine) // нашли перевод строки в потоке
    {
      for(size_t i=0;i<cntr;i++)
      {
        if(receiveBuffer[i] != '\r' && receiveBuffer[i] != '\n')
          thisCommandLine += (char) receiveBuffer[i];
      } // for

      receiveBuffer.remove(0,cntr);
      
    } // if(hasAnswerLine)
  } // else

  // если в приёмном буфере ничего нету - просто почистим память
  if(!receiveBuffer.size())
    receiveBuffer.clear();


  if(hasAnswerLine && !thisCommandLine.length()) // пустая строка, не надо обрабатывать
    hasAnswerLine = false;

   #ifdef _DEBUG
    if(hasAnswerLine)
    {
      DBG(F("<== ESP: "));
      DBGLN(thisCommandLine);
    }
   #endif

    // тут анализируем ответ от ESP, если он есть, на предмет того - соединён ли клиент, отсоединён ли клиент и т.п.
    // это нужно делать именно здесь, поскольку в этот момент в ESP может придти внешний коннект.
    if(hasAnswerLine)
    {
      processKnownStatusFromESP(thisCommandLine);
    }

  // при разборе ответа тут будет лежать тип ответа, чтобы часто не сравнивать со строкой
  ESPKnownAnswer knownAnswer = kaNone;

  if(!flags.onIdleTimer) // только если мы не в режиме простоя
  {
    // анализируем состояние конечного автомата, чтобы понять, что делать
    switch(machineState)
    {
        case espIdle: // ничего не делаем, можем работать с очередью команд и клиентами
        {            
            // смотрим - если есть хоть одна команда в очереди инициализации - значит, мы в процессе инициализации, иначе - можно работать с очередью клиентов
            if(initCommandsQueue.size())
            {
                DBGLN(F("ESP: process next init command..."));
                currentCommand = initCommandsQueue[initCommandsQueue.size()-1];
                initCommandsQueue.pop();
                sendCommand(currentCommand);
            } // if
            else
            {
              // в очереди команд инициализации ничего нет, значит, можем выставить флаг, что мы готовы к работе с клиентами
              flags.ready = true;
              
              if(clientsQueue.size())
              {
                  // получаем первого клиента в очереди
                  TransportClientQueueData dt = clientsQueue[0];
                  int clientID = dt.client->socket;
                  
                  // смотрим, чего он хочет от нас
                  switch(dt.action)
                  {
                    case actionDisconnect:
                    {
                      // хочет отсоединиться
                      currentCommand = cmdCIPCLOSE;
                      String cmd = F("AT+CIPCLOSE=");
                      cmd += clientID;
                      sendCommand(cmd);
                      
                    }
                    break; // actionDisconnect

                    case actionConnect:
                    {
                      // мы разрешаем коннектиться только тогда, когда предыдущий коннект клиента уже обработан
                      if(!cipstartConnectClient)
                      {
                          // здесь надо искать первый свободный слот для клиента
                          CoreTransportClient* freeSlot = getClient(NO_CLIENT_ID);
                          clientID = freeSlot ? freeSlot->socket : NO_CLIENT_ID;
                          
                          if(flags.connectedToRouter)
                          {
                            flags.waitCipstartConnect = true;
                            cipstartConnectClient = dt.client;
                            cipstartConnectClientID = clientID;
                            //flags.cipstartConnectKnownAnswerFound = false;
      
                            currentCommand = cmdCIPSTART;
                            String comm = F("AT+CIPSTART=");
                            comm += clientID;
                            comm += F(",\"TCP\",\"");
                            comm += dt.ip;
                            comm += F("\",");
                            comm += dt.port;
      
                            delete [] clientsQueue[0].ip;
                            clientsQueue[0].ip = NULL;
                  
                            // и отсылаем её
                            sendCommand(comm);
                          } // flags.connectedToRouter
                          else
                          {
                            // не законнекчены к роутеру, не можем устанавливать внешние соединения!!!
                            removeClientFromQueue(dt.client);
                            dt.client->bind(clientID);
                            notifyClientConnected(*(dt.client),false,CT_ERROR_CANT_CONNECT);
                            dt.client->release();
                            
                          }
                      } // if(!cipstartConnectClient)
                    }
                    break; // actionConnect

                    case actionWrite:
                    {
                      // хочет отослать данные

                      currentCommand = cmdCIPSEND;

                      size_t dataSize;
                      uint8_t* buffer = dt.client->getBuffer(dataSize);
                      dt.client->releaseBuffer();

                      clientsQueue[0].data = buffer;
                      clientsQueue[0].dataLength = dataSize;

                      String command = CIPSEND_COMMAND;
                      command += clientID;
                      command += F(",");
                      command += dataSize;
                      flags.waitForDataWelcome = true; // выставляем флаг, что мы ждём >
                      
                      sendCommand(command);
                      
                    }
                    break; // actionWrite
                  } // switch
              }
              else
              {
                timer = millis(); // обновляем таймер в режиме ожидания, поскольку мы не ждём ответа на команды

                // у нас прошла инициализация, нет клиентов в очереди на обработку, следовательно - мы можем проверять модем на зависание
                // тут смотрим - не пора ли послать команду для проверки на зависание. Слишком часто её звать нельзя, что очевидно,
                // поэтому мы будем звать её минимум раз в N секунд. При этом следует учитывать, что мы всё равно должны звать эту команду
                // вне зависимости от того, откликается ли ESP или нет, т.к. в этой команде мы проверяем - есть ли соединение с роутером.
                // эту проверку надо делать периодически, чтобы форсировать переподсоединение, если роутер отвалился.
                static uint32_t hangTimer = 0;
                if(millis() - hangTimer > WIFI_AVAILABLE_CHECK_TIME)
                {
                  hangTimer = millis();
                  sendCommand(cmdCheckModemHang);
                  
                } // if
                
              } // else
            } // else inited
        }
        break; // espIdle

        case espWaitAnswer: // ждём ответа от модема на посланную ранее команду (функция sendCommand переводит конечный автомат в эту ветку)
        {
          // команда, которую послали - лежит в currentCommand, время, когда её послали - лежит в timer.
              if(hasAnswerLine)
              {                
                // есть строка ответа от модема, можем её анализировать, в зависимости от посланной команды (лежит в currentCommand)
                switch(currentCommand)
                {
                  case cmdNone:
                  {
                    // ничего не делаем
                  }
                  break; // cmdNone

                  case cmdPING:
                  {
                    // ждали ответа на пинг
                    if(isKnownAnswer(thisCommandLine,knownAnswer))
                    {
                      flags.specialCommandDone = true;
                      specialCommandResults.push_back(new String(thisCommandLine.c_str()));
                      machineState = espIdle; // переходим к следующей команде
                    }
                    
                  }
                  break; // cmdPING

                  case cmdCIFSR:
                  {
                    // ждём выполнения команды CIFSR
                    if(isKnownAnswer(thisCommandLine,knownAnswer))
                    {
                      flags.specialCommandDone = true;
                      machineState = espIdle; // переходим к следующей команде
                    }
                    else if(thisCommandLine.startsWith(F("+CIFSR:")))
                    {
                      specialCommandResults.push_back(new String(thisCommandLine.c_str()));
                    }
                  }
                  break; // cmdCIFSR

                  case cmdCIPCLOSE:
                  {
                    // отсоединялись. Здесь не надо ждать известного ответа, т.к. ответ может придти асинхронно
                      if(clientsQueue.size())
                      {
                        // клиент отсоединён, ставим ему соответствующий флаг, освобождаем его и удаляем из очереди
                        TransportClientQueueData dt = clientsQueue[0];

                        CoreTransportClient* thisClient = dt.client;
                        removeClientFromQueue(thisClient);

                      } // if(clientsQueue.size()) 
                      
                        machineState = espIdle; // переходим к следующей команде

                  }
                  break; // cmdCIPCLOSE

                  case cmdCIPSTART:
                  {
                    // соединялись, коннект у нас только с внутреннего соединения, поэтому в очереди лежит по-любому
                    // указатель на связанного с нами клиента, который использует внешний пользователь транспорта
                    
                        if(isKnownAnswer(thisCommandLine,knownAnswer))
                        {
                          /*
                          if(knownAnswer == kaOK || knownAnswer == kaError || knownAnswer == kaAlreadyConnected)
                          {
                            flags.cipstartConnectKnownAnswerFound = true;
                          }
                          */
                          if(knownAnswer == kaOK)
                          {
                            // законнектились удачно, перед этим пришла строка вида 0,CONNECT
                            if(clientsQueue.size())
                            {
                               TransportClientQueueData dt = clientsQueue[0];
                               CoreTransportClient* thisClient = dt.client;
                               removeClientFromQueue(thisClient);
                               thisClient->bind(cipstartConnectClientID);
                               CoreTransportClient* client = getClient(cipstartConnectClientID);            
                               notifyClientConnected(*client,true,CT_ERROR_NONE);                               
                               flags.waitCipstartConnect = false;
                               cipstartConnectClient = NULL;
                            }
                          }
                          else
                          {
                              
                            if(clientsQueue.size())
                            {
                                DBG(F("ESP: Client connect ERROR, received: "));
                                DBGLN(thisCommandLine);
                               
                               TransportClientQueueData dt = clientsQueue[0];

                               CoreTransportClient* thisClient = dt.client;
                               removeClientFromQueue(thisClient);

                               // если мы здесь, то мы получили ERROR или ALREADY CONNECTED сразу после команды
                               // AT+CIPSTART. Это значит, что пока у внешнего клиента нет ID, мы его должны
                               // временно назначить, сообщить клиенту, и освободить этот ID.
                               thisClient->bind(cipstartConnectClientID);                               
                               CoreTransportClient* client = getClient(cipstartConnectClientID);            
                               notifyClientConnected(*client,false,CT_ERROR_CANT_CONNECT);
                               thisClient->release();
                            }

                            // ошибка соединения, строка ID,CONNECT нас уже не волнует
                            flags.waitCipstartConnect = false;
                            cipstartConnectClient = NULL;
                            
                          } // else
                          machineState = espIdle; // переходим к следующей команде
                        }                    
                    
                  }
                  break; // cmdCIPSTART


                  case cmdWaitSendDone:
                  {
                    // дожидаемся результата отсыла данных
                      
                      if(isKnownAnswer(thisCommandLine,knownAnswer))
                      {
                        if(knownAnswer == kaSendOk)
                        {
                          // send ok
                          if(clientsQueue.size())
                          {
                             TransportClientQueueData dt = clientsQueue[0];
                             
                             CoreTransportClient* thisClient = dt.client;
                             removeClientFromQueue(thisClient);

                             // очищаем данные у клиента
                             thisClient->clear();

                             notifyDataWritten(*thisClient,CT_ERROR_NONE);
                          }                     
                        } // send ok
                        else
                        {
                          // send fail
                          if(clientsQueue.size())
                          {
                             TransportClientQueueData dt = clientsQueue[0];

                             CoreTransportClient* thisClient = dt.client;
                             removeClientFromQueue(thisClient);
                                                          
                             // очищаем данные у клиента
                             thisClient->clear();
                             
                             notifyDataWritten(*thisClient,CT_ERROR_CANT_WRITE);
                          }                     
                        } // else send fail
  
                        machineState = espIdle; // переходим к следующей команде
                        
                      } // if(isKnownAnswer(*wiFiReceiveBuff,knownAnswer))
                       

                  }
                  break; // cmdWaitSendDone

                  case cmdCIPSEND:
                  {
                    // тут отсылали запрос на запись данных с клиента
                    if(thisCommandLine == F(">"))
                    {
                       // дождались приглашения, можем писать в ESP
                       // тут пишем напрямую
                       if(clientsQueue.size())
                       {
                          // говорим, что ждём окончания отсыла данных
                          currentCommand = cmdWaitSendDone;                          
                          TransportClientQueueData dt = clientsQueue[0];

                            DBG(F("ESP: > RECEIVED, CLIENT #"));
                            DBG(String(dt.client->socket));
                            DBG(F("; LENGTH="));
                            DBGLN(String(dt.dataLength));

                          for(size_t kk=0;kk<dt.dataLength;kk++)
                          {
                            workStream->write(dt.data[kk]);
                            readFromStream();

                          }
                          
                          delete [] clientsQueue[0].data;
                          delete [] clientsQueue[0].ip;
                          clientsQueue[0].data = NULL;
                          clientsQueue[0].ip = NULL;
                          clientsQueue[0].dataLength = 0;

                          // очищаем данные у клиента сразу после отсыла
                          dt.client->clear();
                       }
                    } // if
                    else
                    if(thisCommandLine.indexOf(F("FAIL")) != -1 || thisCommandLine.indexOf(F("ERROR")) != -1)
                    {
                       // всё плохо, не получилось ничего записать
                      if(clientsQueue.size())
                      {
                         
                         TransportClientQueueData dt = clientsQueue[0];

                         CoreTransportClient* thisClient = dt.client;
                         removeClientFromQueue(thisClient);

                          DBG(F("ESP: CLIENT WRITE ERROR #"));
                          DBGLN(String(thisClient->socket));

                         // очищаем данные у клиента
                         thisClient->clear();

                         notifyDataWritten(*thisClient,CT_ERROR_CANT_WRITE);
                        
                      }                     
                      
                      machineState = espIdle; // переходим к следующей команде
              
                    } // else can't write
                    
                  }
                  break; // cmdCIPSEND
                  
                  case cmdWantReady: // ждём загрузки модема в ответ на команду AT+RST
                  {
                    if(isESPBootFound(thisCommandLine))
                    {
                        DBGLN(F("ESP: BOOT FOUND!!!"));
                      
                      machineState = espIdle; // переходим к следующей команде
                    }
                  }
                  break; // cmdWantReady

                  case cmdEchoOff:
                  {
                    if(isKnownAnswer(thisCommandLine,knownAnswer))
                    {
                      DBGLN(F("ESP: Echo OFF command processed."));
                      machineState = espIdle; // переходим к следующей команде
                    }
                  }
                  break; // cmdEchoOff

                  case cmdCWMODE:
                  {
                    if(isKnownAnswer(thisCommandLine,knownAnswer))
                    {
                      DBGLN(F("ESP: CWMODE command processed."));
                      machineState = espIdle; // переходим к следующей команде
                    }
                  }
                  break; // cmdCWMODE

                  case cmdCWSAP:
                  {
                    if(isKnownAnswer(thisCommandLine,knownAnswer))
                    {
                      DBGLN(F("ESP: CWSAP command processed."));
                      machineState = espIdle; // переходим к следующей команде
                    }  
                  }
                  break; // cmdCWSAP

                  case cmdCWJAP:
                  {                    
                    if(isKnownAnswer(thisCommandLine,knownAnswer))
                    {

                      machineState = espIdle; // переходим к следующей команде

                      if(knownAnswer != kaOK)
                      {
                        // ошибка подсоединения к роутеру
                        if(++failConnectToRouterAttempts < WIFI_CONNECT_ATTEMPTS)
                        {
                          DBGLN(F("ESP: CWJAP command FAIL, RESTART!"));
                          restart();
                        }
                      }
                      else
                      {
                        // подсоединились успешно
                        DBGLN(F("ESP: CWJAP command processed."));
                      }
                  
                    }  
                  }
                  break; // cmdCWJAP

                  case cmdCWQAP:
                  {                    
                    if(isKnownAnswer(thisCommandLine,knownAnswer))
                    {
                      DBGLN(F("ESP: CWQAP command processed."));
                      machineState = espIdle; // переходим к следующей команде
                    }  
                  }
                  break; // cmdCWQAP

                  case cmdCIPMODE:
                  {                    
                    if(isKnownAnswer(thisCommandLine,knownAnswer))
                    {
                      DBGLN(F("ESP: CIPMODE command processed."));
                      machineState = espIdle; // переходим к следующей команде
                    }  
                  }
                  break; // cmdCIPMODE

                  case cmdCIPMUX:
                  {                    
                    if(isKnownAnswer(thisCommandLine,knownAnswer))
                    {
                      DBGLN(F("ESP: CIPMUX command processed."));
                      machineState = espIdle; // переходим к следующей команде
                    }  
                  }
                  break; // cmdCIPMUX
                  
                  case cmdCIPSERVER:
                  {                    
                    if(isKnownAnswer(thisCommandLine,knownAnswer))
                    {
                      DBGLN(F("ESP: CIPSERVER command processed."));
                      machineState = espIdle; // переходим к следующей команде
                    }  
                  }
                  break; // cmdCIPSERVER

                  case cmdCheckModemHang:
                  {                    
                    if(isKnownAnswer(thisCommandLine,knownAnswer))
                    {
                      DBGLN(F("ESP: ESP answered and available."));
                      machineState = espIdle; // переходим к следующей команде

                       if(flags.wantReconnect)
                       {
                          // требуется переподсоединение к роутеру. Проще всего это сделать вызовом restart - тогда весь цикл пойдёт с начала
                          restart();

                          // чтобы часто не дёргать реконнект - мы говорим, что после рестарта надо подождать 5 секунд перед тем, как обрабатывать следующую команду
                          DBGLN(F("ESP: Wait 5 seconds before reconnect..."));
                          flags.onIdleTimer = true;
                          idleTimer = millis();
                          idleTime = 5000;
                          
                       } // if(flags.wantReconnect)
                      
                    } // if(isKnownAnswer

                     if(thisCommandLine == F("No AP"))
                     {
                        if(Settings.shouldConnectToRouter())
                        {
                          DBGLN(F("ESP: No connect to router, want to reconnect..."));
                          // нет соединения с роутером, надо переподсоединиться, как только это будет возможно.
                          flags.wantReconnect = true;
                          flags.connectedToRouter = false;
                        }
                      
                     } // if
                      else
                      {
                        // на случай, когда ESP не выдаёт WIFI CONNECTED в порт - проверяем статус коннекта тут,
                        // как признак, что строчка содержит ID нашей сети, проще говоря - не равна No AP
                        if(thisCommandLine.startsWith(F("+CWJAP")))
                          flags.connectedToRouter = true;
                        
                      }
                    
                  }
                  break; // cmdCheckModemHang
                                    
                } // switch

                
              } // if(hasAnswerLine)
              
         
        }
        break; // espWaitAnswer

        case espReboot:
        {
          // ждём перезагрузки модема
          uint32_t powerOffTime = WIFI_REBOOT_TIME;
          
          if(millis() - timer > powerOffTime)
          {
              DBGLN(F("ESP: turn power ON!"));
            
            #ifdef USE_WIFI_REBOOT
              Settings.espPower(true);
            #endif

            machineState = espWaitInit;
            timer = millis();
            
          } // if
        }
        break; // espReboot

        case espWaitInit:
        {
          uint32_t waitTime = WIFI_WAIT_AFTER_REBOOT_TIME;
          if(millis() - timer > waitTime)
          {            
            restart();
            DBGLN(F("ESP: inited after reboot!"));
          } // 
        }
        break;
      
    } // switch

  } // if(!flags.onIdleTimer)

    
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::clearSpecialCommandResults()
{
  for(size_t i=0;i<specialCommandResults.size();i++)
  {
    delete specialCommandResults[i];
  }
  specialCommandResults.clear();
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::begin()
{

   DBGLN(F("ESP: begin."));
    
  workStream = &WIFI_SERIAL;
  WIFI_SERIAL.begin(WIFI_BAUD_RATE);

  // вычитываем старый мусор, т.к. нас могли выключить жёстко
  while(WIFI_SERIAL.available())
    WIFI_SERIAL.read();

  restart();

  #ifdef USE_WIFI_REBOOT
    // есть пин, который надо использовать при зависании
    Settings.espPower(false);
    machineState = espReboot;
  #endif

    DBGLN(F("ESP: started."));

}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::restart()
{
  // очищаем входной буфер
  receiveBuffer.clear();

  // очищаем очередь клиентов, заодно им рассылаем события
  clearClientsQueue(true);

  // т.к. мы ничего не инициализировали - говорим, что мы не готовы предоставлять клиентов
  flags.ready = false;
  flags.isAnyAnswerReceived = false;
  flags.waitForDataWelcome = false;
  flags.connectedToRouter = false;
  flags.wantReconnect = false;
  flags.onIdleTimer = false;
  
  timer = millis();

  flags.waitCipstartConnect = false; // не ждёт соединения внешнего клиента
  cipstartConnectClient = NULL;

  currentCommand = cmdNone;
  machineState = espIdle;

  // инициализируем очередь командами по умолчанию
 createInitCommands(true);
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::createInitCommands(bool addResetCommand)
{  
  // очищаем очередь команд
  clearInitCommands();


 
  if(Settings.shouldConnectToRouter()) // коннектимся к роутеру
    initCommandsQueue.push_back(cmdCWJAP); // коннектимся к роутеру совсем в конце
  else  
    initCommandsQueue.push_back(cmdCWQAP); // отсоединяемся от роутера
    
  initCommandsQueue.push_back(cmdCIPSERVER); // сервер поднимаем в последнюю очередь
  initCommandsQueue.push_back(cmdCIPMUX); // разрешаем множественные подключения
  initCommandsQueue.push_back(cmdCIPMODE); // устанавливаем режим работы
  initCommandsQueue.push_back(cmdCWSAP); // создаём точку доступа
  initCommandsQueue.push_back(cmdCWMODE); // // переводим в смешанный режим
  initCommandsQueue.push_back(cmdEchoOff); // выключаем эхо
  
  if(addResetCommand)
    initCommandsQueue.push_back(cmdWantReady); // надо получить ready от модуля путём его перезагрузки      
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::clearInitCommands()
{
  initCommandsQueue.clear();
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::clearClientsQueue(bool raiseEvents)
{  
  // тут попросили освободить очередь клиентов.
  // для этого нам надо выставить каждому клиенту флаг того, что он свободен,
  // плюс - сообщить, что текущее действие над ним не удалось.  

    for(size_t i=0;i<clientsQueue.size();i++)
    {
        TransportClientQueueData dt = clientsQueue[i];
        delete [] dt.data;
        delete [] dt.ip;

        // если здесь в очереди есть хоть один клиент с неназначенным ID (ждёт подсоединения) - то в события он не придёт,
        // т.к. там сравнивается по назначенному ID. Поэтому мы назначаем ID клиенту в первый свободный слот.
        if(dt.client->socket == NO_CLIENT_ID)
        {
          CoreTransportClient* cl = getClient(NO_CLIENT_ID);
          if(cl)
            dt.client->bind(cl->socket);
        }
        
        if(raiseEvents)
        {
          switch(dt.action)
          {
            case actionDisconnect:
                // при дисконнекте всегда считаем, что ошибок нет
                notifyClientConnected(*(dt.client),false,CT_ERROR_NONE);
            break;
  
            case actionConnect:
                // если было запрошено соединение клиента с адресом - говорим, что соединиться не можем
                notifyClientConnected(*(dt.client),false,CT_ERROR_CANT_CONNECT);
            break;
  
            case actionWrite:
              // если попросили записать данные - надо сообщить подписчикам, что не можем записать данные
              notifyDataWritten(*(dt.client),CT_ERROR_CANT_WRITE);
              notifyClientConnected(*(dt.client),false,CT_ERROR_NONE);
            break;
          } // switch
          

        } // if(raiseEvents)

        dt.client->clear();
        
    } // for

  clientsQueue.clear();

}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreESPTransport::isClientInQueue(CoreTransportClient* client, TransportClientAction action)
{
  for(size_t i=0;i<clientsQueue.size();i++)
  {
    if(clientsQueue[i].client == client && clientsQueue[i].action == action)
      return true;
  }

  return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::addClientToQueue(CoreTransportClient* client, TransportClientAction action, const char* ip, uint16_t port)
{
  while(isClientInQueue(client, action))
  {
      DBG(F("ESP: Client #"));
      DBG(String(client->socket));
      DBG(F(" with same action already in queue, ACTION="));
      DBG(String(action));
      DBGLN(F(" - remove that client!"));
	  
      removeClientFromQueue(client,action);
  }

    TransportClientQueueData dt;
    dt.client = client;
    dt.action = action;
    
    dt.ip = NULL;
    if(ip)
    {
      dt.ip = new char[strlen(ip)+1];
      strcpy(dt.ip,ip);
    }
    dt.port = port;

    clientsQueue.push_back(dt);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::removeClientFromQueue(CoreTransportClient* client, TransportClientAction action)
{
  
  for(size_t i=0;i<clientsQueue.size();i++)
  {
    if(clientsQueue[i].client == client && clientsQueue[i].action == action)
    {
      delete [] clientsQueue[i].ip;
      delete [] clientsQueue[i].data;
      client->clear();
      
        for(size_t j=i+1;j<clientsQueue.size();j++)
        {
          clientsQueue[j-1] = clientsQueue[j];
        }
        
        clientsQueue.pop();
        break;
    } // if
    
  } // for  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::removeClientFromQueue(CoreTransportClient* client)
{
  
  for(size_t i=0;i<clientsQueue.size();i++)
  {
    if(clientsQueue[i].client == client)
    {
      
      delete [] clientsQueue[i].ip;
      delete [] clientsQueue[i].data;
      client->clear();
      
        for(size_t j=i+1;j<clientsQueue.size();j++)
        {
          clientsQueue[j-1] = clientsQueue[j];
        }
        
        clientsQueue.pop();
        break;
    } // if
    
  } // for
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::beginWrite(CoreTransportClient& client)
{
  
  // добавляем клиента в очередь на запись
  addClientToQueue(&client, actionWrite);

  // клиент добавлен, теперь при обновлении транспорта мы начнём работать с записью в поток с этого клиента
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::beginConnect(CoreTransportClient& client, const char* ip, uint16_t port)
{

  if(client.connected())
  {
    
    DBGLN(F("ESP: client already connected!"));
    return;
    
  }
  
  // добавляем клиента в очередь на соединение
  addClientToQueue(&client, actionConnect, ip, port);

  // клиент добавлен, теперь при обновлении транспорта мы начнём работать с соединением клиента
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreESPTransport::beginDisconnect(CoreTransportClient& client)
{
  if(!client.connected())
  {
    return;
  }

  // добавляем клиента в очередь на соединение
  addClientToQueue(&client, actionDisconnect);

  // клиент добавлен, теперь при обновлении транспорта мы начнём работать с отсоединением клиента
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreESPTransport::ready()
{
  return flags.ready && flags.isAnyAnswerReceived; // если мы полностью инициализировали ESP - значит, можем работать
}
//--------------------------------------------------------------------------------------------------------------------------------------

