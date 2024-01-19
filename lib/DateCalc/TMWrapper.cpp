#include <TMWrapper.h>

int isLeapYear(year_t year)
{
    year = year <= tm_year_offset ? year + tm_year_offset : year;
    return ((year % 4) == 0) && (((year % 100) != 0) || ((year % 400) == 0));
}

day_t numDayInMonth(const year_t year, const month_t mon)
{
    month_t month = mon + 1;
    const year_t leap_year = isLeapYear(year) ? 1 : 0;
    const day_t num_day_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const int num_of_days = num_day_in_month[month - 1] + (month == 2 ? leap_year : 0);
    return num_of_days;
}

int dayOfWeek(year_t year, month_t month, day_t day)
{
    year = year <= tm_year_offset ? year + tm_year_offset : year;
    const static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    year -= month < 3;
    return (year + year / 4 - year / 100 + year / 400 + t[month - 1] + day) % 7;
}

int16_t dayOfYear(const year_t year, month_t month, day_t day)
{
    // Add the days in the previous months
    while (month-- > 0)
    {
        day = day + numDayInMonth(year, month);
    }
    return day - 1;
}

uint16_t bounds(const int16_t v, const int16_t minVal, const int16_t maxVal, const delta_t dir)
{
    const int16_t value = v + dir;
    if (dir > 0)
    {
        return value > maxVal ? minVal : value;
    }
    else
    {
        return value < minVal ? maxVal : value;
    }
}

TMWrapper::TMWrapper(const year_t year, const month_t month, const day_t day, const tm tm)
{
    _tm.tm_year = year;
    _tm.tm_mon = month;
    _tm.tm_mday = day;
    _tm.tm_hour = tm.tm_hour;
    _tm.tm_min = tm.tm_min;
    _tm.tm_sec = tm.tm_sec;
}

TMWrapper::TMWrapper(const tm tm, const hour_t hour, const minute_t minute, const second_t second)
{
    _tm.tm_year = tm.tm_year;
    _tm.tm_mon = tm.tm_mon;
    _tm.tm_mday = tm.tm_mday;
    _tm.tm_hour = hour;
    _tm.tm_min = minute;
    _tm.tm_sec = second;
}
TMWrapper::TMWrapper(const year_t year, const month_t month, const day_t day, const hour_t hour, const minute_t minute, const second_t second, const bool isDst)
{
    _tm.tm_year = year;
    _tm.tm_mon = month;
    _tm.tm_mday = day;
    _tm.tm_hour = hour;
    _tm.tm_min = minute;
    _tm.tm_sec = second;
}

TMWrapper &TMWrapper::modifyYear(const delta_t delta)
{
    _tm.tm_year = bounds(_tm.tm_year, 0, 999, delta);
    const int maxDays = numDayInMonth(_tm.tm_year, _tm.tm_mon);
    if (_tm.tm_mday > maxDays)
    {
        _tm.tm_mday = maxDays;
    }
    return *this;
}

TMWrapper &TMWrapper::modifyMonth(const delta_t delta)
{
    _tm.tm_mon = bounds(_tm.tm_mon, 0, 11, delta);
    const int maxDays = numDayInMonth(_tm.tm_year, _tm.tm_mon);
    if (_tm.tm_mday > maxDays)
    {
        _tm.tm_mday = maxDays;
    }
    return *this;
}

TMWrapper &TMWrapper::modifyDay(const delta_t delta)
{
    const int maxDays = numDayInMonth(_tm.tm_year, _tm.tm_mon);
    _tm.tm_mday = bounds(_tm.tm_mday, 1, maxDays, delta);
    return *this;
}

TMWrapper &TMWrapper::modifyHour(const delta_t delta)
{
    _tm.tm_hour = bounds(_tm.tm_hour, 0, 23, delta);
    return *this;
}
TMWrapper &TMWrapper::modifyMinute(const delta_t delta)
{
    _tm.tm_min = bounds(_tm.tm_min, 0, 59, delta);
    return *this;
}
TMWrapper &TMWrapper::modifySecond(const delta_t delta)
{
    _tm.tm_sec = 0;
    return *this;
}

char *TMWrapper::toDateTimeString(char *buffer, const size_t sz) const
{

    const tm ltm = get_tm();
#ifdef DISP_SECS
    strftime(buffer, sz, "%d/%m/%y %H:%M:%S", &ltm);
#else
    strftime(buffer, sz, "%d/%m/%y %H:%M", &ltm);
#endif
    return buffer;
}

const int64_t differenceInSeconds(const tm &start, const tm &end)
{
    tm f_ltm = start;
    tm t_ltm = end;
    const time_t from = mktime(&f_ltm);
    const time_t to = mktime(&t_ltm);
    const double diff = difftime(to, from);
    return (int64_t)diff;
}

long TMWrapper::diff(const TMWrapper &tm) const
{
    return differenceInSeconds(get_tm(), tm.get_tm());
}

const tm complete_tm(const tm &dtv)
{
    tm ltm = dtv;
    ltm.tm_wday = dayOfWeek(ltm.tm_year, ltm.tm_mon, ltm.tm_mday);
    ltm.tm_yday = dayOfYear(ltm.tm_year, ltm.tm_mon, ltm.tm_mday);
    ltm.tm_isdst = 0;
    return ltm;
}

const tm TMWrapper::get_tm() const
{
    tm ltm;
    ltm.tm_year = _tm.tm_year;
    ltm.tm_mon = _tm.tm_mon;
    ltm.tm_mday = _tm.tm_mday;
    ltm.tm_hour = _tm.tm_hour;
    ltm.tm_min = _tm.tm_min;
    ltm.tm_sec = _tm.tm_sec;
    ltm = complete_tm(ltm);
    return ltm;
}
