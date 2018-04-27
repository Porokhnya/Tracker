#ifndef _UTFTMENU_H
#define _UTFTMENU_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "TinyVector.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class HalDC;
typedef void HalDCDescriptor;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// абстрактный класс экрана
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class AbstractHALScreen
{
  public:

    void setup(HalDC* dc);
    void update(HalDC* dc);
    void draw(HalDC* dc);

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

    
    virtual void doSetup(HalDC* dc) = 0;
    virtual void doUpdate(HalDC* dc) = 0;
    virtual void doDraw(HalDC* dc) = 0;
    virtual void onButtonPressed(HalDC* dc,int pressedButton) = 0;

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
  
  HalDCDescriptor* getDC() { return halDCDescriptor; };

private:

  AbstractHALScreen* requestedToActiveScreen;
  int requestedToActiveScreenIndex;

  OnScreenAction on_action;
  
  HALScreensList screens;
  HalDCDescriptor* halDCDescriptor;

  int currentScreenIndex;
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern HalDC Screen;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
