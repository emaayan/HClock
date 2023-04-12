#include <Arduino.h>

#include <IRTCLibWrapper.h>
#include <RTCLibWrapper.h>

// #include <ClockController.h>

#include <IDisplayWrapper.h>
#include <LCDDisplayWrapper.h>

#include <OneButton.h>
extern "C"
{
#include <HebDateDisplay.h>
}

IDisplayWrapper *_disp = new LCDDisplayWrapper();
IRTCLibWrapper *_rtc = new RTCLibWrapper();

enum ConfigMode
{
    NONE,
    YEAR,
    MONTH,
    DAY,
    HOUR,
    MINUTE,
    SECOND,
    BRIGHTNESS
};
ConfigMode _dp = NONE;

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
TMWrapper tmw = TMWrapper(0, 0, 0, 0, 0, 0);

// char getConfigMode(const ConfigMode dp)
// {
//     switch (dp)
//     {
//     case NONE:
//         return ' ';
//     case YEAR:
//         return 'Y';
//     case MONTH:
//         return 'M';
//     case DAY:
//         return 'D';
//     case HOUR:
//         return 'h';
//     case MINUTE:
//         return 'm';
//     case SECOND:
//         return 's';
//     case BRIGHTNESS:
//         return 'B';
//     default:
//         return ' ';
//     }
// }
// void displayMode(const ConfigMode mode)
// {
//     const char confMode = getConfigMode(mode);
//     const char c[2] = {confMode, '\0'};
//     _disp->display(c, 19, 0, false);
// }
// void changeMode(const ConfigMode mode)
// {
//     displayMode(mode);
//     _dp = mode;
// }

// void onExitConfigEnter()
// {
//     changeMode(NONE);
// }
// void onInitConfigEnter()
// {

//     if (_dp != NONE)
//     {
//         onExitConfigEnter();
//     }
//     else
//     {
//         changeMode(YEAR);
//     }
// }

// void onSwitchConfigMode()
// {
//     switch (_dp)
//     {
//     case YEAR:
//         _dp = MONTH;
//         break;
//     case MONTH:
//         _dp = DAY;
//         break;
//     case DAY:
//         _dp = HOUR;
//         break;
//     case HOUR:
//         _dp = MINUTE;
//         break;
//     case MINUTE:
//         _dp = SECOND;
//         break;
//     case SECOND:
//         _dp = BRIGHTNESS;
//         break;
//     case BRIGHTNESS:
//         _dp = NONE;
//         break;
//     default:
//         break;
//     }
// }

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
    brightness = 150; //_confSettings->initSetting(BRIGHT_ADDR, 100);
    sendBrightness(brightness);
}

// /*
//  *true increase, false decrease, every change write to EEPROM
//  */
// uint8_t setBrightness(bool dir)
// {
//     int8_t i = dir ? -1 : 1; // this is because PNP transistor
//     int8_t setting = i * brightAdjResolution;
//     uint8_t curSetting = brightness;
//     uint8_t settingToBe = curSetting + setting;
//     if (settingToBe > 0 && settingToBe < 255)
//     {
//         brightness = settingToBe;
//    //     _confSettings->writeSetting(BRIGHT_ADDR, brightness);
//         sendBrightness(brightness);
//     }
//     return brightness;
// }

// void incBrightness()
// {
//     setBrightness(true);
// }

// void decBrightness()
// {
//     setBrightness(false);
// }

// TMWrapper change(const ConfigMode dp, TMWrapper &dtv, const delta_t dir)
// {
//     switch (dp)
//     {
//     case YEAR:
//     {
//         return dtv.modifyYear(dir);
//     }
//     case MONTH:
//     {
//         return dtv.modifyMonth(dir);
//     }
//     case DAY:
//     {
//         return dtv.modifyDay(dir);
//     }
//     case HOUR:
//     {
//         return dtv.modifyHour(dir);
//     }
//     case MINUTE:
//     {
//         return dtv.modifyMinute(dir);
//     }
//     case SECOND:
//     {
//         return dtv.modifySecond(dir);
//     }
//     default:
//     {
//         return dtv;
//     }
//     }
//     return dtv;
// }
// void onIncrement()
// {
//     if (_dp != NONE && _dp != BRIGHTNESS)
//     {
//         tmw = change(_dp, tmw, 1); // clockController->increment();
//         _rtc->changeTime(tmw);
//     }
//     else
//     {
//         if (_dp == BRIGHTNESS)
//         {
//              incBrightness();
//         }
//     }
// }

// void onDecrement()
// {
//     if (_dp != NONE && _dp != BRIGHTNESS)
//     {
//         tmw = change(_dp, tmw, -1); // clockController->increment();
//         _rtc->changeTime(tmw);
//     }
//     else
//     {
//         if (_dp == BRIGHTNESS)
//         {
//              decBrightness();
//         }
//     }
// }
// void setupButtons()
// {
//     set_state_button.attachLongPressStart(onInitConfigEnter);
//     set_state_button.attachClick(onSwitchConfigMode);

//     increase_button.attachClick(onIncrement);
//     increase_button.attachDuringLongPress(onIncrement);
//     decrease_button.attachClick(onDecrement);
//     decrease_button.attachDuringLongPress(onDecrement);
// }


void setupRTC()
{
    _rtc->init();
}

void setupDisp()
{
   _disp->init();
   _disp->lightUp();
    initBrightness();
}

void setup()
{
    Wire.begin();
    setupRTC();
    setupDisp();
    // Serial.begin(115200);
   // setupButtons();
}

void onTickButtons()
{
    set_state_button.tick();
    increase_button.tick();
    decrease_button.tick();
}

unsigned long timstamp = 0;

TMWrapper h_tmw = TMWrapper(0, 0, 0, 0, 0, 0);

void onTick()
{
    unsigned long ts = millis();
    if (ts - timstamp > 60)
    {
        timstamp = millis();
        tmw = _rtc->now();
        char buff[18] = "";
        tmw.toDateTimeString(buff, sizeof(buff));
        _disp->println(0, false, "%-19s", buff);
    //    displayMode(_dp);
    }
    if (abs(tmw.diff(h_tmw)) > 6000)
    {
        const tm tm = tmw.get_tm();
        HebDates hr = {"", "", "", "", "", ""};
         displayHebDates(tm, true, 2, &hr);
      //  _disp->println(1, true, "%s, %s %s %s", hr.day_name, hr.dayInMonth, hr.monthName, hr.isNewMonthIndicator);
        // _disp->println(2, true, "%s", hr.festivalName);
        // _disp->println(3, true, "%s", hr.omer_count_name);
        h_tmw = tmw;
    }
    onTickButtons();
}
void loop()
{

    onTick();
    // Serial.println(hd.day_name);
}
