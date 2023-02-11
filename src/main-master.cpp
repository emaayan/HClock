
#include <Arduino.h>
#include <OneButton.h>
#include <IRTCLibWrapper.h>
#include <RTCLibWrapper.h>
#include <DisplayWrapper.h>
#include <ClockController.h>
#include <DebugUtils.h>
#ifdef DEBUG_AVR
#include <avr8-stub.h>
#endif

IDisplayWrapper * _disp=new DisplayWrapper();
IRTCLibWrapper * _rtc=new RTCLibWrapper();
ClockControler clockControler(_disp,_rtc);


void setupMachine()
{    
}

void onTickMachine()
{
  clockControler.onTick();

}


/* #region  Buttons */
const int RED_BUTTUN = 2;
const int GREEN_BUTTUN = 4;
const int BLUE_BUTTUN = 8;

OneButton set_state_button(RED_BUTTUN, false);
OneButton increase_button(GREEN_BUTTUN, false);
OneButton decrease_button(BLUE_BUTTUN, false);

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
void onIncrement()
{
  clockControler.increment();
}
void onDecrement()
{
  clockControler.decrement();
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
  
  settings.timezone=2.0;
  settings.isIsrael=1;
  settings.latitude=40.66896;
  settings.longitude=-73.94284;
  settings.elevation=34;
  clockControler.init(settings);
  setupMachine();
  setupButtons();  
}

void onTick()
{
  onTickButtons();
  onTickMachine();
}
int i = 0;
void loop()
{
 // delay(1000);
   
  onTick();
}
