#ifndef ClockController_h
#define ClockController_h

#include "Utils.h"
#include "IDisplayWrapper.h"
#include "DisplayWrapper.h"
#include "RTCLibWrapper.h"



#include <hebrewcalendar.h>
#include <hdateformat.h>

class ClockControler
{

public:
    explicit ClockControler(IDisplayWrapper *disp) //:_disp(address,lcd_cols,lcd_rows)
    {
        _disp = disp;
    }
    void init()
    {
        _tickStamp = millis();
        _rtc.init();
        _dtv = _rtc.now();
        _displayeddtv = _dtv;

        _disp->init();
        _disp->clear();
        _disp->lightUp();
        //  debug("INit");
    }
    void onInit()
    {
        displayTime(_dtv);
        displayHebDate(_displayeddtv);
        displayConfigMode(RTCLibWrapper::NONE);
        //   debug("InitStates");
    }

    void onIdle()
    {
        // debug("IdleState");
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
        RTCLibWrapper::DateTimeValue dtv = _rtc.increment(_dp, _dtv);
        _rtc.changeTime(dtv);
    }

    void decrement()
    {
        RTCLibWrapper::DateTimeValue dtv = _rtc.decrement(_dp, _dtv);
        _rtc.changeTime(dtv);
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
        RTCLibWrapper::DateTimeValue dtv = _rtc.now();
        if (abs(_rtc.differenceInSeconds(dtv, _dtv)) >= 1)
        {
            displayTime(dtv);
        }
        if (abs(_rtc.differenceInSeconds(_displayeddtv, _dtv)) >= 60)
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
        char fmt[] = "DD/MM/YYYY hh:mm:ss";
        char *time = _rtc.toString(dtv, fmt);
        _disp->println(0, false, "%-19s", time);
        _dtv = dtv;
    }

    void displayHebrewDayMonth(const hdate hebrewDate)
    {
        const char *day_name = numtowday(hebrewDate, 1);

        char dayInMonth[3 * 2] = "";
        numtohchar(dayInMonth, sizeof(dayInMonth), hebrewDate.day);
        const char *monthName = numtohmonth(hebrewDate.month, hebrewDate.leap);

        const yomtov isNewMonth = getroshchodesh(hebrewDate);
        char isNewMonthIndicator[9 + 1] = "";
        const long sz_month=sizeof(isNewMonthIndicator);
        if (isNewMonth != CHOL)
        {
            const char *buf = PSTR(", ר\"ח"); // yomtovformat(isNewMonth);
            strncpy_P(isNewMonthIndicator, buf, sz_month);
        }
        else
        {
            strncpy(isNewMonthIndicator, "", sz_month);
        }
        
        _disp->println(1, true, "%s, %s %s %s", day_name, dayInMonth, monthName, isNewMonthIndicator);
    }

    void displayFestival(const hdate hebrewDate)
    {
        const yomtov yom_tov = getyomtov(hebrewDate);
        // yom_tov=CHOL_HAMOED_SUKKOS_DAY3;

        const size_t yom_tov_name_size = 39;
        char yom_tov_name[yom_tov_name_size] = "";

        if (yom_tov != CHOL)
        {
            const char *buf = yomtovformat(yom_tov);
            strncpy_P(yom_tov_name, buf, yom_tov_name_size);
        }

        _disp->println(2, true, "%s", yom_tov_name);
    }

    void displayOmer(const hdate hebrewDate)
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

    void displayHebDate(RTCLibWrapper::DateTimeValue dtv)
    {

        const float timezone = 2.0; //-4.0;
        const long int offset = (long int)3600 * timezone;

        const tm ltm = _rtc.convertToStdTime(dtv);
        hdate hebrewDate = convertDate(ltm);

        hebrewDate.EY = 1; // if it's in israel TODO: cofigure per location
        hebrewDate.offset = offset;
        displayHebrewDayMonth(hebrewDate);
        displayFestival(hebrewDate);
        displayOmer(hebrewDate);

        _displayeddtv = dtv;
    }

    const char *yomtovformat(yomtov current)
    {
        switch (current)
        {
        case CHOL:
            break;
        case PESACH_DAY1:
            return PSTR("פסח יו\"ט");
        case PESACH_DAY2:
            return PSTR("פסח יו\"ט");
        case SHVEI_SHEL_PESACH:
            return PSTR("שביעי של פסח");
        case ACHRON_SHEL_PESACH:
            return PSTR("אחרון של פסח");
        case SHAVOUS_DAY1:
            return PSTR("שבועות 1");
        case SHAVOUS_DAY2:
            return PSTR("2 שבועות");
        case ROSH_HASHANAH_DAY1:
            return PSTR("ראש השנה א'");
        case ROSH_HASHANAH_DAY2:
            return PSTR("ראש השנה ב'");
        case YOM_KIPPUR:
            return PSTR("יום כיפור");
        case SUKKOS_DAY1:
            return PSTR("חג סוכות 1'");
        case SUKKOS_DAY2:
            return PSTR("חג סוכות 2'");
        case SHMEINI_ATZERES:
            return PSTR("שמיני עצרת");
        case SIMCHAS_TORAH:
            return PSTR("שמחת תורה");
        case CHOL_HAMOED_PESACH_DAY1:
            return PSTR("פסח ב' )חוה\"מ(");
        case CHOL_HAMOED_PESACH_DAY2:
            return PSTR("פסח ג' )חוה\"מ(");
        case CHOL_HAMOED_PESACH_DAY3:
            return PSTR("פסח ד' )חוה\"מ(");
        case CHOL_HAMOED_PESACH_DAY4:
            return PSTR("פסח ה' )חוה\"מ(");
        case CHOL_HAMOED_PESACH_DAY5:
            return PSTR("פסח ו' )חוה\"מ(");

        case CHOL_HAMOED_SUKKOS_DAY1:
            return PSTR("חוה\"מ א' סוכות");
        case CHOL_HAMOED_SUKKOS_DAY2:
            return PSTR("חול המועד ב' סוכות");
        case CHOL_HAMOED_SUKKOS_DAY3:
            return PSTR("חול המועד ג' סוכות");
        case CHOL_HAMOED_SUKKOS_DAY4:
            return PSTR("חול המועד ד' סוכות");
        case CHOL_HAMOED_SUKKOS_DAY5:
            return PSTR("חול המועד סוכות");

        case HOSHANA_RABBAH:
            return PSTR("הושענא רבה");
        case PESACH_SHEINI:
            return PSTR("פסח שני");
        case LAG_BAOMER:
            return PSTR("ל\"ג בעומר");
        case TU_BAV:
            return PSTR("ט\"ו באב");
        case CHANUKAH_DAY1:
            return PSTR("חנוכה נר 1");
        case CHANUKAH_DAY2:
            return PSTR("חנוכה נר 2");
        case CHANUKAH_DAY3:
            return PSTR("חנוכה נר 3");
        case CHANUKAH_DAY4:
            return PSTR("חנוכה נר 4");
        case CHANUKAH_DAY5:
            return PSTR("חנוכה נר 5");
        case CHANUKAH_DAY6:
            return PSTR("חנוכה נר 6");
        case CHANUKAH_DAY7:
            return PSTR("חנוכה נר 7");
        case CHANUKAH_DAY8:
            return PSTR("חנוכה נר אחרון");
        case TU_BISHVAT:
            return PSTR("ט\"ו בשבט");
        case PURIM_KATAN:
            return PSTR("פורים קטן");
        case SHUSHAN_PURIM_KATAN:
            return PSTR("שושן פורים קטן");
        case PURIM:
            return PSTR("פורים");
        case SHUSHAN_PURIM:
            return PSTR("שושן פורים");
        case SHIVA_ASAR_BTAAMUZ:
            return PSTR("צום שבעה עשר בתמוז");
        case TISHA_BAV:
            return PSTR("צום תשעה באב");
        case TZOM_GEDALIA:
            return PSTR("צום גדליה");
        case ASARAH_BTEVES:
            return PSTR("צום עשרה בטבת");
        case TAANIS_ESTER:
            return PSTR("תענית אסתר");
        case EREV_PESACH:
            return PSTR("ערב פסח");
        case EREV_SHAVOUS:
            return PSTR("ערב שבועות");
        case EREV_ROSH_HASHANAH:
            return PSTR("ערב ראש השנה");
        case EREV_YOM_KIPPUR:
            return PSTR("ערב יום כיפור");
        case EREV_SUKKOS:
            return PSTR("ערב סוכות");
        case SHKALIM:
            return PSTR("שקלים");
        case ZACHOR:
            return PSTR("זכור");
        case PARAH:
            return PSTR("פרה");
        case HACHODESH:
            return PSTR("החודש");
        case ROSH_CHODESH:
            return PSTR("היום ר\"ח");
        case MACHAR_CHODESH:
            return PSTR("מחר ר\"ח");
        case SHABBOS_MEVORCHIM:
            return PSTR("שבת מברכים");
        }
        return "\0";
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
    RTCLibWrapper::DateTimeValue _dtv;
    RTCLibWrapper::DateTimeValue _displayeddtv;
    RTCLibWrapper _rtc;
    RTCLibWrapper::DatePart _dp = RTCLibWrapper::NONE;
    IDisplayWrapper *_disp;
    unsigned long _tickStamp = 0;
};

#endif