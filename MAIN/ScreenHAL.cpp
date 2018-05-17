#include "CONFIG.h"
#include "ScreenHAL.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if DISPLAY_USED == DISPLAY_ILI9341
  const uint8_t utf8_rus_charmap[] PROGMEM = {'A',128,'B',129,130,'E',131,132,133,134,135,'K',136,'M','H','O',137,'P','C','T',138,139,'X',140,141,
  142,143,144,145,146,147,148,149,'a',150,151,152,153,'e',154,155,156,157,158,159,160,161,162,'o',163,'p','c',164,'y',165,'x',166,167,168,169,170,
  171,172,173,174,175};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void buttonPressed(int btn) // вызывается по нажатию на кнопку - тут можно пищать баззером, например)
{
    if(btn != -1)
    {
      // BUZZ!!!!
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void buttonDrawed() // вызывается после отрисовки каждой кнопки
{
  yield();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------  
#endif // DISPLAY_USED == DISPLAY_ILI9341
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int rusPrintFunc(const char* st,int x, int y, int deg, bool computeStringLengthOnly)
{
  return Screen.print(st,x,y,deg,computeStringLengthOnly);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AbstractHALScreen::AbstractHALScreen(const char* name)
{
  screenName = name;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AbstractHALScreen::~AbstractHALScreen()
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    delete screenButtons;
  #endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AbstractHALScreen::setup(HalDC* hal)
{
#if DISPLAY_USED == DISPLAY_ILI9341  
  screenButtons = new UTFT_Buttons_Rus(hal->getUTFT(), hal->getTouch(), rusPrintFunc);
  screenButtons->setTextFont(SCREEN_SMALL_FONT);
  screenButtons->setButtonColors(SCREEN_BUTTON_COLORS);
#endif  

  // тут общие для всех классов настройки
  doSetup(hal); 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AbstractHALScreen::update(HalDC* hal)
{
  if(isActive())
  {
    #if DISPLAY_USED == DISPLAY_ILI9341 
      int pressedButton = screenButtons->checkButtons(buttonPressed);
      
      if(pressedButton != -1)
      {
        hal->notifyAction(this);
        onButtonPressed(hal, pressedButton);
      }
    #endif

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
      #if DISPLAY_USED == DISPLAY_ILI9341 
        screenButtons->drawButtons(buttonDrawed); 
      #endif
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

  #if DISPLAY_USED == DISPLAY_ILI9341
    halTouch = NULL;
  #endif
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
  #if DISPLAY_USED == DISPLAY_ILI9341

  halDCDescriptor->InitLCD(SCREEN_ORIENTATION);
  setBackColor(SCREEN_BACK_COLOR);
  fillScreen(SCREEN_BACK_COLOR);
  setFont(SCREEN_SMALL_FONT);

  halTouch->InitTouch(SCREEN_ORIENTATION);
  halTouch->setPrecision(TOUCH_PRECISION);
  
  #else
    #error "Unsupported display!"
  #endif  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::setup()
{
  //создание библиотеки для экрана

  #if DISPLAY_USED == DISPLAY_ILI9341
  
    halDCDescriptor = new UTFT();
    halTouch = new URTouch(TFT_TOUCH_CLK_PIN,TFT_TOUCH_CS_PIN,TFT_TOUCH_DIN_PIN,TFT_TOUCH_DOUT_PIN,TFT_TOUCH_IRQ_PIN);

  
    #if DISPLAY_INIT_DELAY > 0
    delay(DISPLAY_INIT_DELAY);
    #endif  
  
  #else
    #error "Unsupported display!"
  #endif
  
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
   fillScreen(SCREEN_BACK_COLOR);

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
/*
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
*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::setFont(FONT_TYPE* font)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
   halDCDescriptor->setFont(font); 
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
FONT_TYPE* HalDC::getFont()
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    return halDCDescriptor->getFont();
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::fillScreen(COLORTYPE color)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
	//halDCDescriptor->setColor(color);
	//halDCDescriptor->fillRect(0, 0, 240, 320, color);
    halDCDescriptor->fillScr(0,0,0);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::setBackColor(COLORTYPE color)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    halDCDescriptor->setBackColor(color);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
COLORTYPE  HalDC::getBackColor()
{
  return halDCDescriptor->getBackColor(); 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HalDC::setColor(COLORTYPE color)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    halDCDescriptor->setColor(color);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
COLORTYPE  HalDC::getColor()
{
  return halDCDescriptor->getColor();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  HalDC::drawRect(int x1, int y1, int x2, int y2)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    halDCDescriptor->drawRect(x1,y1,x2,y2);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  HalDC::drawRoundRect(int x1, int y1, int x2, int y2)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    halDCDescriptor->drawRoundRect(x1,y1,x2,y2);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  HalDC::fillRect(int x1, int y1, int x2, int y2)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    halDCDescriptor->fillRect(x1,y1,x2,y2);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  HalDC::fillRoundRect(int x1, int y1, int x2, int y2)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    halDCDescriptor->fillRoundRect(x1,y1,x2,y2);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t HalDC::getFontWidth(FONT_TYPE* font)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    if(!font)
      return 0;
    
    return READ_FONT_BYTE(0);
    
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t HalDC::getFontHeight(FONT_TYPE* font)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    if(!font)
      return 0;
    
    return READ_FONT_BYTE(1); 
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int HalDC::print(const char* st,int x, int y, int deg, bool computeStringLengthOnly)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    return printILI(st,x,y,deg,computeStringLengthOnly);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if DISPLAY_USED == DISPLAY_ILI9341
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int HalDC::printILI(const char* st,int x, int y, int deg, bool computeStringLengthOnly)
{
    
 int stl, i;
  stl = strlen(st);

  if (halDCDescriptor->orient==PORTRAIT)
  {
    if (x==RIGHT) 
      x=(halDCDescriptor->disp_x_size+1)-(stl*halDCDescriptor->cfont.x_size);
  
    if (x==CENTER) 
      x=((halDCDescriptor->disp_x_size+1)-(stl*halDCDescriptor->cfont.x_size))/2;
  } 
  else 
  {
    if (x==RIGHT) 
      x=(halDCDescriptor->disp_y_size+1)-(stl*halDCDescriptor->cfont.x_size);
    
    if (x==CENTER) 
      x=((halDCDescriptor->disp_y_size+1)-(stl*halDCDescriptor->cfont.x_size))/2;
  }
  
  uint8_t utf_high_byte = 0;
  uint8_t ch, ch_pos = 0;
  
  for (i = 0; i < stl; i++) 
  {
    ch = st[i];
    
    if ( ch >= 128) 
    {
      if ( utf_high_byte == 0 && (ch ==0xD0 || ch == 0xD1)) 
      {
        utf_high_byte = ch;
        continue;
      } 
      else 
      {
        if ( utf_high_byte == 0xD0) 
        {
          if (ch == 0x81) 
          { //Ё
            ch = 6;
          } 
          else 
          {
            if(ch <= 0x95) 
            {
              ch -= 0x90;
            } 
            else if( ch < 0xB6)
            {
              ch -= (0x90 - 1);
            } 
            else 
            {
              ch -= (0x90 - 2);
            }
          }
          
          ch = pgm_read_byte((utf8_rus_charmap + ch));
        
        } 
        else if (utf_high_byte == 0xD1) 
        {
          if (ch == 0x91) 
          {//ё
            ch = 39;
          } 
          else 
          {
            ch -= 0x80;
            ch += 50;
          }
          
          ch = pgm_read_byte((utf8_rus_charmap + ch));
        }
        
        utf_high_byte = 0;
      }
    } 
    else 
    {
      utf_high_byte = 0;
    }
    

    if (deg==0) 
    {
      if(!computeStringLengthOnly)
        halDCDescriptor->printChar(ch, x + (ch_pos * (halDCDescriptor->cfont.x_size)), y);
    } 
    else 
    {
      if(!computeStringLengthOnly)
        halDCDescriptor->rotateChar(ch, x, y, ch_pos, deg);
    }
    ++ch_pos;
  } // for  

  return ch_pos;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif // DISPLAY_USED == DISPLAY_ILI9341
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
HalDC Screen;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

