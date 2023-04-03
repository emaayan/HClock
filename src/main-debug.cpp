#include <Arduino.h>
#include <Wire.h>

#include <RTCLibWrapper.h>


#ifdef AVR_DEBUG
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr8-stub.h>
#include <app_api.h>
#endif

IRTCLibWrapper *_rtc = new RTCLibWrapper();

extern "C"
{
#include <hebrewcalendar.h>
#include <hdateformat.h>
}

void setup()
{
#ifdef AVR_DEBUG
    debug_init();
#endif
}
void loop()
{
    delay(200);
    //  struct tm ltm; // = *pltm;
    // ltm.tm_sec = 27;
    // ltm.tm_min = 16;
    // ltm.tm_hour = 16;
    // ltm.tm_mday = 20;
    // ltm.tm_mon = 0;
    // ltm.tm_year = 123;
    // ltm.tm_wday = 5;
    // ltm.tm_yday = 19;
    // ltm.tm_isdst = 0;
    // breakpoint();
    hdate hebrewDate; //= convertDate(ltm);
    hebrewDate.year = 5783;
    hebrewDate.month = 2;
    hebrewDate.day = 14;
    hebrewDate.hour = 18;
    hebrewDate.min = 0;
    hebrewDate.sec = 41;
    hebrewDate.msec = 0;
    hebrewDate.wday = 6;
    hebrewDate.dayofyear = 22;

    parshah p = getparshah(hebrewDate);

    long int i = HebrewCalendarElapsedDays(5783); // HebrewCalendarElapsedDays(5783);
    if (i == 0)
    {
    }
    //     delay(i);
    // if (i == 0)
    // {
    // }
    // hdate hebrewDate = convertDate(ltm);
}