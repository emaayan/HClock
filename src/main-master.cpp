
#include <Arduino.h>
#include <OneButton.h>
#include <IRTCLibWrapper.h>
#include <RTCLibWrapper.h>
#include <DisplayWrapper.h>
#include <ClockController.h>
#include <DebugUtils.h>
#include <EEPROM.h>

#ifdef DEBUG_AVR
#include <avr8-stub.h>
#endif

IDisplayWrapper *_disp = new DisplayWrapper();
IRTCLibWrapper *_rtc = new RTCLibWrapper();
ClockControler clockControler(_disp, _rtc);

const int UNREADVALUE = 255;
uint8_t readSetting(int addr)
{
  return EEPROM.read(addr);
}

void writeSetting(int addr, uint8_t value)
{
  if (value < UNREADVALUE)
  {
    EEPROM.write(addr, value);
  }
}


uint8_t initSetting(int addr, int def)
{
  uint8_t setting = readSetting(addr);
  if (setting == UNREADVALUE)
  {
    setting = def;    
    writeSetting(addr,setting);    
  }
  return setting;
}


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

OneButton set_state_button(RED_BUTTON, PULL_UP);
OneButton increase_button(GREEN_BUTTON, PULL_UP);
OneButton decrease_button(BLUE_BUTTON, PULL_UP);

void onExitConfigEnter()
{
  clockControler.changeMode(RTCLibWrapper::NONE);
}
void onInitConfigEnter()
{
  RTCLibWrapper::DatePart dp = clockControler.getMode();
  if (dp != RTCLibWrapper::NONE)
  {
    onExitConfigEnter();
  }
  else
  {
    clockControler.changeMode(RTCLibWrapper::YEAR);
  }
}

void onSwitchConfigMode()
{

  RTCLibWrapper::DatePart dp = clockControler.getMode();

  switch (clockControler.getMode())
  {
  case RTCLibWrapper::YEAR:
    dp = RTCLibWrapper::MONTH;
    break;
  case RTCLibWrapper::MONTH:
    dp = RTCLibWrapper::DAY;
    break;
  case RTCLibWrapper::DAY:
    dp = RTCLibWrapper::HOUR;
    break;
  case RTCLibWrapper::HOUR:
    dp = RTCLibWrapper::MINUTE;
    break;
  case RTCLibWrapper::MINUTE:
    dp = RTCLibWrapper::SECOND;
    break;
  case RTCLibWrapper::SECOND:
    dp = RTCLibWrapper::NONE;
    break;
  default:
    break;
  }
  clockControler.changeMode(dp);
}

#ifndef BRIGHTNESS_PIN
  #define BRIGHTNESS_PIN 6
#endif

uint8_t brightness;
const uint8_t brightAdjResolution = 10;
const int BRIGHT_ADDR = 0;

void sendBrightness(uint8_t brightness){
  analogWrite(BRIGHTNESS_PIN, brightness);   
}

void initBrightness()
{
  pinMode(BRIGHTNESS_PIN, OUTPUT);
  brightness = initSetting(BRIGHT_ADDR, 100);  
  sendBrightness(brightness);   
}

/*
  *true increase, false decrease, every change write to EEPROM
*/
uint8_t setBrightness(bool dir)
{
  int8_t i = dir ? -1 : 1;//this is because PNP transistor
  int8_t setting = i * brightAdjResolution;
  uint8_t curSetting=brightness;
  uint8_t settingToBe=curSetting + setting;
  if (settingToBe > 0 && settingToBe < 255)
  {
    brightness = settingToBe;
    writeSetting(BRIGHT_ADDR, brightness);    
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
  RTCLibWrapper::DatePart dp = clockControler.getMode();
  if (dp != RTCLibWrapper::NONE)
  {
    clockControler.increment();
  }
  else
  {    
    incBrightness();
  }
 
}

void onDecrement()
{
  RTCLibWrapper::DatePart dp = clockControler.getMode();
  if (dp != RTCLibWrapper::NONE)
  {
    clockControler.decrement();
  }
  else
  {
    decBrightness();    
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
#ifdef DEBUG_AVR
  debug_init();
#endif
#ifdef DEBUG_CON
  Serial.begin(115200);
#endif

  Wire.begin();

  ClockSettings settings;

  settings.timezone = 2.0;
  settings.isIsrael = 1;
  settings.latitude = 40.66896;
  settings.longitude = -73.94284;
  settings.elevation = 34;
  clockControler.init(settings);
  initBrightness();
  setupButtons();
}

void onTick()
{
  onTickButtons();
  
  clockControler.onTick();
}

void loop()
{
  // delay(1000);
  
  onTick();
}
