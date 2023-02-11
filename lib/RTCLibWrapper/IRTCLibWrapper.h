#ifndef IRTCLibWrapper_h
#define IRTCLibWrapper_h
#include <time.h>

class IRTCLibWrapper
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
    virtual void init() = 0;
    virtual DateTimeValue now() = 0;
    virtual struct tm nowInStdLocalTime() = 0;
    virtual char *toString(DateTimeValue dtv, char *fmt) = 0;
    virtual char *toString(char *fmt) = 0;
    virtual void changeTime(DateTimeValue dtv) = 0;
    virtual int32_t differenceInSeconds(DateTimeValue leftdtv, DateTimeValue rightdtv) = 0;
    virtual DateTimeValue increment(DatePart dp, DateTimeValue dtv) = 0;
    virtual DateTimeValue decrement(DatePart dp, DateTimeValue dtv) = 0;
    virtual tm convertToStdTime(DateTimeValue dtv) = 0;
};
#endif