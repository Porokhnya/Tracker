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


USBHost usb;
ADK adk(&usb, "Arduino SA",
	"Arduino_Terminal",
	"Arduino Terminal for Android",
	"1.0",
	"http://labs.arduino.cc/uploads/ADK/ArduinoTerminal/ThibaultTerminal_ICS_0001.apk",
	"1");






#define RCVSIZE 128




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

	if (usb.Init() == -1)
		Serial2.println("OSC did not start.");
	 
	delay(20);

}

void loop()
{

	//char helloworld[] = {'^', 'X', 'A', '^', 'F', 'O', '2', '0', ',', '2', '0', '^', 'F', 'D', 'W', 'A', 'R', 'I', 'N', 'C', '^', 'F', 'S', '^', 'X', 'Z'};

	uint8_t buf[RCVSIZE];
	uint32_t nbread = 0;
	char helloworld[] = "Hello World!\r\n";

	usb.Task();

	if (adk.isReady() == false) {
		return;
	}
	/* Write hello string to ADK */
	adk.SndData(strlen(helloworld), (uint8_t *)helloworld);

	delay(1000);

	/* Read data from ADK and print to UART */
	adk.RcvData((uint8_t *)&nbread, buf);
	if (nbread > 0)
	{
		Serial2.print("RCV: ");
		for (uint32_t i = 0; i < nbread; ++i)
		{
			Serial2.print((char)buf[i]);
		}
		Serial2.print("\r\n");
	}

}
