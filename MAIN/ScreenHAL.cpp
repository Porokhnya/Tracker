#include "CONFIG.h"
#include "ScreenHAL.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AbstractHALScreen::AbstractHALScreen(const char* name)
{
  screenName = name;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AbstractHALScreen::~AbstractHALScreen()
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AbstractHALScreen::setup(HalDC* hal)
{
  // тут общие для всех классов настройки
  doSetup(hal); 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AbstractHALScreen::update(HalDC* hal)
{
  if(isActive())
  {
  int pressedButton = -1;//screenButtons->checkButtons();
  
  if(pressedButton != -1)
  {
    hal->notifyAction(this);
    onButtonPressed(hal, pressedButton);
  }

    if(isActive())
      doUpdate(hal);
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AbstractHALScreen::draw(HalDC* hal)
{
  if(isActive())
  {
    doDraw(hal);
    
    if(isActive())
    {
     // screenButtons->drawButtons(); 
    }
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// HalDC
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
HalDC::HalDC()
{
  currentScreenIndex = -1;
  requestedToActiveScreen = NULL;
  requestedToActiveScreenIndex = -1;
  on_action = NULL;
  halDCDescriptor = NULL;
  cursorX = cursorY = 0;
  curFont = NULL;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::notifyAction(AbstractHALScreen* screen)
{
  if(on_action)
    on_action(screen);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::addScreen(AbstractHALScreen* screen)
{
  screen->setup(this);
  screens.push_back(screen);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::initHAL()
{
  //Тут инициализация/переинициализация дисплея
  //halDCDescriptor->InitLCD();
  //halDCDescriptor->clrScr();

  halDCDescriptor->begin();
  halDCDescriptor->cp437(true);
  halDCDescriptor->display();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::setup()
{
  //создание библиотеки для экрана
  //halDCDescriptor = new HalDCDescriptor(LCD_SCK_PIN, LCD_MOSI_PIN, LCD_DC_PIN, LCD_RST_PIN, LCD_CS_PIN);
   halDCDescriptor = new HalDCDescriptor(DC_PIN, CS_PIN, RST_PIN);

  // инициализируем дисплей
  initHAL();
  
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::update()
{

  if(requestedToActiveScreen != NULL)
  {
    // попросили сделать экран активным
    AbstractHALScreen* screen = requestedToActiveScreen;
    currentScreenIndex = requestedToActiveScreenIndex;
    
    requestedToActiveScreen = NULL;
    
    screen->setActive(true);
    screen->onActivate();

    //Тут очистка экрана
     clearScreen(BGCOLOR);

    screen->update(this);
    screen->draw(this);
    
    return;
    
  } // if(requestedToActiveScreen != NULL)

  if(currentScreenIndex == -1) // ни разу не рисовали ещё ничего, исправляемся
  {
    if(screens.size())
     switchToScreen((unsigned int)0); // переключаемся на первый экран, если ещё ни разу не показали ничего     
  }

  if(currentScreenIndex == -1)
    return;

  // обновляем текущий экран
  AbstractHALScreen* currentScreen = screens[currentScreenIndex];
  currentScreen->update(this);
  
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AbstractHALScreen* HalDC::getActiveScreen()
{
  if(currentScreenIndex < 0 || !screens.size())
    return NULL;

  return screens[currentScreenIndex];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::switchToScreen(AbstractHALScreen* screen)
{

  if(requestedToActiveScreen != NULL) // ждём переключения на новый экран
    return;
  
  if(currentScreenIndex > -1 && screens.size())
  {
     AbstractHALScreen* si = screens[currentScreenIndex];
     si->setActive(false);
     si->onDeactivate();
  }
  
  for(size_t i=0;i<screens.size();i++)
  {
    if(screens[i] == screen)
    {
      requestedToActiveScreen = screen;
      requestedToActiveScreenIndex = i;

      break;
    }
  }  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::switchToScreen(unsigned int screenIndex)
{
  if(screenIndex < screens.size())
      switchToScreen(screens[screenIndex]);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::switchToScreen(const char* screenName)
{
  
  // переключаемся на запрошенный экран
  for(size_t i=0;i<screens.size();i++)
  {
    AbstractHALScreen* si = screens[i];
    if(!strcmp(si->getName(),screenName))
    {
      switchToScreen(si);
      break;
    }
  }

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String HalDC::utf8rus(const char* source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = strlen(source); i = 0;

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
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::setCursor(uint16_t x, uint16_t y)
{
  cursorX = x;
  cursorY = y;
  halDCDescriptor->setCursor(x,y);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::println(const char* str)
{
  halDCDescriptor->println(utf8rus(str));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::print(const char* str)
{
  halDCDescriptor->print(utf8rus(str));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::display()
{
  halDCDescriptor->display();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::setFont(FONT_TYPE* font)
{
 curFont = font;
 halDCDescriptor->setFont(font); 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::setTextSize(uint8_t sz)
{
  halDCDescriptor->setTextSize(sz);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::clearScreen(COLORTYPE color)
{
  halDCDescriptor->clearDisplay();
  halDCDescriptor->display();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::setTextColor(COLORTYPE color, COLORTYPE bgColor)
{
 halDCDescriptor->setTextColor(color, bgColor);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t HalDC::getFontWidth(FONT_TYPE* font)
{
  /*
    if(!font)
      return 0;
    
    return READ_FONT_BYTE(0);
   */

  halDCDescriptor->setFont(font);

  int16_t  x1, y1;
  uint16_t w, h;
 
  halDCDescriptor->getTextBounds("W", 0, 0, &x1, &y1, &w, &h);
  
  halDCDescriptor->setFont(curFont);

  return w;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t HalDC::getFontHeight(FONT_TYPE* font)
{
  /*
    if(!font)
      return 0;
    
    return READ_FONT_BYTE(1); 
    */

  halDCDescriptor->setFont(font);

  int16_t  x1, y1;
  uint16_t w, h;
 
  halDCDescriptor->getTextBounds("W", 0, 0, &x1, &y1, &w, &h);
  
  halDCDescriptor->setFont(curFont);

  return h;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
HalDC Screen;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

