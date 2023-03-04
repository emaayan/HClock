#include <RTClib.h>
#include <time.h>
#include <RTCLibWrapper.h>

DateTime fromDateTimeValue(RTCLibWrapper::DateTimeValue dtv)
{

    DateTime dt = DateTime(dtv.year, dtv.month, dtv.day, dtv.hour, dtv.minute, dtv.second);
    return dt;
}

int isLeapYear(int year)
{
    return ((year % 4) == 0) && (((year % 100) != 0) || ((year % 400) == 0));
}

const int num_day_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
int numDayInMonth(int year, int month)
{
    int leap_year = 0;
    int num_of_days = 0;
    if (isLeapYear(year) == true)
    {
        leap_year = 1;
    }
    if (month == 2)
    {
        num_of_days = num_day_in_month[1] + leap_year;
    }
    else
    {
        num_of_days = num_day_in_month[month - 1];
    }
    return num_of_days;
}

int dayofweek(int d, int m, int y)
{
    y += 2000;
    const static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    y -= m < 3;
    return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}
int16_t dayOfYear(int16_t year, int16_t month, int16_t day)
{
    // Add the days in the previous months
    while (month-- > 0)
    {
        day = day + numDayInMonth(year, month); // num_day_in_month[month - 1];
    }
    return day - 1;
}

tm RTCLibWrapper::convertToStdTime(RTCLibWrapper::DateTimeValue dtv)
{
    tm dtvl;
    dtvl.tm_year = dtv.year - 1900;
    dtvl.tm_mon = dtv.month - 1;
    dtvl.tm_mday = dtv.day;
    dtvl.tm_yday = dayOfYear(dtv.year, dtv.month, dtv.day); // dtv.day;
    dtvl.tm_wday = dtv.dayOfWeek;
    dtvl.tm_hour = dtv.hour;
    dtvl.tm_min = dtv.minute;
    dtvl.tm_sec = dtv.second;
    dtvl.tm_isdst = 0;
    return dtvl;
}

void RTCLibWrapper::init()
{
    if (!_rtc.begin())
    {
        while (1)
            delay(10);
    }
    if (_rtc.lostPower())
    {
        _rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
}

RTCLibWrapper::DateTimeValue toDateTimeValue(DateTime dt)
{
    RTCLibWrapper::DateTimeValue dtv;
    dtv.year = dt.year();
    dtv.month = dt.month();
    dtv.day = dt.day();
    dtv.dayOfWeek = dt.dayOfTheWeek();
    dtv.hour = dt.hour();
    dtv.minute = dt.minute();
    dtv.second = dt.second();
    dtv.timestamp = dt.unixtime();
    return dtv;
}

RTCLibWrapper::DateTimeValue RTCLibWrapper::now()
{
    DateTime dt = _rtc.now();
    return toDateTimeValue(dt);
}

struct tm RTCLibWrapper::nowInStdLocalTime()
{
    DateTime dt = _rtc.now();
    RTCLibWrapper::DateTimeValue dtv = toDateTimeValue(dt);
    return convertToStdTime(dtv);
}

char *RTCLibWrapper::toString(RTCLibWrapper::DateTimeValue dtv, char *fmt)
{
    DateTime dt = fromDateTimeValue(dtv);
    return dt.toString(fmt);
}
char *RTCLibWrapper::toString(char *fmt)
{
    return toString(now(), fmt);
}

void RTCLibWrapper::changeTime(RTCLibWrapper::DateTimeValue dtv)
{
    DateTime dt = fromDateTimeValue(dtv);
    _rtc.adjust(dt);
}

int32_t RTCLibWrapper::differenceInSeconds(RTCLibWrapper::DateTimeValue leftdtv, RTCLibWrapper::DateTimeValue rightdtv)
{
    DateTime leftdt = fromDateTimeValue(leftdtv);
    DateTime rightdt = fromDateTimeValue(rightdtv);
    TimeSpan ts = leftdt - rightdt;
    return ts.totalseconds();
}

RTCLibWrapper::DateTimeValue RTCLibWrapper::increment(RTCLibWrapper::DatePart dp, RTCLibWrapper::DateTimeValue dtv)
{
    switch (dp)
    {
    case RTCLibWrapper::YEAR:
    {
        int year = dtv.year == 99 ? 0 : dtv.year + 1;
        int day = dtv.day <= numDayInMonth(dtv.year, dtv.month) ? dtv.day : numDayInMonth(dtv.year, dtv.month);
        DateTime dateTime(year, dtv.month, day, dtv.hour, dtv.minute, dtv.second);
        return toDateTimeValue(dateTime);
    }
    case RTCLibWrapper::MONTH:
    {
        int month = dtv.month == 12 ? 1 : dtv.month + 1;
        int day = dtv.day <= numDayInMonth(dtv.year, dtv.month) ? dtv.day : numDayInMonth(dtv.year, dtv.month);
        DateTime dateTime(dtv.year, month, day, dtv.hour, dtv.minute, dtv.second);
        return toDateTimeValue(dateTime);
    }
    case RTCLibWrapper::DAY:
    {

        DateTime dt = fromDateTimeValue(dtv);
        dt = dt + TimeSpan(1, 0, 0, 0);
        return toDateTimeValue(dt);
        // int day = dtv.day == numDayInMonth(dtv.year, dtv.month) ? 1 : dtv.day + 1;
        // DateTime dateTime(dtv.year, dtv.month, day, dtv.hour, dtv.minute, dtv.second);
        // return toDateTimeValue(dateTime);
    }
    case RTCLibWrapper::HOUR:
    {
        DateTime dt = fromDateTimeValue(dtv);
        dt = dt + TimeSpan(0, 1, 0, 0);
        return toDateTimeValue(dt);
        // int hour = dtv.hour == 23 ? 0 : dtv.hour + 1;
        // DateTime dateTime(dtv.year, dtv.month, dtv.day, hour, dtv.minute, dtv.second);
        // return toDateTimeValue(dateTime);
    }
    case RTCLibWrapper::MINUTE:
    {
        DateTime dt = fromDateTimeValue(dtv);
        dt = dt + TimeSpan(0, 0, 1, 0);
        return toDateTimeValue(dt);
        // int minute = dtv.minute == 59 ? 0 : dtv.minute + 1;
        // DateTime dateTime(dtv.year, dtv.month, dtv.day, dtv.hour, minute, dtv.second);
        // return toDateTimeValue(dateTime);
    }
    case RTCLibWrapper::SECOND:
    {
        DateTime dateTime(dtv.year, dtv.month, dtv.day, dtv.hour, dtv.minute, 0);
        return toDateTimeValue(dateTime);
    }
    default:
    {
        DateTime dateTime(dtv.year, dtv.month, dtv.day, dtv.hour, dtv.minute, 0);
        return toDateTimeValue(dateTime);
    }
    }
}
RTCLibWrapper::DateTimeValue RTCLibWrapper::decrement(RTCLibWrapper::DatePart dp, RTCLibWrapper::DateTimeValue dtv)
{
    switch (dp)
    {
    case RTCLibWrapper::YEAR:
    {
        int year = dtv.year == 0 ? 99 : dtv.year - 1;
        int day = dtv.day <= numDayInMonth(dtv.year, dtv.month) ? dtv.day : numDayInMonth(dtv.year, dtv.month);
        DateTime dateTime(year, dtv.month, day, dtv.hour, dtv.minute, dtv.second);
        return toDateTimeValue(dateTime);
    }
    case RTCLibWrapper::MONTH:
    {
        int month = dtv.month == 1 ? 12 : dtv.month - 1;
        int day = dtv.day <= numDayInMonth(dtv.year, dtv.month) ? dtv.day : numDayInMonth(dtv.year, dtv.month);
        DateTime dateTime(dtv.year, month, day, dtv.hour, dtv.minute, dtv.second);
        return toDateTimeValue(dateTime);
    }
    case RTCLibWrapper::DAY:
    {
        DateTime dt = fromDateTimeValue(dtv);
        dt = dt - TimeSpan(1, 0, 0, 0);
        return toDateTimeValue(dt);
        // int day = dtv.day == 1 ? numDayInMonth(dtv.year, dtv.month) : dtv.day - 1;
        // DateTime dateTime(dtv.year, dtv.month, day, dtv.hour, dtv.minute, dtv.second);
        // return toDateTimeValue(dateTime);
    }
    case RTCLibWrapper::HOUR:
    {
        DateTime dt = fromDateTimeValue(dtv);
        dt = dt - TimeSpan(0, 1, 0, 0);
        // int hour = dtv.hour == 0 ? 23 : dtv.hour - 1;
        // DateTime dateTime(dtv.year, dtv.month, dtv.day, hour, dtv.minute, dtv.second);
        return toDateTimeValue(dt);
    }
    case RTCLibWrapper::MINUTE:
    {
        DateTime dt = fromDateTimeValue(dtv);
        dt = dt - TimeSpan(0, 0, 1, 0);
        // int minute = dtv.minute == 0 ? 59 : dtv.minute - 1;
        // DateTime dateTime(dtv.year, dtv.month, dtv.day, dtv.hour, minute, dtv.second);
        return toDateTimeValue(dt);
    }
    case RTCLibWrapper::SECOND:
    {
        DateTime dateTime(dtv.year, dtv.month, dtv.day, dtv.hour, dtv.minute, 0);
        return toDateTimeValue(dateTime);
    }
    default:
    {
        DateTime dateTime(dtv.year, dtv.month, dtv.day, dtv.hour, dtv.minute, dtv.second);
        return toDateTimeValue(dateTime);
    }
    }
}


