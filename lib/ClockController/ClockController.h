#ifndef ClockController_h
#define ClockController_h

#include <DebugUtils.h>
#include <IDisplayWrapper.h>
#include <IRTCLibWrapper.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

extern "C"
{
    #include <hebrewcalendar.h>
}

typedef int32_t timestamp_t;
struct ClockSettings
{
    float timezone = 2.0; //-4.0;
    double latitude = 0;
    double longitude = 0;
    double elevation = 0;
    bool isDst = false;
    bool calcTimes = false;
    bool isIsrael = 1;
};

enum ConfigMode
{
    NONE,
    YEAR,
    MONTH,
    DAY,
    HOUR,
    MINUTE,
    SECOND,
    LOCATION,
    TIMEZONE

};

class ClockController
{

public:
    explicit ClockController(IDisplayWrapper *disp, IRTCLibWrapper *rtc);

    void init(const ClockSettings &settings);
    void changeMode(ConfigMode mode);
    ConfigMode getMode() const;
    void increment();
    void decrement();

    void onTick(timestamp_t tick);

    void displayMode(const ConfigMode mode);
    void displayTime(const TMWrapper& dtv);    
    hdate convertToHebrewDate( const TMWrapper& ltm) const;
    void displayHebDate(const hdate& hebrewDate);

private:
    
    TMWrapper _dtv = TMWrapper(0, 0, 0);
    TMWrapper _hebdtv = TMWrapper(0, 0, 0);

    IRTCLibWrapper *_rtc;
    ConfigMode _dp = NONE;
    IDisplayWrapper *_disp;
    timestamp_t _tickStamp = 0;
    ClockSettings _settings;
    const timestamp_t resolution = 50;
};

#endif