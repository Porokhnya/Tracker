#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "ScreenHAL.h"
#include "DS18B20.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// главный экран
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MainScreen : public AbstractHALScreen
{
  public:

  static AbstractHALScreen* create()
  {
    return new MainScreen();
  }
  
   virtual void onActivate();
   virtual void onDeactivate();


protected:
  
    virtual void doSetup(HalDC* hal);
    virtual void doUpdate(HalDC* hal);
    virtual void doDraw(HalDC* hal);
    virtual void onButtonPressed(HalDC* hal, int pressedButton);

private:
    MainScreen();

    DS18B20Temperature temp;
    void drawTemperature(HalDC* hal);

    uint16_t adcValue;
    void drawADC(HalDC* hal);
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern MainScreen* mainScreen;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

