#ifndef RTCLibWrapper_h
#define RTCLibWrapper_h

#include <RTClib.h>
#include <time.h>
class RTCLibWrapper
{
public:
    enum DatePart
    {
        NONE,
        YEAR,
        MONTH,
        DAY,
        HOUR,
        MINUTE,
        SECOND
    };
    struct DateTimeValue
    {
        int16_t year;
        int8_t month;
        int8_t day;
        int8_t dayOfWeek;
        int8_t hour;
        int8_t minute;
        int8_t second;
        uint32_t timestamp;
    };
    void init()
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
    DateTimeValue now()
    {
        DateTime dt = _rtc.now();
        return toDateTimeValue(dt);
    }
    struct tm nowInStdLocalTime()
    {
        DateTime dt = _rtc.now();
        DateTimeValue dtv = toDateTimeValue(dt);
        return convertToStdTime(dtv);
    }

    char *toString(DateTimeValue dtv, char *fmt)
    {
        DateTime dt = fromDateTimeValue(dtv);
        return dt.toString(fmt);
    }
    char *toString(char *fmt)
    {
        return toString(now(), fmt);
    }

    void changeTime(DateTimeValue dtv)
    {
        DateTime dt = fromDateTimeValue(dtv);
        _rtc.adjust(dt);
    }
    int32_t differenceInSeconds(DateTimeValue leftdtv, DateTimeValue rightdtv)
    {
        DateTime leftdt = fromDateTimeValue(leftdtv);
        DateTime rightdt = fromDateTimeValue(rightdtv);
        TimeSpan ts = leftdt - rightdt;
        return ts.totalseconds();
    }
    DateTimeValue increment(DatePart dp, DateTimeValue dtv)
    {
        switch (dp)
        {
        case YEAR:
        {
            int year = dtv.year == 99 ? 0 : dtv.year + 1;
            int day = dtv.day <= numDayInMonth(dtv.year, dtv.month) ? dtv.day : numDayInMonth(dtv.year, dtv.month);
            DateTime dateTime(year, dtv.month, day, dtv.hour, dtv.minute, dtv.second);
            return toDateTimeValue(dateTime);
        }
        case MONTH:
        {
            int month = dtv.month == 12 ? 1 : dtv.month + 1;
            int day = dtv.day <= numDayInMonth(dtv.year, dtv.month) ? dtv.day : numDayInMonth(dtv.year, dtv.month);
            DateTime dateTime(dtv.year, month, day, dtv.hour, dtv.minute, dtv.second);
            return toDateTimeValue(dateTime);
        }
        case DAY:
        {

            DateTime dt = fromDateTimeValue(dtv);
            dt = dt + TimeSpan(1, 0, 0, 0);
            return toDateTimeValue(dt);
            // int day = dtv.day == numDayInMonth(dtv.year, dtv.month) ? 1 : dtv.day + 1;
            // DateTime dateTime(dtv.year, dtv.month, day, dtv.hour, dtv.minute, dtv.second);
            // return toDateTimeValue(dateTime);
        }
        case HOUR:
        {
            DateTime dt = fromDateTimeValue(dtv);
            dt = dt + TimeSpan(0, 1, 0, 0);
            return toDateTimeValue(dt);
            // int hour = dtv.hour == 23 ? 0 : dtv.hour + 1;
            // DateTime dateTime(dtv.year, dtv.month, dtv.day, hour, dtv.minute, dtv.second);
            // return toDateTimeValue(dateTime);
        }
        case MINUTE:
        {
            DateTime dt = fromDateTimeValue(dtv);
            dt = dt + TimeSpan(0, 0, 1, 0);
            return toDateTimeValue(dt);
            // int minute = dtv.minute == 59 ? 0 : dtv.minute + 1;
            // DateTime dateTime(dtv.year, dtv.month, dtv.day, dtv.hour, minute, dtv.second);
            // return toDateTimeValue(dateTime);
        }
        case SECOND:
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
    DateTimeValue decrement(DatePart dp, DateTimeValue dtv)
    {
        switch (dp)
        {
        case YEAR:
        {
            int year = dtv.year == 0 ? 99 : dtv.year - 1;
            int day = dtv.day <= numDayInMonth(dtv.year, dtv.month) ? dtv.day : numDayInMonth(dtv.year, dtv.month);
            DateTime dateTime(year, dtv.month, day, dtv.hour, dtv.minute, dtv.second);
            return toDateTimeValue(dateTime);
        }
        case MONTH:
        {
            int month = dtv.month == 1 ? 12 : dtv.month - 1;
            int day = dtv.day <= numDayInMonth(dtv.year, dtv.month) ? dtv.day : numDayInMonth(dtv.year, dtv.month);
            DateTime dateTime(dtv.year, month, day, dtv.hour, dtv.minute, dtv.second);
            return toDateTimeValue(dateTime);
        }
        case DAY:
        {
            DateTime dt = fromDateTimeValue(dtv);
            dt = dt - TimeSpan(1, 0, 0, 0);
            return toDateTimeValue(dt);
            // int day = dtv.day == 1 ? numDayInMonth(dtv.year, dtv.month) : dtv.day - 1;
            // DateTime dateTime(dtv.year, dtv.month, day, dtv.hour, dtv.minute, dtv.second);
            // return toDateTimeValue(dateTime);
        }
        case HOUR:
        {
            DateTime dt = fromDateTimeValue(dtv);
            dt = dt - TimeSpan(0, 1, 0, 0);
            // int hour = dtv.hour == 0 ? 23 : dtv.hour - 1;
            // DateTime dateTime(dtv.year, dtv.month, dtv.day, hour, dtv.minute, dtv.second);
            // return toDateTimeValue(dateTime);
        }
        case MINUTE:
        {
            DateTime dt = fromDateTimeValue(dtv);
            dt = dt - TimeSpan(0, 0, 1, 0);
            // int minute = dtv.minute == 0 ? 59 : dtv.minute - 1;
            // DateTime dateTime(dtv.year, dtv.month, dtv.day, dtv.hour, minute, dtv.second);
            // return toDateTimeValue(dateTime);
        }
        case SECOND:
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
    DateTimeValue addDays(DateTimeValue dtv)
    {
        int numOfDayInMonth = numDayInMonth(dtv.month, dtv.year);
        int diff = 0;
        if (dtv.day > numOfDayInMonth)
        {
            diff = dtv.day - numOfDayInMonth;
        }
        DateTime dt = fromDateTimeValue(dtv) + TimeSpan(diff, 0, 0, 0);
        return toDateTimeValue(dt);
    }
    tm convertToStdTime(DateTimeValue dtv)
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

private:
    DateTimeValue toDateTimeValue(DateTime dt)
    {
        DateTimeValue dtv;
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

    DateTime fromDateTimeValue(DateTimeValue dtv)
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
        // If current year is a leap year and the date
        // given is after the 28th of February then
        // it must include the 29th February
        // if (month > 2 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
        // {
        //     ++day;
        // }

        // Add the days in the previous months
        while (month-- > 0)
        {
            day = day + numDayInMonth(year, month); // num_day_in_month[month - 1];
        }
        return day - 1;
    }

    RTC_DS3231 _rtc;
};
#endif