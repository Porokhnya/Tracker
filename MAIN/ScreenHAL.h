#ifndef _UTFTMENU_H
#define _UTFTMENU_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "CONFIG.h"
#include "TinyVector.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class HalDC;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if DISPLAY_USED == DISPLAY_ILI9341
#include <UTFT_ILI9341.h>
#include <URTouchCD_ILI9341.h>
#include <URTouch_ILI9341.h>
#include "UTFT_Buttons_Rus.h"
#else
  #error "Unsupported display!"
#endif  
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if DISPLAY_USED == DISPLAY_ILI9341

  #if defined (__arm__)
    #define READ_FONT_BYTE(x) font[x]  
  #elif defined(__AVR__)  
    #define READ_FONT_BYTE(x) pgm_read_byte(&(font[x]))  
  #endif

#else
  #error "Unsupported display!"
#endif  
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if DISPLAY_USED == DISPLAY_ILI9341
  typedef UTFT HalDCDescriptor;
  typedef uint16_t COLORTYPE;
  typedef uint8_t FONT_TYPE;
  extern FONT_TYPE BigRusFont[];               // какой шрифт используем
  extern FONT_TYPE SmallRusFont[];             // какой шрифт используем
  extern FONT_TYPE Inconsola[];    // какой шрифт используем
  extern FONT_TYPE Various_Symbols_16x32[];    // какой шрифт используем
  extern FONT_TYPE Various_Symbols_32x32[];    // какой шрифт используем
  
  #define SCREEN_BACK_COLOR VGA_BLACK          // цвет фона
  #define SCREEN_TEXT_COLOR VGA_WHITE          // цвет шрифта
  #define SCREEN_SMALL_FONT Inconsola       // маленький шрифт
  #define SCREEN_ORIENTATION  PORTRAIT         // ориентация экрана вертикальная
#define SCREEN_ORIENTATION  LANDSCAPE         // ориентация экрана вертикальная
  #define TOUCH_PRECISION PREC_HI
  #define SCREEN_BUTTON_COLORS VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_BLUE // цвета для кнопок
  #define SCREEN_BUTTON_COLORS2 VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, 0x4DC9 // цвета для кнопок
  
#else
  #error "Unsupported display!"
#endif  
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

    #if DISPLAY_USED == DISPLAY_ILI9341
      UTFT_Buttons_Rus* screenButtons; // кнопки на экране    
    #endif

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


  #if DISPLAY_USED == DISPLAY_ILI9341
    UTFT* getUTFT() {return halDCDescriptor; }
    URTouch* getTouch() {return halTouch; }
  #endif

  void addScreen(AbstractHALScreen* screen);

  AbstractHALScreen* getActiveScreen();
  void onAction(OnScreenAction handler) {on_action = handler;}
  void notifyAction(AbstractHALScreen* screen);

  void switchToScreen(AbstractHALScreen* screen);
  void switchToScreen(const char* screenName);
  void switchToScreen(unsigned int screenIndex);

  // HARDWARE HAL
  int print(const char* str,int x, int y, int deg=0, bool computeStringLengthOnly=false);

  void setFont(FONT_TYPE* font);
  FONT_TYPE* getFont();
  void setBackColor(COLORTYPE color);
  COLORTYPE  getBackColor();
  void setColor(COLORTYPE color);
  COLORTYPE  getColor();
  void fillScreen(COLORTYPE color);
  void  drawRect(int x1, int y1, int x2, int y2);
  void  drawRoundRect(int x1, int y1, int x2, int y2);
  void  fillRect(int x1, int y1, int x2, int y2);
  void  fillRoundRect(int x1, int y1, int x2, int y2);
  uint16_t getFontWidth(FONT_TYPE* font);
  uint16_t getFontHeight(FONT_TYPE* font);
  

private:

#if DISPLAY_USED == DISPLAY_ILI9341
  int printILI(const char* str,int x, int y, int deg=0, bool computeStringLengthOnly=false);
#endif

  //String utf8rus(const char* source);

  AbstractHALScreen* requestedToActiveScreen;
  int requestedToActiveScreenIndex;

  OnScreenAction on_action;
  
  HALScreensList screens;
  HalDCDescriptor* halDCDescriptor;
  int currentScreenIndex;

  #if DISPLAY_USED == DISPLAY_ILI9341
    URTouch* halTouch;
  #endif

  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern HalDC Screen;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
