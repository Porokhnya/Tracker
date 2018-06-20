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
//#include <adk.h>
#include <MouseController.h>



Uart Serial2(&sercom2, 3, 4, SERCOM_RX_PAD_1, UART_TX_PAD_0);  // Подключить Serial2
void SERCOM2_Handler()                                         // Подключить Serial2 
{
	Serial2.IrqHandler();
}



// Initialize USB Controller
USBHost usb;

// Attach mouse controller to USB
MouseController mouse(usb);

// variables for mouse button states
boolean leftButton = false;
boolean middleButton = false;
boolean rightButton = false;

// This function intercepts mouse movements
void mouseMoved() {
	Serial2.print("Move: ");
	Serial2.print(mouse.getXChange());
	Serial2.print(", ");
	Serial2.println(mouse.getYChange());
}

// This function intercepts mouse movements while a button is pressed
void mouseDragged() {
	Serial2.print("DRAG: ");
	Serial2.print(mouse.getXChange());
	Serial2.print(", ");
	Serial2.println(mouse.getYChange());
}

// This function intercepts mouse button press
void mousePressed() {
	Serial2.print("Pressed: ");
	if (mouse.getButton(LEFT_BUTTON)) {
		Serial2.print("L");
		leftButton = true;
	}
	if (mouse.getButton(MIDDLE_BUTTON)) {
		Serial2.print("M");
		middleButton = true;
	}
	if (mouse.getButton(RIGHT_BUTTON)) {
		Serial2.print("R");
		rightButton = true;
	}
	Serial2.println();
}

// This function intercepts mouse button release
void mouseReleased() {
	Serial2.print("Released: ");
	if (!mouse.getButton(LEFT_BUTTON) && leftButton == true) {
		Serial2.print("L");
		leftButton = false;
	}
	if (!mouse.getButton(MIDDLE_BUTTON) && middleButton == true) {
		Serial2.print("M");
		middleButton = false;
	}
	if (!mouse.getButton(RIGHT_BUTTON) && rightButton == true) {
		Serial2.print("R");
		rightButton = false;
	}
	Serial2.println();
}


void setup(void)
{
	Serial2.begin(115200);              // Возможно принтер

	//Assign pins 3 & 4 SERCOM functionality
	pinPeripheral(3, PIO_SERCOM_ALT);    // Настройка Serial2
	pinPeripheral(4, PIO_SERCOM_ALT);    // Настройка Serial2


while (!Serial2); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
Serial2.println("Mouse Controller Program started");

if (usb.Init() == -1)
Serial2.println("OSC did not start.");

delay(20);
}

void loop()
{
	// Process USB tasks
	usb.Task();
}
