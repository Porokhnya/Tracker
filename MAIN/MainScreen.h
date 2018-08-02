#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "ScreenHAL.h"
#include "Settings.h"
#include "FileUtils.h"

#define Serial SerialUSB     // Подключить USB порт
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
    LinkList<uint8_t> intervalsRingBuffer;
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
typedef enum
{
  dmStartScreen,
  dmExportToSerial,
  dmExportToWiFi,
  dmExportToPrinter,
  dmExportDone
  
} SettingScreenDrawMode;
*/
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

  /*

  bool ignoreKeys, exportActive;
  SettingScreenDrawMode drawMode;
  uint32_t dummyTimerNeedToRemoveLater;

  
  void drawExportToSerial(HalDC* hal);
  void drawExportToWiFi(HalDC* hal);
  void drawExportToPrinter(HalDC* hal);
  void drawExportDone(HalDC* hal);

  bool isExportDone();
  */

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
