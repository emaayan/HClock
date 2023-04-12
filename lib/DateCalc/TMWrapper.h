#ifndef C82E444B_1CB6_45C6_B8FD_DD8BC278857C
#define C82E444B_1CB6_45C6_B8FD_DD8BC278857C

#include <stdint.h>
#include <time.h>
#include <DebugUtils.h>

typedef uint16_t year_t;
typedef uint8_t month_t;
typedef uint8_t day_t;
typedef uint8_t hour_t;
typedef uint8_t minute_t;
typedef uint8_t second_t;
typedef int16_t delta_t;
const year_t tm_year_offset = 1900;
const month_t tm_month_offset = 1;
// struct tm {
//    int tm_sec;         /* seconds,  range 0 to 59          */
//    int tm_min;         /* minutes, range 0 to 59           */
//    int tm_hour;        /* hours, range 0 to 23             */
//    int tm_mday;        /* day of the month, range 1 to 31  */
//    int tm_mon;         /* month, range 0 to 11             */
//    int tm_year;        /* The number of years since 1900   */
//    int tm_wday;        /* day of the week, range 0 to 6    */
//    int tm_yday;        /* day in the year, range 0 to 365  */
//    int tm_isdst;       /* daylight saving time             */
// };

class TMWrapper
{

public:
    TMWrapper(const year_t year, const month_t month, const day_t day, hour_t hour = 0, minute_t minute = 0, second_t second = 0, bool isDst = false);
    explicit TMWrapper(const tm tm);    
    TMWrapper &modifyYear(const delta_t delta);
    TMWrapper &modifyMonth(const delta_t delta);
    TMWrapper &modifyDay(const delta_t delta);
    TMWrapper &modifyHour(const delta_t delta);
    TMWrapper &modifyMinute(const delta_t delta);
    TMWrapper &modifySecond(const delta_t delta);
    char *toDateTimeString(char *buffer, const size_t sz) const;
    long diff(const TMWrapper& tm) const;
    const tm get_tm() const;

private:
    struct DateTimeValue
    {
        year_t tm_year;
        month_t tm_mon;  //: 4;
        day_t tm_mday;   //: 5;
        hour_t tm_hour;  //: 5;
        minute_t tm_min; //: 6;
        second_t tm_sec; //: 6;
    };
    DateTimeValue _tm;
};
#endif /* C82E444B_1CB6_45C6_B8FD_DD8BC278857C */
