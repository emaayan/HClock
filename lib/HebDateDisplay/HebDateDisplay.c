#include <HebDateDisplay.h>

#include <hebrewcalendar.h>
#include <hdateformat.h>
#include <zmanim.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

hdate convertToHebDate(const struct tm tm, const bool isIsrael, const float tz)
{
    hdate hebrewDate = convertDate(tm);
    setEY(&hebrewDate, isIsrael);
    const long int offset = (long int)3600 * tz;
    hebrewDate.offset = offset;
    return hebrewDate;
}
void displayHebDates(const struct tm tm, const bool isIsrael, const float tz, struct HebDates *hr)
{
    const hdate hebrewDate = convertToHebDate(tm, isIsrael, tz);
    displayHebrewDates(&hebrewDate, hr);
}
void displayDayName(const hdate *hebrewDate, struct HebDates *hr)
{
    const char *day_name = numtowday(*hebrewDate, 1);
    strncpy(hr->day_name, day_name, sizeof(hr->day_name));
}

void displayHebDayMonth(const hdate * hebrewDate, struct HebDates *hr)
{
    numtohchar(hr->dayInMonth, sizeof(hr->dayInMonth), hebrewDate->day);
    const char *monthName = numtohmonth(hebrewDate->month, hebrewDate->leap);
    strncpy(hr->monthName, monthName, sizeof(hr->monthName));
}

void displayNewMonth(const hdate *hebrewDate, struct HebDates *hr)
{
    char isNewMonthIndicator[9 + 1] = "";
    const size_t sz_month = sizeof(isNewMonthIndicator);
    const yomtov shabat_mevarchim = getshabbosmevorchim(*hebrewDate);
    const yomtov isNewMonth = getroshchodesh(*hebrewDate);
    if (shabat_mevarchim != CHOL)
    {
        //molad molad = getmolad(hebrewDate.year, hebrewDate.month+1);                
        const char *buf = "מולד";
        strncpy(isNewMonthIndicator, buf, sz_month);
    }
    else if (isNewMonth != CHOL)
    {
        const char *buf = ",ר\"ח";
        strncpy(isNewMonthIndicator, buf, sz_month);
    }
    else
    {
        strncpy(isNewMonthIndicator, "", sz_month);
    }

    strncpy(hr->isNewMonthIndicator, isNewMonthIndicator, sizeof(hr->isNewMonthIndicator));
}
void displayHebrewDates(const hdate *hebrewDate, struct HebDates *hr)
{
    displayDayName(hebrewDate, hr);
    displayHebDayMonth(hebrewDate, hr);
    displayNewMonth(hebrewDate, hr);
    displayHebFestival(hebrewDate, hr);
    displayOmer(hebrewDate, hr);    
}

char *displayFestival_std(const yomtov yom_tov, char *buff, size_t szBuff)
{
    const char *yom_tov_name = yomtovformat(yom_tov);
    strncpy(buff, yom_tov_name, szBuff);
    return buff;
}

#ifndef STD_C
#include <hformat.h>
#endif

void displayHebFestival(const hdate *hebrewDate, struct HebDates *hr)
{
    const yomtov yom_tov = getyomtov(*hebrewDate);
    if (yom_tov != CHOL)
    {
#ifndef STD_C
        displayFestival(yom_tov, hr->festivalName, sizeof(hr->festivalName));
#else
        displayFestival_std(yom_tov, hr->festivalName, sizeof(hr->festivalName));
#endif
    }
}

void displayOmer(const hdate *hebrewDate, struct HebDates *hr)
{

    char omer_count_name[(9 * 2) + 1] = "";
    const long omer_count_size = sizeof(omer_count_name);

    const int omer_count = getomer(*hebrewDate);
    if (omer_count)
    {
        char omer_day[5 + 1] = "";
        numtohchar(omer_day, sizeof(omer_day), omer_count);
        snprintf(omer_count_name, omer_count_size, "%s בעומר", omer_day);
        strncpy(hr->omer_count_name, omer_count_name, sizeof(hr->omer_count_name));
    }
}

char *formattime(const hdate date, char *buff, size_t sz)
{
    time_t time = hdatetime_t(date);
    struct tm *tm = localtime(&time);
    strftime(buff, sz, "%H:%M", tm);
    return buff;
}

void displayTimes(const hdate *hDate, location here, struct HebTimes *hebTimes)
{
    const hdate hebrewDate = *hDate;
    formattime(getalosbaalhatanya(hebrewDate, here), hebTimes->dawn, sizeof(hebTimes->dawn));
    formattime(getshmabaalhatanya(hebrewDate, here), hebTimes->shma, sizeof(hebTimes->shma));
    formattime(gettefilabaalhatanya(hebrewDate, here), hebTimes->tefila, sizeof(hebTimes->tefila));
    formattime(getsunrise(hebrewDate, here), hebTimes->sunrise, sizeof(hebTimes->sunrise));
    formattime(getsunset(hebrewDate, here), hebTimes->sunset, sizeof(hebTimes->sunset));

    if (iscandlelighting(hebrewDate) == 1)
    {
        formattime(getcandlelighting(hebrewDate, here), hebTimes->candleLight, sizeof(hebTimes->candleLight));
    }

    if (iscandlelighting(hebrewDate) == 2)
    {
        formattime(gettzais8p5(hebrewDate, here), hebTimes->tzais, sizeof(hebTimes->tzais));
    }
    else if (isassurbemelachah(hebrewDate))
    {
        formattime(gettzais8p5(hebrewDate, here), hebTimes->tzais, sizeof(hebTimes->tzais));
    }
    else
    {
        formattime(gettzaisbaalhatanya(hebrewDate, here), hebTimes->tzais, sizeof(hebTimes->tzais));
    }
}
