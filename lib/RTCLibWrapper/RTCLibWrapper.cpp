
#include <RTCLibWrapper.h>
#include <time.h>
void RTCLibWrapper::init()
{

    if (!_rtcCtrl.begin())
    {
        while (1)
            delay(10);
    }
    if (_rtcCtrl.lostPower())
    {
        _rtcCtrl.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
}

DateTime fromDateTimeValue(const TMWrapper& dtv)
{
    const tm tm= dtv.get_tm();

    DateTime dt = DateTime(tm.tm_year+tm_year_offset,tm.tm_mon+tm_month_offset, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return dt;
}

TMWrapper toDateTimeValue(const DateTime dt)
{
    TMWrapper tm(dt.year()-tm_year_offset, dt.month()-tm_month_offset, dt.day(), dt.hour(), dt.minute(), dt.second(), false);    
    return tm;  
}

TMWrapper RTCLibWrapper::now()
{    
    const DateTime dt = _rtcCtrl.now();    
    return toDateTimeValue(dt);
}

void RTCLibWrapper::changeTime(const TMWrapper dtv)
{
    DateTime dt = fromDateTimeValue(dtv);
    _rtcCtrl.adjust(dt);
}
