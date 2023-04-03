#include <Arduino.h>
#include <Wire.h>
#include <OneButton.h>
#include <Settings.h>

#include <ClockController.h>
#include <RTCLibWrapper.h>
#include <DisplayWrapper.h>

#include <DebugUtils.h>

#ifdef DEBUG_AVR
#include <avr8-stub.h>
#endif

IDisplayWrapper *_disp = new DisplayWrapper();
IRTCLibWrapper *_rtc = new RTCLibWrapper();
ClockController clockControler(_disp, _rtc);

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
  clockControler.changeMode(NONE);
}
void onInitConfigEnter()
{
  ConfigMode dp = clockControler.getMode();
  if (dp != NONE)
  {
    onExitConfigEnter();
  }
  else
  {
    clockControler.changeMode(YEAR);
  }
}

void onSwitchConfigMode()
{

  ConfigMode dp = clockControler.getMode();

  switch (clockControler.getMode())
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
    dp = NONE;
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

void sendBrightness(uint8_t brightness)
{
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
  int8_t i = dir ? -1 : 1; // this is because PNP transistor
  int8_t setting = i * brightAdjResolution;
  uint8_t curSetting = brightness;
  uint8_t settingToBe = curSetting + setting;
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
  ConfigMode dp = clockControler.getMode();
  if (dp != NONE)
  {
    clockControler.increment();
  }
  else
  {
    //  incBrightness();
  }
}

void onDecrement()
{
  ConfigMode dp = clockControler.getMode();
  if (dp != NONE)
  {
    clockControler.decrement();
  }
  else
  {
    //    decBrightness();
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
// unsigned long t;
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
  setenv("TZ", "IST-2IDT,M3.4.4/26,M10.5.0", 1);
  tzset();
  settings.timezone = 2.0;
  settings.isIsrael = 1;
  settings.latitude = 32.109333;
  settings.longitude = 34.855499;
  settings.elevation = 34;
  settings.calcTimes = true;
  clockControler.init(settings);

  //_rtc->changeTime(TMWrapper(123,2,27,23,23,0));

  // initBrightness();
  setupButtons();
  // t=millis();
}

void onTick()
{
  onTickButtons();

  clockControler.onTick(millis());
}

void loop()
{
  // if (millis()-t>5000){
  //   // clockControler.changeMode(MONTH);
  //   // clockControler.increment();
  //   t=millis();
  // }
  // t=millis();
  // delay(1000);

  onTick();
}
