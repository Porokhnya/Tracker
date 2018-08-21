#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "ScreenHAL.h"
#include "Settings.h"
#include "FileUtils.h"
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

    bool lastLogActiveFlag;
    void drawLogState(HalDC* hal);
    void drawLogDuration(HalDC* hal);

   bool lastDoorOpen;
   bool doorStateVisible;
   uint32_t doorStateBlinkTimer;
   bool blinkDoorState;
   void drawDoorState(HalDC* hal);
  
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

    void drawGUI(HalDC* hal);
    //LinkList<uint8_t> intervalsRingBuffer;
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  exportToSerial,
  exportToWiFi,
  exportToPrinter
  
} ExportMode;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SCREEN_FILES_COUNT 4 // кол-во файлов на одном экране
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ExportToSerialScreen : public AbstractHALScreen
{
public:

  static AbstractHALScreen* create()
  {
    return new ExportToSerialScreen();
  }

  virtual void onActivate();
  virtual void onDeactivate();

  void setFile(FileEntry* entry);


protected:

  virtual void doSetup(HalDC* hal);
  virtual void doUpdate(HalDC* hal);
  virtual void doDraw(HalDC* hal);
  virtual void onButtonPressed(HalDC* hal, int pressedButton);

private:
  ExportToSerialScreen();
  FileEntry* selectedFile;

  bool isExportDone;
  void startExport();
};  
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ExportLogsScreen : public AbstractHALScreen
{
public:

  static AbstractHALScreen* create()
  {
    return new ExportLogsScreen();
  }

  virtual void onActivate();
  virtual void onDeactivate();

  void setMode(ExportMode mode);
  void rescanFiles();


protected:

  virtual void doSetup(HalDC* hal);
  virtual void doUpdate(HalDC* hal);
  virtual void doDraw(HalDC* hal);
  virtual void onButtonPressed(HalDC* hal, int pressedButton);

private:
  ExportLogsScreen();

  ExportMode exportMode;

  bool hasSD;
  int totalFilesCount;
  FileEntry** files;
  void clearFiles();

  int selectedFileNumber;
  int currentPage;
  void selectFile(HalDC* hal, int8_t step);

  void drawGUI(HalDC* hal);
  void drawFiles(HalDC* hal);

  void exportSelectedFile(HalDC* hal);

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

  void drawGUI(HalDC* hal);
  void drawStartScreen(HalDC* hal);


};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class BordersScreen : public AbstractHALScreen
{
public:

	static AbstractHALScreen* create()
	{
		return new BordersScreen();
	}

	virtual void onActivate();
	virtual void onDeactivate();


protected:

	virtual void doSetup(HalDC* hal);
	virtual void doUpdate(HalDC* hal);
	virtual void doDraw(HalDC* hal);
	virtual void onButtonPressed(HalDC* hal, int pressedButton);

private:
	BordersScreen();

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  temperatureBorder,
  humidityBorder,
  adcBorder
  
} WhichBorder;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class EditBorderScreen : public AbstractHALScreen
{
public:

  static AbstractHALScreen* create(const char* screenName, WhichBorder b)
  {
    return new EditBorderScreen(screenName, b);
  }

  virtual void onActivate();
  virtual void onDeactivate();


protected:

  virtual void doSetup(HalDC* hal);
  virtual void doUpdate(HalDC* hal);
  virtual void doDraw(HalDC* hal);
  virtual void onButtonPressed(HalDC* hal, int pressedButton);

private:
  EditBorderScreen(const char* screenName, WhichBorder b);

  WhichBorder whichBorder;
  int16_t minVal, maxVal, currentMinVal, currentMaxVal;
  uint8_t changeStep;
  uint8_t editedVal;
  const char* caption;

  void saveValues();
  void reloadValues();

  void drawGUI(HalDC* hal);

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
