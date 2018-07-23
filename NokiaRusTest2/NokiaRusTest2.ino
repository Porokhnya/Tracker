
#include <Arduino.h>                                            // required before wiring_private.h
#include "wiring_private.h"                                     // pinPeripheral() function
#include "Wire.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "Adafruit_MCP23017.h"
#include "ds3231.h"
#include "SdFat.h"
//#include "Adafruit_Si7021.h"

// Software SPI (slower updates, more flexible pin options):

#define  SCLK  7  // Serial clock out (SCLK)
#define  DIN  A4  // Serial data out (DIN)
#define  DC  5    // Data/Command select (D/C)
#define  CS  31   // LCD chip select (CS)
#define  RST  30  // LCD reset (RST) 

Adafruit_PCD8544 display = Adafruit_PCD8544(SCLK, DIN, DC, CS, RST);

Adafruit_MCP23017 mcp;


#define Serial SERIAL_PORT_USBVIRTUAL                          // USB SAMD21G18A

Uart Serial2(&sercom2, 3, 4, SERCOM_RX_PAD_1, UART_TX_PAD_0);  // Подключить Serial2
void SERCOM2_Handler()                                         // Подключить Serial2 
{
	Serial2.IrqHandler();
}

//Adafruit_Si7021 sensor = Adafruit_Si7021();
#define  LCD_led 4                                             // МСР 4 Вывод подсветки питания дисплея

void setup()   {
  Serial.begin(9600);
  //Assign pins 3 & 4 SERCOM functionality
  pinPeripheral(3, PIO_SERCOM_ALT);    // Настройка Serial2
  pinPeripheral(4, PIO_SERCOM_ALT);    // Настройка Serial2

  delay(1000);

  //while (!Serial) {};

  Wire.begin();
  mcp.begin(1);      // use default address 0

  mcp.pinMode(LCD_led, OUTPUT);             // Настроить подсветку дисплея
  mcp.digitalWrite(LCD_led, LOW);           // Включить подсветку дисплея


  char DiodRight[] = "-\x91-";
  char DiodLeft[] = "-\x92-";
  char Capacitor[] = "-\x93-";
  char Resistor[] = "-\x94\x95-";
  char Omega[] = "\x90";
  char Micro[] = "\xB5";

  display.begin();
  display.cp437(true);
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(40);

  display.clearDisplay();   // clears the screen and buffer

  // russian font
  display.clearDisplay();
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println(utf8rus("Шрифт 1"));
  display.setTextSize(2);
  display.println(utf8rus("Шрифт2"));
  display.setTextSize(3);
  display.println(utf8rus("Шр3"));
  display.display();
  delay(5000);

  display.clearDisplay();
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print(DiodLeft); display.print(utf8rus("Диод влево"));
  display.setCursor(0,8);
  display.print(DiodRight); display.print(utf8rus("Диод вправо"));
  display.setCursor(0,16);
  display.print(Capacitor); display.print(utf8rus("Конденсатор"));
  display.setCursor(0,24);
  display.print(Resistor); display.print(utf8rus("Резистор"));
  display.setCursor(0,32);
  display.print(Omega); display.print(utf8rus(" символ Omega"));
  display.setCursor(0,40);
  display.print(Micro); display.print(utf8rus(" символ Micro"));
  display.display();
 
}

void loop() {
}


/* Recode russian fonts from UTF-8 to Windows-1251 */
String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB8; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
return target;
}

