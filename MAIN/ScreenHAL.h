#ifndef _UTFTMENU_H
#define _UTFTMENU_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "TinyVector.h"
//#include <LCD5110_Graph.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class HalDC;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
#if defined (__arm__)
  #define READ_FONT_BYTE(x) font[x]  
#elif defined(__AVR__)  
  #define READ_FONT_BYTE(x) pgm_read_byte(&(font[x]))  
#endif
 */
//typedef LCD5110 HalDCDescriptor;
//extern FONTTYPE SmallFont[]; // малый шрифт (из библиотеки)
//extern FONTTYPE MediumNumbers[]; // средний шрифт для цифр (из библиотеки)

typedef Adafruit_PCD8544 HalDCDescriptor;
typedef uint16_t COLORTYPE;
typedef GFXfont FONT_TYPE;


#define BGCOLOR BLACK
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// абстрактный класс экрана
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class AbstractHALScreen
{
  public:

    void setup(HalDC* hal);
    void update(HalDC* hal);
    void draw(HalDC* hal);

    const char* getName() {return screenName;}
    bool isActive() {return isActiveScreen; }
    void setActive(bool val){ isActiveScreen = val; }
  
    AbstractHALScreen(const char* name);
    virtual ~AbstractHALScreen();

    // вызывается, когда переключаются на экран
    virtual void onActivate(){}

    // вызывается, когда экран становится неактивным
    virtual void onDeactivate() {}

  protected:

    
    virtual void doSetup(HalDC* hal) = 0;
    virtual void doUpdate(HalDC* hal) = 0;
    virtual void doDraw(HalDC* hal) = 0;
    virtual void onButtonPressed(HalDC* hal,int pressedButton) = 0;

    private:
      const char* screenName;
      bool isActiveScreen;
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef Vector<AbstractHALScreen*> HALScreensList; // список экранов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// класс-менеджер работы с экраном
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef void (*OnScreenAction)(AbstractHALScreen* screen);
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class HalDC
{

public:
  HalDC();

  void setup();
  void update();
  void initHAL();

  void addScreen(AbstractHALScreen* screen);

  AbstractHALScreen* getActiveScreen();
  void onAction(OnScreenAction handler) {on_action = handler;}
  void notifyAction(AbstractHALScreen* screen);

  void switchToScreen(AbstractHALScreen* screen);
  void switchToScreen(const char* screenName);
  void switchToScreen(unsigned int screenIndex);

  void setCursor(uint16_t x, uint16_t y);
  void print(const char* str);
  void print(const String& str) { print(str.c_str()); }
  void println(const char* str);
  void println(const String& str) {println(str.c_str()); }
  void display();
  void setFont(FONT_TYPE* font);
  void clearScreen(COLORTYPE color = 0);
  void setTextColor(COLORTYPE color, COLORTYPE bgColor);
  void setTextSize(uint8_t sz);

  uint16_t getFontWidth(FONT_TYPE* font);
  uint16_t getFontHeight(FONT_TYPE* font);
  

private:


  String utf8rus(const char* source);

  AbstractHALScreen* requestedToActiveScreen;
  int requestedToActiveScreenIndex;

  OnScreenAction on_action;
  
  HALScreensList screens;
  HalDCDescriptor* halDCDescriptor;
  uint16_t cursorX, cursorY;

  int currentScreenIndex;
  FONT_TYPE* curFont;
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern HalDC Screen;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
