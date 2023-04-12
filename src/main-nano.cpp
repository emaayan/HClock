#include <Arduino.h>
#include <Wire.h>
#include <OneButton.h>

#include <IRTCLibWrapper.h>
#include <RTCLibWrapper.h>
#include <LCDDisplayWrapper.h>
extern "C"
{
#include <HebDateDisplay.h>
}

#include <ClockController.h>
#include <DebugUtils.h>
#include <Settings.h>

#ifdef DEBUG_AVR
#include <avr8-stub.h>
#endif
Settings *_confSettings = new Settings();
IDisplayWrapper *_disp = new LCDDisplayWrapper();
IRTCLibWrapper *_rtc = new RTCLibWrapper();

class ClockControllerLCD : public ClockController
{

public:
  ClockControllerLCD(IDisplayWrapper *disp, IRTCLibWrapper *rtc) : ClockController(disp, rtc)
  {
  }
  char getConfigMode(const ConfigMode dp)
  {
    switch (dp)
    {
    case NONE:
      return ' ';
    case YEAR:
      return 'Y';
    case MONTH:
      return 'M';
    case DAY:
      return 'D';
    case HOUR:
      return 'h';
    case MINUTE:
      return 'm';
    case SECOND:
      return 's';
    case BRIGHTNESS:
      return 'B';
    default:
      return ' ';
    }
  }
  
  void displayMode(const ConfigMode mode)
  {
    const char confMode = getConfigMode(mode);
    const char c[2] = {confMode, '\0'};
    _disp->display(c, 19, 0, false);
  }

  void displayTime(const TMWrapper &dtv)
  {
    char buffer[18] = "";
    dtv.toDateTimeString(buffer, size_t(buffer));
    _disp->println(0, false, "%-19s", buffer);
  }

  void displayHebDate(const HebDates &hr)
  {
    
    _disp->println(1, true, "%s, %s %s %s", hr.day_name, hr.dayInMonth, hr.monthName, hr.isNewMonthIndicator);
    _disp->println(2, true, "%s", hr.festivalName);
    _disp->println(3, true, "%s", hr.omer_count_name);
    // if (_settings.calcTimes)
    // {
    //     HebTimes ht = {"", "", "", "", "", "", ""};
    //     location loc;
    //     loc.longitude = _settings.longitude;
    //     loc.latitude = _settings.latitude;
    //     loc.elevation = _settings.elevation;

    //     displayTimes(hebrewDate, loc,&ht);
    // }
  }
};
ClockController *clockController = new ClockControllerLCD(_disp, _rtc);

/* #region  Buttons */
#ifndef RED_BUTTON
#define RED_BUTTON 4
#endif
#ifndef GREEN_BUTTON
#define GREEN_BUTTON 7
#endif

#ifndef BLUE_BUTTON
#define BLUE_BUTTON 8
#endif

#ifndef PULL_UP
#define PULL_UP false
#endif

#ifndef BRIGHTNESS_PIN
#define BRIGHTNESS_PIN 6
#endif

OneButton set_state_button(RED_BUTTON, PULL_UP);
OneButton increase_button(GREEN_BUTTON, PULL_UP);
OneButton decrease_button(BLUE_BUTTON, PULL_UP);

void onExitConfigEnter()
{
  clockController->changeMode(NONE);
}
void onInitConfigEnter()
{
  ConfigMode dp = clockController->getMode();
  if (dp != NONE)
  {
    onExitConfigEnter();
  }
  else
  {
    clockController->changeMode(YEAR);
  }
}

void onSwitchConfigMode()
{

  ConfigMode dp = clockController->getMode();

  switch (clockController->getMode())
  {
  case YEAR:
    dp = MONTH;
    break;
  case MONTH:
    dp = DAY;
    break;
  case DAY:
    dp = HOUR;
    break;
  case HOUR:
    dp = MINUTE;
    break;
  case MINUTE:
    dp = SECOND;
    break;
  case SECOND:
    dp = BRIGHTNESS;
    break;
  case BRIGHTNESS:
    dp = NONE;
    break;
  default:
    break;
  }
  clockController->changeMode(dp);
}

uint8_t brightness;
const uint8_t brightAdjResolution = 10;
const int BRIGHT_ADDR = 1;

void sendBrightness(uint8_t brightness)
{
  analogWrite(BRIGHTNESS_PIN, brightness);
}

void initBrightness()
{
  pinMode(BRIGHTNESS_PIN, OUTPUT);
  brightness = _confSettings->initSetting(BRIGHT_ADDR, 100);
  sendBrightness(brightness);
}

/*
 *true increase, false decrease, every change write to EEPROM
 */
uint8_t setBrightness(bool dir)
{
  int8_t i = dir ? -1 : 1; // this is because PNP transistor
  int8_t setting = i * brightAdjResolution;
  uint8_t curSetting = brightness;
  uint8_t settingToBe = curSetting + setting;
  if (settingToBe > 0 && settingToBe < 255)
  {
    brightness = settingToBe;
    _confSettings->writeSetting(BRIGHT_ADDR, brightness);
    sendBrightness(brightness);
  }
  return brightness;
}

void incBrightness()
{
  setBrightness(true);
}

void decBrightness()
{
  setBrightness(false);
}

void onIncrement()
{
  ConfigMode dp = clockController->getMode();
  if (dp != NONE && dp != BRIGHTNESS)
  {
    clockController->increment();
  }
  else
  {
    if (dp == BRIGHTNESS)
    {
      incBrightness();
    }
  }
}

void onDecrement()
{
  ConfigMode dp = clockController->getMode();
  if (dp != NONE && dp != BRIGHTNESS)
  {
    clockController->decrement();
  }
  else
  {
    if (dp == BRIGHTNESS)
    {
      decBrightness();
    }
  }
}

void setupButtons()
{
  set_state_button.attachLongPressStart(onInitConfigEnter);
  set_state_button.attachClick(onSwitchConfigMode);

  increase_button.attachClick(onIncrement);
  increase_button.attachDuringLongPress(onIncrement);
  decrease_button.attachClick(onDecrement);
  decrease_button.attachDuringLongPress(onDecrement);
}

void onTickButtons()
{
  set_state_button.tick();
  increase_button.tick();
  decrease_button.tick();
}
/* #endregion */

void setup()
{

#ifdef DEBUG_CON
  Serial.begin(115200);
#endif

  Wire.begin();
  _confSettings->init(2);

  ClockSettings settings;

  settings.timezone = 2.0;
  settings.isIsrael = 1;
  settings.latitude = 32.109333;
  settings.longitude = 34.855499;
  settings.elevation = 34;
  settings.calcTimes = false;
  clockController->init(settings);

#ifdef BRIGHTNESS_PIN
  initBrightness();
#endif
  setupButtons();
}

void onTick()
{
  onTickButtons();

  clockController->onTick(millis());
}

void loop()
{
 
  onTick();
}
