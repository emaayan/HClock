#include <ClockController.h>
extern "C"
{
#include <HebDateDisplay.h>
}

ClockController::ClockController(IDisplayWrapper *disp, IRTCLibWrapper *rtc)
{
    _disp = disp;
    _rtc = rtc;
}

void ClockController::init(const ClockSettings &settings)
{
    _settings = settings;

    _rtc->init();

    _disp->init();
    _disp->lightUp();
}

void ClockController::changeMode(const ConfigMode mode)
{
    displayMode(mode);
    _dp = mode;
}

ConfigMode ClockController::getMode() const
{
    return _dp;
}

TMWrapper change(const ConfigMode dp, TMWrapper &dtv, const delta_t dir)
{
    switch (dp)
    {
    case YEAR:
    {
        return dtv.modifyYear(dir);
    }
    case MONTH:
    {
        return dtv.modifyMonth(dir);
    }
    case DAY:
    {
        return dtv.modifyDay(dir);
    }
    case HOUR:
    {
        return dtv.modifyHour(dir);
    }
    case MINUTE:
    {
        return dtv.modifyMinute(dir);
    }
    case SECOND:
    {
        return dtv.modifySecond(dir);
    }
    default:
    {
        return dtv;
    }
    }
    return dtv;
}

void ClockController::increment()
{
    TMWrapper dtv = change(_dp, _dtv, 1);
    _rtc->changeTime(dtv);
}

void ClockController::decrement()
{
    TMWrapper dtv = change(_dp, _dtv, -1);
    _rtc->changeTime(dtv);
}

void ClockController::onTick(const timestamp_t tick)
{

    const timestamp_t delta = tick - _tickStamp;
    //  debug("Tick %d %d ",tick,delta);
    if (_tickStamp == 0 || delta > resolution)
    {
        if (tick - _tickStamp >= delta)
        {
            const TMWrapper dtv = _rtc->now();
            displayTime(dtv);
            _dtv = _rtc->now();
        }
        if (abs(_dtv.diff(_hebdtv)) >= 60)
        {
            HebDates hr = {"", "", "", "", "", ""};
            displayHebDates(_dtv.get_tm(), _settings.isIsrael,_settings.timezone, &hr);            
            displayHebDate(hr);
            _hebdtv = _dtv;
        }
        _tickStamp = tick;
    }
}




void ClockController::displayMode(const ConfigMode mode)
{
    // const char confMode = getConfigMode(mode);
    // const char c[2] = {confMode, '\0'};
    // _disp->display(c, 19, 0, false);
}

void ClockController::displayTime(const TMWrapper &dtv)
{
    //   char buffer[18] = "";
    // dtv.toDateTimeString(buffer, size_t(buffer));
    // _disp->println(0, false, "%-19s", buffer);   
}

void ClockController::displayHebDate(const HebDates &hebrewDate)
{  
    //  HebDates hr = {"", "", "", "", "", ""};
    // displayHebrewDayMonth(hebrewDate, &hr);
    //  displayFestival(hebrewDate, &hr);
    //  displayOmer(hebrewDate, &hr);

    // _disp->println(1, true, "%s, %s %s %s", hr.day_name, hr.dayInMonth, hr.monthName, hr.isNewMonthIndicator);
    //  _disp->println(2, true, "%s", hr.festivalName);
    //  _disp->println(3, true, "%s", hr.omer_count_name);
 
}
