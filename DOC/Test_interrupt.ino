




const int LAST_PIN = 13;

volatile int tick;

void ding() {
  tick++;
}

void setup() {
  int i;
  for (i = 0; i <= LAST_PIN; i++) {
    pinMode(i, INPUT_PULLUP);
    attachInterrupt(i, ding, FALLING);
  }
  Serial.begin(9600);
}

void loop() {
  while (!tick);
  for (int i = 0; i <= LAST_PIN; i++)
    if (!digitalRead(i))
      Serial.print(i);
  Serial.print("  ");
  Serial.println(tick);
  tick = 0;
}



/*

         EIC  ZERO pins
           0   11
           1   13
           2   10 a0 a5
           3   12
           4   6 a3
           5   7 a4
           6   8  sda(20)
           7   9  scl(21)
           8   a1
           9   3 a2
           10  1 MOSI(23)
           11  0  SCK(24)
           12  MISO(22)
           13
           14  2
           15  5

*/

Отключение светодиодов

В предыдущих версиях эти TX и RX светодиоды, остались неиспользованными в коде ядра Ардуино,
 но может быть активирована в вашем эскизе.
 Если, как я, Вы решили воспользоваться этими бывшими неиспользованными светодиодами Tx и Rx для ваших собственных целей, 
 то необходимо деактивировать контакты Tx и Rx на родном USB-порту.

К счастью, Arduino завернул код Tx и RX LED в # ifdef директивы в родном файле USB.
 Это означает, что если вы закомментируете определения PIN_LED_TXL и PIN_LED_RXL в "варианте нуля".h " файл,
 он будет деактивировать Tx и Rx светодиоды на родном USB-порту и ваш свободный, чтобы использовать их в эскизе oncemore.
 
 // LEDs
#define PIN_LED_13           (13u)
//#define PIN_LED_RXL          (25u)
//#define PIN_LED_TXL          (26u)
#define PIN_LED              PIN_LED_13
#define PIN_LED2             PIN_LED_RXL
#define PIN_LED3             PIN_LED_TXL
#define LED_BUILTIN          PIN_LED_13
 
 