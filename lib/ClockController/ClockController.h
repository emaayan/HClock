#ifndef ClockController_h
#define ClockController_h

#include <DebugUtils.h>
#include <IDisplayWrapper.h>
#include <IRTCLibWrapper.h>


extern "C"
{
#include <hebrewcalendar.h>
#include <hdateformat.h>
#include <zmanim.h>
}
struct ClockSettings
{
    float timezone = 2.0; //-4.0;
    double latitude = 0;
    double longitude = 0;
    double elevation = 0;
    bool isIsrael = 1;
};
class ClockControler
{

public:
    explicit ClockControler(IDisplayWrapper *disp,IRTCLibWrapper * rtc)
    {
        _disp = disp;
        _rtc=rtc;
        
    }
    void init(const ClockSettings &settings)
    {
        _settings = settings;
        _tickStamp = millis();
        _rtc->init();
        _dtv = _rtc->now();
        _displayeddtv = _dtv;

        _disp->init();
        _disp->clear();
        _disp->lightUp();
        //  debug("INit");
        displayTime(_dtv);
        displayHebDate(_displayeddtv);
        displayConfigMode(RTCLibWrapper::NONE);
    }
   

    void changeMode(RTCLibWrapper::DatePart mode)
    {
        displayConfigMode(mode);
    }

    RTCLibWrapper::DatePart getMode()
    {
        return _dp;
    }

    void increment()
    {
        RTCLibWrapper::DateTimeValue dtv = _rtc->increment(_dp, _dtv);
        _rtc->changeTime(dtv);
    }

    void decrement()
    {
        RTCLibWrapper::DateTimeValue dtv = _rtc->decrement(_dp, _dtv);
        _rtc->changeTime(dtv);
    }

    void onTick()
    {
        unsigned long tick = millis();
        unsigned long resolution = 200;
        unsigned long delta = tick - _tickStamp;
        //  debug("Tick %d %d ",tick,delta);
        if (delta > resolution)
        {
            onTickChanged(delta);
            _tickStamp = tick;
        }
    }
    void onTickChanged(unsigned long tickDelta)
    {
        RTCLibWrapper::DateTimeValue dtv = _rtc->now();
        if (abs(_rtc->differenceInSeconds(dtv, _dtv)) >= 1)
        {
            displayTime(dtv);
        }
        if (abs(_rtc->differenceInSeconds(_displayeddtv, _dtv)) >= 60)
        {
            displayHebDate(_dtv);
        }
    }

    void displayConfigMode(RTCLibWrapper::DatePart dp)
    {
        char confMode = getConfigMode(dp);
        const char c[2] = {confMode, '\0'};
        _disp->display(c, 19, 0, false);
        _dp = dp;
    }

    void displayTime(RTCLibWrapper::DateTimeValue dtv)
    {
        char fmt[] = "DD/MM/YYYY hh:mm";
        char *time = _rtc->toString(dtv, fmt);
        _disp->println(0, false, "%-19s", time);
        _dtv = dtv;
    }

    void displayHebrewDayMonth(const hdate &hebrewDate)
    {
        const char *day_name = numtowday(hebrewDate, 1);

        char dayInMonth[3 * 2] = "";
        numtohchar(dayInMonth, sizeof(dayInMonth), hebrewDate.day);
        const char *monthName = numtohmonth(hebrewDate.month, hebrewDate.leap);

        const yomtov isNewMonth = getroshchodesh(hebrewDate);
        char isNewMonthIndicator[9 + 1] = "";
        const long sz_month = sizeof(isNewMonthIndicator);
        if (isNewMonth != CHOL)
        {
            const char *buf = PSTR(",ר\"ח"); // yomtovformat(isNewMonth);
            strncpy_P(isNewMonthIndicator, buf, sz_month);
        }
        else
        {
            strncpy(isNewMonthIndicator, "", sz_month);
        }

        _disp->println(1, true, "%s, %s %s %s", day_name, dayInMonth, monthName, isNewMonthIndicator);
    }

    void displayFestival(const hdate &hebrewDate)
    {
        const yomtov yom_tov = getyomtov(hebrewDate);    
        const char * yom_tov_name =yomtovformat(yom_tov);        
        _disp->println(2, true, "%s", yom_tov_name);
    }

    char *formattime(const hdate &date)
    {
        static char final[32];
        final[0] = '\0';
        time_t time = hdatetime_t(date);
        struct tm *tm = localtime(&time);
        strftime(final, 31, "%I:%M %p %Z", tm);
        return final;
    }

    void displayOmer(const hdate &hebrewDate)
    {

        char omer_count_name[(9 * 2) + 1] = "";
        const long omer_count_size = sizeof(omer_count_name);

        const int omer_count = getomer(hebrewDate);
        if (omer_count)
        {
            char omer_day[5 + 1] = "";
            numtohchar(omer_day, sizeof(omer_day), omer_count);
            snprintf(omer_count_name, omer_count_size, "%s בעומר", omer_day);
        }

 
        _disp->println(3, true, "%s", omer_count_name);
    }

    hdate convertToHebrewDate(RTCLibWrapper::DateTimeValue dtv)
    {
        const tm ltm = _rtc->convertToStdTime(dtv);
        hdate hebrewDate = convertDate(ltm);
        hebrewDate.EY = _settings.isIsrael; // if it's in israel TODO: cofigure per location
        const long int offset = (long int)3600 * _settings.timezone;
        hebrewDate.offset = offset;
        return hebrewDate;
    }

    void displayHebDate(RTCLibWrapper::DateTimeValue dtv)
    {
        hdate hebrewDate = convertToHebrewDate(dtv);
        displayHebrewDayMonth(hebrewDate);
        displayFestival(hebrewDate);
        displayOmer(hebrewDate);

        _displayeddtv = dtv;
    }


    char getConfigMode(RTCLibWrapper::DatePart dp)
    {
        switch (dp)
        {
        case RTCLibWrapper::NONE:
            return ' ';
            break;
        case RTCLibWrapper::YEAR:
            return 'Y';
            break;
        case RTCLibWrapper::MONTH:
            return 'M';
            break;
        case RTCLibWrapper::DAY:
            return 'D';
            break;
        case RTCLibWrapper::HOUR:
            return 'h';
            break;
        case RTCLibWrapper::MINUTE:
            return 'm';
            break;
        case RTCLibWrapper::SECOND:
            return 's';
            break;
        default:
            return ' ';
        }
    }

private:
    IRTCLibWrapper::DateTimeValue _dtv=         {.year=0,.month=0,.day=0,.dayOfWeek=0,.hour=0,.minute=0,.second=0,.timestamp=0};
    IRTCLibWrapper::DateTimeValue _displayeddtv={.year=0,.month=0,.day=0,.dayOfWeek=0,.hour=0,.minute=0,.second=0,.timestamp=0};
    IRTCLibWrapper * _rtc;
    IRTCLibWrapper::DatePart _dp = IRTCLibWrapper::NONE;
    IDisplayWrapper *_disp;
    unsigned long _tickStamp = 0;
    ClockSettings _settings;
};

#endif