/*
Copyright (c) 2012 Arduino.  All right reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#define ARDUINO_MAIN
#include "variant.h"
#include "wiring_private.h"  // pinPeripheral() function
#include "Arduino.h" 
#include <stdio.h>
#include <SPI.h>
#include "wiring_constants.h"
#include <adk.h>


Uart Serial2(&sercom2, 3, 4, SERCOM_RX_PAD_1, UART_TX_PAD_0);  // Подключить Serial2
void SERCOM2_Handler()                                         // Подключить Serial2 
{
	Serial2.IrqHandler();
}
//#define SERIAL_PORT_MONITOR Serial2
#if defined(LED_BUILTIN)
#define LED LED_BUILTIN // Use built in LED
#else
#define LED 13 // Set to something here that makes sense for your board.
#endif


USBHost usb;
ADK adk(&usb, "TKJElectronics", // Manufacturer Name
	"ArduinoBlinkLED", // Model Name
	"Example sketch for the USB Host Shield", // Description (user-visible string)
	"1.0", // Version
	"http://www.tkjelectronics.dk/uploads/ArduinoBlinkLED.apk", // URL (web page to visit if no installed apps support the accessory)
	"123456789"); // Serial Number (optional)

uint32_t timer;
bool connected;




#define RCVSIZE 128
uint32_t next_time;



void setup(void)
{
	Serial2.begin(115200);              // Возможно принтер
	SerialUSB.begin(115200);              // Возможно принтер
	Serial.begin(115200);              // Возможно принтер
										//Assign pins 3 & 4 SERCOM functionality
	pinPeripheral(3, PIO_SERCOM_ALT);    // Настройка Serial2
	pinPeripheral(4, PIO_SERCOM_ALT);    // Настройка Serial2


	while (!Serial2); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
	Serial2.println("Controller Program started");

	if (usb.Init() == -1) {
		Serial2.print("\r\nOSCOKIRQ failed to assert");
		while (1); // halt
	}
	pinMode(LED, OUTPUT);
	Serial2.print("\r\nArduino Blink LED Started");
	 
	delay(20);

	next_time = millis() + 10000;


}

void loop()
{

	//char helloworld[] = {'^', 'X', 'A', '^', 'F', 'O', '2', '0', ',', '2', '0', '^', 'F', 'D', 'W', 'A', 'R', 'I', 'N', 'C', '^', 'F', 'S', '^', 'X', 'Z'};

	uint8_t buf[RCVSIZE];
	uint32_t nbread = 0;
	char helloworld[] = "Hello World!\r\n";

	usb.Task();



	if (usb.getUsbTaskState() == USB_STATE_RUNNING)
	{
	/*	if (millis() >= next_time)
		{
*/
		Serial2.print("usb.getUsbTaskState  - ");
		Serial2.println(usb.getUsbTaskState());

	/*		usb.ForEachUsbDevice(&PrintAllDescriptors);
			usb.ForEachUsbDevice(&PrintAllAddresses);*/

		//	while (1);                           //stop
		//}
	}

	if (adk.isReady()) 
	{
		if (!connected) 
		{
			connected = true;
			Serial2.print(F("\r\nConnected to accessory"));
		}

		uint8_t msg[1];
		uint8_t len = sizeof(msg);
		uint8_t rcode = adk.RcvData(&len, msg);
		if (rcode && rcode != 4) {
			Serial2.print(F("\r\nData rcv: "));
			Serial2.print(rcode, HEX);
		}
		else if (len > 0) {
			Serial2.print(F("\r\nData Packet: "));
			Serial2.print(msg[0]);
			digitalWrite(LED, msg[0] ? HIGH : LOW);
		}

		if ((int32_t)((uint32_t)millis() - timer) >= 1000) { // Send data every 1s
			timer = (uint32_t)millis();
			rcode = adk.SndData(sizeof(timer), (uint8_t*)&timer);
			if (rcode && rcode != 4) {
				Serial2.print(F("\r\nData send: "));
				Serial2.print(rcode, HEX);
			}
			else if (rcode != 4) {
				Serial2.print(F("\r\nTimer: "));
				Serial2.print(timer);
			}
		}
	}
	else 
	{
		if (connected) 
		{
			connected = false;
			Serial2.print(F("\r\nDisconnected from accessory"));
			digitalWrite(LED, LOW);
		}
		else
		{

			Serial2.println(F("\r\nDisconnected from accessory"));
			while (1); // halt
		}
	}









	//if (adk.isReady() == false) {
	//	return;
	//}
	///* Write hello string to ADK */
	//adk.SndData(strlen(helloworld), (uint8_t *)helloworld);

	//delay(1000);

	///* Read data from ADK and print to UART */
	//adk.RcvData((uint8_t *)&nbread, buf);
	//if (nbread > 0)
	//{
	//	Serial2.print("RCV: ");
	//	for (uint32_t i = 0; i < nbread; ++i)
	//	{
	//		Serial2.print((char)buf[i]);
	//	}
	//	Serial2.print("\r\n");
	//}

}
