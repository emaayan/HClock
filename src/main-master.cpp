#include <Arduino.h>
#include <OneButton.h>

#include <StateMachine.h>
#include <LinkedList.h>
#include "ClockController.h"

// #define DEBUG_AVR

// https://playground.arduino.cc/Code/Time/
// https://kosherjava.com/zmanim-project/downloads/

// http://harel.org.il/zvi/calendar/gauss/gauss.pdf
// https://www.sciencedirect.com/science/article/pii/0898122177900931

// https://github.com/yparitcher/libzmanim
// #define DEBUG_CON

#include "Utils.h"

#ifdef DEBUG_AVR
#include <avr8-stub.h>
#endif

ClockControler clockControler;

/* #region  Machine */
StateMachine machine;

State *initState;
State *idleState;
State *readTimeState;

State *yearChange;
State *monthChange;
State *dayChange;

void setupMachine()
{
  initState = machine.addState([]()
                               { clockControler.onInit(); });

  idleState = machine.addState([]()
                               { clockControler.onIdle(); });

  initState->addTransition([]()
                           { return true; },
                           idleState);

  readTimeState = machine.addState([]()
                                   { clockControler.onReadTime(); });

  idleState->addTransition([]()
                           { return true; },
                           readTimeState);

  readTimeState->addTransition([]()
                               { return true; },
                               idleState);

  yearChange = machine.addState([]() {

  });

  monthChange = machine.addState([]() {});

  dayChange = machine.addState([]() {});
  ///  machine.transitionTo()
  //  yearChange->addTransition()
}

void onTickMachine()
{
  machine.run();
}
/* #endregion */

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
  set_state_button.attachLongPressStop(onInitConfigEnter);
  set_state_button.attachClick(onSwitchConfigMode);

  increase_button.attachClick(onIncrement);
  decrease_button.attachClick(onDecrement);
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

  clockControler.init();
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
   char cc[2]="";   
   sprintf(cc,"%s hello",cc);
   debug("%s ",cc);
  onTick();
}
