#ifndef ClockController_h
#define ClockController_h

#include <DebugUtils.h>
#include <IDisplayWrapper.h>
#include <IRTCLibWrapper.h>
extern "C"
{
    #include <HebDateDisplay.h>
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
    BRIGHTNESS,
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

    virtual void displayMode(const ConfigMode mode);
    virtual void displayTime(const TMWrapper& dtv);        
    virtual void displayHebDate(const HebDates &hebrewDate);

protected:
    IDisplayWrapper *_disp;
    IRTCLibWrapper *_rtc;

private:
    TMWrapper _dtv = TMWrapper(0, 0, 0);
    TMWrapper _hebdtv = TMWrapper(0, 0, 0);

    ConfigMode _dp = NONE;
    timestamp_t _tickStamp = 0;
    ClockSettings _settings;
    const timestamp_t resolution = 50;
};

#endif