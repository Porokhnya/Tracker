
#include <Arduino.h>   // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function

#define Serial SERIAL_PORT_USBVIRTUAL
Uart Serial2 (&sercom2, 3, 4, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM2_Handler()
{
  Serial2.IrqHandler();
}
 
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);

  delay(1000);

  // Assign pins 3 & 4 SERCOM functionality
  pinPeripheral(3, PIO_SERCOM_ALT);
  pinPeripheral(4, PIO_SERCOM_ALT);

   Serial2.println("Test2");
   Serial1.println("Test1");
     Serial.println("TestUSB");
}
 
uint8_t i=0;
void loop() {
  Serial.print(i);
  Serial2.write(i++);
  if (Serial2.available()) {
    Serial.print(" -> 0x"); Serial.print(Serial2.read(), HEX);
  }
  Serial.println();
  
  delay(10);
 
}


