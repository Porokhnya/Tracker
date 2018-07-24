
#include <Arduino.h>                                            // required before wiring_private.h
#include "wiring_private.h"                                     // pinPeripheral() function
#include "Wire.h"
#include <SPI.h>
#include <LCD5110_Graph.h>  
#include "Adafruit_MCP23017.h"
#include "ds3231.h"
#include "SdFat.h"
#include "Adafruit_Si7021.h"

#define Serial SERIAL_PORT_USBVIRTUAL                          // USB SAMD21G18A

Uart Serial2(&sercom2, 3, 4, SERCOM_RX_PAD_1, UART_TX_PAD_0);  // Подключить Serial2
void SERCOM2_Handler()                                         // Подключить Serial2 
{
	Serial2.IrqHandler();
}


bool PWR_LCD = true;                                         // Признак включения подсветки дисплея
bool PWR_ESP = true;                                         // Признак включения ESP8266
Adafruit_Si7021 sensor = Adafruit_Si7021();

//----------------- Переменные для проверки часов ------------
#define BUFF_MAX 128
uint8_t time[8];
char recv[BUFF_MAX];
unsigned int recv_size = 0;
unsigned long prev, interval = 1000;
void parse_cmd(char *cmd, int cmdsize);
//----------------------------------------------------

#define  CLK  7  // Serial clock out (CLK)
#define  DIN  A4  // Serial data out (DIN)
#define  DC  5    // Data/Command select (D/C)
#define  CS  31   // LCD chip select (CS)
#define  RST  30  // LCD reset (RST) 

LCD5110 myGLCD(CLK, DIN, DC, RST, CS);              // объявляем номера пинов LCD

extern uint8_t SmallFont[];                         // малый шрифт (из библиотеки)
extern uint8_t MediumNumbers[];                     // средний шрифт для цифр (из библиотеки)

#define  ledPin  13                                 // Назначение светодиода на плате
#define  PWR_On_In 38                               // Вход признака включения питания

#define  Key_line_In11 11                           // Линия приема сигнала от 1 ряда кнопок 
#define  Key_line_In12 12                           // Линия приема сигнала от 2 ряда кнопок 

#define  Key_line_Out0 0                            // МСР 0 Линия выдачи сигнала на кнопки
#define  Key_line_Out1 1                            // МСР 1 Линия выдачи сигнала на кнопки
#define  Key_line_Out2 2                            // МСР 2 Линия выдачи сигнала на кнопки
#define  Key_line_Out3 3                            // МСР 3 Линия выдачи сигнала на кнопки

#define  LCD_led 4                                  // МСР 4 Вывод подсветки питания дисплея
#define  PWR_On_Out 5                               // МСР 5 Вывод поддержки включения питания. Отключение питания контроллера                        
#define  PWR_WiFi 6                                 // МСР 4 Вывод управления питанием модуля WiFi
#define  Key_door A2                                // Концевик двери                      
#define  Sensor_temp A1                             // Температурный аналоговый сенсор

bool state_door = false;

extern "C" char *sbrk(int i);                       // Для измерения свободной памяти 

Adafruit_MCP23017 mcp;


//------------------------------------------------------------------------------
// Set USE_SDIO to zero for SPI card access. 
#define USE_SDIO 0
/*
* SD chip select pin.  Common values are:
*/
const uint8_t SD_CHIP_SELECT = 10;
/*
* Set DISABLE_CHIP_SELECT to disable a second SPI device.
* For example, with the Ethernet shield, set DISABLE_CHIP_SELECT
* to 10 to disable the Ethernet controller.
*/
const int8_t DISABLE_CHIP_SELECT = -1;

#if USE_SDIO
// Use faster SdioCardEX
SdFatSdioEX sd;
// SdFatSdio sd;
#else // USE_SDIO
SdFat sd;
#endif  // USE_SDIO

// serial output steam
ArduinoOutStream cout(Serial);

// global for card size
uint32_t cardSize;

// global for card erase size
uint32_t eraseSize;
//------------------------------------------------------------------------------
// store error strings in flash
#define sdErrorMsg(msg) sd.errorPrint(F(msg));
//------------------------------------------------------------------------------
uint8_t cidDmp() {
	cid_t cid;
	if (!sd.card()->readCID(&cid)) {
		sdErrorMsg("readCID failed");
		return false;
	}
	cout << F("\nManufacturer ID: ");
	cout << hex << int(cid.mid) << dec << endl;
	cout << F("OEM ID: ") << cid.oid[0] << cid.oid[1] << endl;
	cout << F("Product: ");
	for (uint8_t i = 0; i < 5; i++) {
		cout << cid.pnm[i];
	}
	cout << F("\nVersion: ");
	cout << int(cid.prv_n) << '.' << int(cid.prv_m) << endl;
	cout << F("Serial number: ") << hex << cid.psn << dec << endl;
	cout << F("Manufacturing date: ");
	cout << int(cid.mdt_month) << '/';
	cout << (2000 + cid.mdt_year_low + 10 * cid.mdt_year_high) << endl;
	cout << endl;
	return true;
}
//------------------------------------------------------------------------------
uint8_t csdDmp() {
	csd_t csd;
	uint8_t eraseSingleBlock;
	if (!sd.card()->readCSD(&csd)) {
		sdErrorMsg("readCSD failed");
		return false;
	}
	if (csd.v1.csd_ver == 0) {
		eraseSingleBlock = csd.v1.erase_blk_en;
		eraseSize = (csd.v1.sector_size_high << 1) | csd.v1.sector_size_low;
	}
	else if (csd.v2.csd_ver == 1) {
		eraseSingleBlock = csd.v2.erase_blk_en;
		eraseSize = (csd.v2.sector_size_high << 1) | csd.v2.sector_size_low;
	}
	else {
		cout << F("csd version error\n");
		return false;
	}
	eraseSize++;
	cout << F("cardSize: ") << 0.000512*cardSize;
	cout << F(" MB (MB = 1,000,000 bytes)\n");

	cout << F("flashEraseSize: ") << int(eraseSize) << F(" blocks\n");
	cout << F("eraseSingleBlock: ");
	if (eraseSingleBlock) {
		cout << F("true\n");
	}
	else {
		cout << F("false\n");
	}
	return true;
}
//------------------------------------------------------------------------------
// print partition table
uint8_t partDmp() {
	mbr_t mbr;
	if (!sd.card()->readBlock(0, (uint8_t*)&mbr)) {
		sdErrorMsg("read MBR failed");
		return false;
	}
	for (uint8_t ip = 1; ip < 5; ip++) {
		part_t *pt = &mbr.part[ip - 1];
		if ((pt->boot & 0X7F) != 0 || pt->firstSector > cardSize) {
			cout << F("\nNo MBR. Assuming Super Floppy format.\n");
			return true;
		}
	}
	cout << F("\nSD Partition Table\n");
	cout << F("part,boot,type,start,length\n");
	for (uint8_t ip = 1; ip < 5; ip++) {
		part_t *pt = &mbr.part[ip - 1];
		cout << int(ip) << ',' << hex << int(pt->boot) << ',' << int(pt->type);
		cout << dec << ',' << pt->firstSector << ',' << pt->totalSectors << endl;
	}
	return true;
}
//------------------------------------------------------------------------------
void volDmp() {
	cout << F("\nVolume is FAT") << int(sd.vol()->fatType()) << endl;
	cout << F("blocksPerCluster: ") << int(sd.vol()->blocksPerCluster()) << endl;
	cout << F("clusterCount: ") << sd.vol()->clusterCount() << endl;
	cout << F("freeClusters: ");
	uint32_t volFree = sd.vol()->freeClusterCount();
	cout << volFree << endl;
	float fs = 0.000512*volFree*sd.vol()->blocksPerCluster();
	cout << F("freeSpace: ") << fs << F(" MB (MB = 1,000,000 bytes)\n");
	cout << F("fatStartBlock: ") << sd.vol()->fatStartBlock() << endl;
	cout << F("fatCount: ") << int(sd.vol()->fatCount()) << endl;
	cout << F("blocksPerFat: ") << sd.vol()->blocksPerFat() << endl;
	cout << F("rootDirStart: ") << sd.vol()->rootDirStart() << endl;
	cout << F("dataStartBlock: ") << sd.vol()->dataStartBlock() << endl;
	if (sd.vol()->dataStartBlock() % eraseSize) {
		cout << F("Data area is not aligned on flash erase boundaries!\n");
		cout << F("Download and use formatter from www.sdcard.org!\n");
	}
}
//------------------------------------------------------------------------------



//-------------------------------------------------------------------------------

int FreeRam() {
	char stack_dummy = 0;
	return &stack_dummy - sbrk(0);
}


// Определяем нажатую клавишу
int num_key = 0;
void test_key()
{
	num_key = 0;
	mcp.digitalWrite(Key_line_Out0, HIGH);
	mcp.digitalWrite(Key_line_Out1, HIGH);
	mcp.digitalWrite(Key_line_Out2, HIGH);
	mcp.digitalWrite(Key_line_Out3, HIGH);

	for (int i = 0; i < 4; i++)
	{
		mcp.digitalWrite(i, LOW);
			if (digitalRead(Key_line_In11) == LOW)
			{
				num_key = 4-i;
				break;
			}
			mcp.digitalWrite(i, HIGH);
	}

	for (int i = 0; i < 4; i++)
	{
		mcp.digitalWrite(i, LOW);
			if (digitalRead(Key_line_In12) == LOW)
			{
				num_key = 7-i;
				break;
			}
	}

	mcp.digitalWrite(Key_line_Out0, LOW);
	mcp.digitalWrite(Key_line_Out1, LOW);
	mcp.digitalWrite(Key_line_Out2, LOW);
	mcp.digitalWrite(Key_line_Out3, LOW);
}


void info()
{
	myGLCD.setFont(SmallFont);                          // задаём размер шрифта
	myGLCD.print("    ", CENTER, 1);                      // Очистить строку 1
	myGLCD.print("FreeRam", LEFT, 1);                      // выводим в строке 1 
	myGLCD.print(String(FreeRam()), RIGHT, 1);                      // выводим в строке 1 

	//Serial.print("Humidity:    "); Serial.print(sensor.readHumidity(), 2);
	//Serial.print("\tTemperature: "); Serial.println(sensor.readTemperature(), 2);
	//myGLCD.print("H:", LEFT, 20);                      // выводим в строке 1 
	//myGLCD.print(String(sensor.readHumidity()), 11, 20);                      // выводим в строке 1 
	//myGLCD.print("T:", 43, 20);                      // выводим в строке 1 
	//myGLCD.print(String(sensor.readTemperature()), RIGHT, 20);                      // выводим в строке 1 
	myGLCD.update();
}



void setup()
{
	Serial.begin(115200);               // USB
	Serial1.begin(115200);             // WiFi
	Serial2.begin(19200);              // Возможно для принтера

	//Assign pins 3 & 4 SERCOM functionality
	pinPeripheral(3, PIO_SERCOM_ALT);    // Настройка Serial2
	pinPeripheral(4, PIO_SERCOM_ALT);    // Настройка Serial2

	delay(1000);

	//while (!Serial) {};

	Wire.begin();
	mcp.begin(1);      // use default address 0

	mcp.pinMode(LCD_led, OUTPUT);             // Настроить подсветку дисплея
	mcp.digitalWrite(LCD_led, LOW);           // Включить подсветку дисплея

	mcp.pinMode(Key_line_Out0, OUTPUT);       // Настроить кнопки
	mcp.pinMode(Key_line_Out1, OUTPUT);       // Настроить кнопки
	mcp.pinMode(Key_line_Out2, OUTPUT);       // Настроить кнопки
	mcp.pinMode(Key_line_Out3, OUTPUT);       // Настроить кнопки

	mcp.pinMode(PWR_On_Out, OUTPUT);          // Настроить поддержку питания
	mcp.pinMode(PWR_WiFi, OUTPUT);            // Настроить поддержку питания WiFi


	mcp.digitalWrite(Key_line_Out0, LOW);
	mcp.digitalWrite(Key_line_Out1, LOW);
	mcp.digitalWrite(Key_line_Out2, LOW);
	mcp.digitalWrite(Key_line_Out3, LOW);

	mcp.digitalWrite(PWR_On_Out, LOW);        // Настроить канал управления питанием
	mcp.digitalWrite(PWR_WiFi, LOW);          // Включить питание WiFi

	myGLCD.InitLCD();                                // инициализация LCD дисплея

	myGLCD.clrScr();                                 // очистка экрана
	myGLCD.setFont(SmallFont);                       // задаём размер шрифта

	pinMode(PWR_On_In, INPUT);
	digitalWrite(PWR_On_In, HIGH);
	pinMode(Key_door, INPUT);


	if (digitalRead(PWR_On_In) == LOW) mcp.digitalWrite(PWR_On_Out, HIGH); // Проверить кнопку питания. Если нажата - включить поддержку

	Serial.print("FreeRam");
	Serial.println(FreeRam());
	Serial.println("\r\n==================");

	// ---------------------------- Настраиваем часы -------------------------------------------------
	DS3231_init(DS3231_INTCN);
	memset(recv, 0, BUFF_MAX);

	//----------------------------- Подключить прерывание от кнопок ------------------------
	attachInterrupt(Key_line_In11, test_key, FALLING);
	attachInterrupt(Key_line_In12, test_key, FALLING);

	//-------------------------------------------------------------------------
	//-------------------------- Настроить SD --------------------------------------------------

	// use uppercase in hex and use 0X base prefix
	cout << uppercase << showbase << endl;

	// F stores strings in flash to save RAM
	cout << F("SdFat version: ") << SD_FAT_VERSION << endl;
#if !USE_SDIO  
	if (DISABLE_CHIP_SELECT < 0) {
		cout << F(
			"\nAssuming the SD is the only SPI device.\n"
			"Edit DISABLE_CHIP_SELECT to disable another device.\n");
	}
	else {
		cout << F("\nDisabling SPI device on pin ");
		cout << int(DISABLE_CHIP_SELECT) << endl;
		pinMode(DISABLE_CHIP_SELECT, OUTPUT);
		digitalWrite(DISABLE_CHIP_SELECT, HIGH);
	}
	cout << F("\nAssuming the SD chip select pin is: ") << int(SD_CHIP_SELECT);
	cout << F("\nEdit SD_CHIP_SELECT to change the SD chip select pin.\n");
#endif  // !USE_SDIO  


	

	SD_info();
	myGLCD.print("SDcard", LEFT, 10);                      // выводим в строке 1 
	myGLCD.print(String(0.000512*cardSize), RIGHT, 10);                      // выводим в строке 1 

	//----------------------------- Настроить датчик -------------------------------------------------
	Serial.println("\nSi7021 test!");

	if (!sensor.begin()) {
		Serial.println("Did not find Si7021 sensor!");
		//while (true);
	}

	//------------------------------------------------------------------------------

	Serial.println("==============================");
	Serial.println();
	Serial.println();
}

void loop(void)
{
	// read from port 1, send to port 0:
	if (Serial1.available()) {
		int inByte = Serial1.read();
		Serial.write(inByte);
	}

	// read from port 0, send to port 1:
	if (Serial.available()) {
		int inByte = Serial.read();
		Serial1.write(inByte);
	}

	if (digitalRead(PWR_On_In) == LOW) mcp.digitalWrite(PWR_On_Out, HIGH);   // Проверить кнопку питания. Если нажата - включить поддержку

	if (num_key == 7)                                                        // Выключить/включить питание ESP. Подключена только для тестирования
	{
		mcp.digitalWrite(PWR_WiFi, PWR_ESP);
		PWR_ESP = !PWR_ESP;
		//num_key = 0;
	}
	else if (num_key == 6)                                                   // Выключить/включить подсветку дисплея  
	{
		mcp.digitalWrite(LCD_led, PWR_LCD);
		PWR_LCD = !PWR_LCD;
	}
	else if (num_key == 5)                                                  // Выключить питание. Подключена только для тестирования
	{
		mcp.digitalWrite(LCD_led, HIGH);
		mcp.digitalWrite(PWR_WiFi, HIGH);
		mcp.digitalWrite(PWR_On_Out, LOW);
	}
	if(num_key != 0)                                                  // Вывети в порт номер нажатой кнопки. Для тестирования.
	{
		Serial.println(num_key);
		myGLCD.print(String(num_key), RIGHT, 20);                      // выводим в строке 1 
		myGLCD.update();
		num_key = 0;
	}

	// Проверить концевик двери
	if (digitalRead(Key_door) == LOW)
	{
		//state_door = true;
		myGLCD.print("On ", RIGHT, 40);
	}
	else
	{
		//state_door = false;
		myGLCD.print("Off", RIGHT, 40);
	}

	int sensor = analogRead(Sensor_temp);
	myGLCD.print("    ", RIGHT, 30);
	myGLCD.print(String(sensor), RIGHT, 30);

	// ---------------------------- Отобразить время  -------------------------------------------------
	char in;
	char buff[BUFF_MAX];
	unsigned long now = millis();
	struct ts t;

	// show time once in a while
	if ((now - prev > interval)) 
	{
		DS3231_get(&t);

		// there is a compile time option in the library to include unixtime support
#ifdef CONFIG_UNIXTIME
		snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d %ld", t.year,
			t.mon, t.mday, t.hour, t.min, t.sec, t.unixtime);
#else
	/*	snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d", t.year,
			t.mon, t.mday, t.hour, t.min, t.sec);
*/
		snprintf(buff, BUFF_MAX, "%02d:%02d:%02d", t.hour, t.min, t.sec);
		myGLCD.print(buff, LEFT, 30);                        // выводим в строке 2 
		snprintf(buff, BUFF_MAX, "%d.%02d.%02d", t.year,t.mon, t.mday);
		myGLCD.print(buff, LEFT, 40);                        // выводим в строке 2 
		myGLCD.update();
		info();

#endif

		prev = now;
	}

	// ++++++++++++++++++++ Настройка часов через Serial ++++++++++++++++++++++++++++

	//if (Serial.available() > 0) 
	//{
	//	in = Serial.read();

	//	if ((in == 10 || in == 13) && (recv_size > 0)) {
	//		parse_cmd(recv, recv_size);
	//		recv_size = 0;
	//		recv[0] = 0;
	//	}
	//	else if (in < 48 || in > 122) {
	//		;       // ignore ~[0-9A-Za-z]
	//	}
	//	else if (recv_size > BUFF_MAX - 2) {   // drop lines that are too long
	//										   // drop
	//		recv_size = 0;
	//		recv[0] = 0;
	//	}
	//	else if (recv_size < BUFF_MAX - 2) {
	//		recv[recv_size] = in;
	//		recv[recv_size + 1] = 0;
	//		recv_size += 1;
	//	}

	//}
}

void parse_cmd(char *cmd, int cmdsize)
{
	uint8_t i;
	uint8_t reg_val;
	char buff[BUFF_MAX];
	struct ts t;

	//snprintf(buff, BUFF_MAX, "cmd was '%s' %d\n", cmd, cmdsize);
	//Serial.print(buff);

	// TssmmhhWDDMMYYYY aka set time
	if (cmd[0] == 84 && cmdsize == 16) {
		//T355720619112011
		t.sec = inp2toi(cmd, 1);
		t.min = inp2toi(cmd, 3);
		t.hour = inp2toi(cmd, 5);
		t.wday = cmd[7] - 48;
		t.mday = inp2toi(cmd, 8);
		t.mon = inp2toi(cmd, 10);
		t.year = inp2toi(cmd, 12) * 100 + inp2toi(cmd, 14);
		DS3231_set(t);
		Serial.println("OK");
	}
	else if (cmd[0] == 49 && cmdsize == 1) {  // "1" get alarm 1
		DS3231_get_a1(&buff[0], 59);
		Serial.println(buff);
	}
	else if (cmd[0] == 50 && cmdsize == 1) {  // "2" get alarm 1
		DS3231_get_a2(&buff[0], 59);
		Serial.println(buff);
	}
	else if (cmd[0] == 51 && cmdsize == 1) {  // "3" get aging register
		Serial.print("aging reg is ");
		Serial.println(DS3231_get_aging(), DEC);
	}
	else if (cmd[0] == 65 && cmdsize == 9) {  // "A" set alarm 1
		DS3231_set_creg(DS3231_INTCN | DS3231_A1IE);
		//ASSMMHHDD
		for (i = 0; i < 4; i++) {
			time[i] = (cmd[2 * i + 1] - 48) * 10 + cmd[2 * i + 2] - 48; // ss, mm, hh, dd
		}
		uint8_t flags[5] = { 0, 0, 0, 0, 0 };
		DS3231_set_a1(time[0], time[1], time[2], time[3], flags);
		DS3231_get_a1(&buff[0], 59);
		Serial.println(buff);
	}
	else if (cmd[0] == 66 && cmdsize == 7) {  // "B" Set Alarm 2
		DS3231_set_creg(DS3231_INTCN | DS3231_A2IE);
		//BMMHHDD
		for (i = 0; i < 4; i++) {
			time[i] = (cmd[2 * i + 1] - 48) * 10 + cmd[2 * i + 2] - 48; // mm, hh, dd
		}
		uint8_t flags[5] = { 0, 0, 0, 0 };
		DS3231_set_a2(time[0], time[1], time[2], flags);
		DS3231_get_a2(&buff[0], 59);
		Serial.println(buff);
	}
	else if (cmd[0] == 67 && cmdsize == 1) {  // "C" - get temperature register
		Serial.print("temperature reg is ");
		Serial.println(DS3231_get_treg(), DEC);
	}
	else if (cmd[0] == 68 && cmdsize == 1) {  // "D" - reset status register alarm flags
		reg_val = DS3231_get_sreg();
		reg_val &= B11111100;
		DS3231_set_sreg(reg_val);
	}
	else if (cmd[0] == 70 && cmdsize == 1) {  // "F" - custom fct
		reg_val = DS3231_get_addr(0x5);
		Serial.print("orig ");
		Serial.print(reg_val, DEC);
		Serial.print("month is ");
		Serial.println(bcdtodec(reg_val & 0x1F), DEC);
	}
	else if (cmd[0] == 71 && cmdsize == 1) {  // "G" - set aging status register
		DS3231_set_aging(0);
	}
	else if (cmd[0] == 83 && cmdsize == 1) {  // "S" - get status register
		Serial.print("status reg is ");
		Serial.println(DS3231_get_sreg(), DEC);
	}
	else {
		Serial.print("unknown command prefix ");
		Serial.println(cmd[0]);
		Serial.println(cmd[0], DEC);
	}
}


void SD_info()
{

	uint32_t t = millis();
#if USE_SDIO
	if (!sd.cardBegin()) {
		sdErrorMsg("\ncardBegin failed");
		return;
	}
#else  // USE_SDIO
	// Initialize at the highest speed supported by the board that is
	// not over 50 MHz. Try a lower speed if SPI errors occur.
	if (!sd.cardBegin(SD_CHIP_SELECT, SD_SCK_MHZ(50))) {
		sdErrorMsg("cardBegin failed");
		return;
	}
#endif  // USE_SDIO 
	t = millis() - t;

	cardSize = sd.card()->cardSize();
	if (cardSize == 0) {
		sdErrorMsg("cardSize failed");
		return;
	}
	cout << F("\ninit time: ") << t << " ms" << endl;
	cout << F("\nCard type: ");
	switch (sd.card()->type()) {
	case SD_CARD_TYPE_SD1:
		cout << F("SD1\n");
		break;

	case SD_CARD_TYPE_SD2:
		cout << F("SD2\n");
		break;

	case SD_CARD_TYPE_SDHC:
		if (cardSize < 70000000) {
			cout << F("SDHC\n");
		}
		else {
			cout << F("SDXC\n");
		}
		break;

	default:
		cout << F("Unknown\n");
	}
	if (!cidDmp()) {
		return;
	}
	if (!csdDmp()) {
		return;
	}
	uint32_t ocr;
	if (!sd.card()->readOCR(&ocr)) {
		sdErrorMsg("\nreadOCR failed");
		return;
	}
	cout << F("OCR: ") << hex << ocr << dec << endl;
	if (!partDmp()) {
		return;
	}
	if (!sd.fsBegin()) {
		sdErrorMsg("\nFile System initialization failed.\n");
		return;
	}
	volDmp();
}

