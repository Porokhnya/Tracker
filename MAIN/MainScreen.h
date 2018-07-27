#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "ScreenHAL.h"
#include "Settings.h"
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

    Si7021Data lastSensorData;
    void drawTemperature(HalDC* hal);

    uint16_t adcValue;
    void drawADC(HalDC* hal);
	int oldsecond;
	void drawTime(HalDC* hal);
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern MainScreen* mainScreen;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MenuScreen1 : public AbstractHALScreen
{
  public:

  static AbstractHALScreen* create()
  {
    return new MenuScreen1();
  }
  
   virtual void onActivate();
   virtual void onDeactivate();


protected:
  
    virtual void doSetup(HalDC* hal);
    virtual void doUpdate(HalDC* hal);
    virtual void doDraw(HalDC* hal);
    virtual void onButtonPressed(HalDC* hal, int pressedButton);

private:
    MenuScreen1();
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class MenuScreen2 : public AbstractHALScreen
{
public:

	static AbstractHALScreen* create()
	{
		return new MenuScreen2();
	}

	virtual void onActivate();
	virtual void onDeactivate();


protected:

	virtual void doSetup(HalDC* hal);
	virtual void doUpdate(HalDC* hal);
	virtual void doDraw(HalDC* hal);
	virtual void onButtonPressed(HalDC* hal, int pressedButton);

private:
	MenuScreen2();

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class MenuScreen3 : public AbstractHALScreen
{
public:

	static AbstractHALScreen* create()
	{
		return new MenuScreen3();
	}

	virtual void onActivate();
	virtual void onDeactivate();


protected:

	virtual void doSetup(HalDC* hal);
	virtual void doUpdate(HalDC* hal);
	virtual void doDraw(HalDC* hal);
	virtual void onButtonPressed(HalDC* hal, int pressedButton);

private:
	MenuScreen3();

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


class MenuScreen4 : public AbstractHALScreen
{
public:

	static AbstractHALScreen* create()
	{
		return new MenuScreen4();
	}

	virtual void onActivate();
	virtual void onDeactivate();


protected:

	virtual void doSetup(HalDC* hal);
	virtual void doUpdate(HalDC* hal);
	virtual void doDraw(HalDC* hal);
	virtual void onButtonPressed(HalDC* hal, int pressedButton);

private:
	MenuScreen4();

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
