#include <HebDateDisplay.h>

#include <hebrewcalendar.h>
#include <hdateformat.h>
#include <zmanim.h>
#include <shuir.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

hdate convertToHebDate(const struct tm ltm, const bool isIsrael, const float tz)
{
    struct tm tm = ltm;
    hdate hebrewDate = convertDate(tm);
    setEY(&hebrewDate, isIsrael);
    const long int offset = (long int)3600 * tz;
    hebrewDate.offset = offset;
    return hebrewDate;
}
hdate displayHebDates(const struct tm tm, const bool isIsrael, const float tz, struct HebDates *hr)
{
    const hdate hebrewDate = convertToHebDate(tm, isIsrael, tz);
    displayHebrewDates(&hebrewDate, hr);
    return hebrewDate;
}
void displayDayName(const hdate *hebrewDate, struct HebDates *hr)
{
    const char *day_name = numtowday(*hebrewDate, 1);
    strncpy(hr->day_name, day_name, sizeof(hr->day_name));
}

void displayHebDayMonth(const hdate *hebrewDate, struct HebDates *hr)
{
    numtohchar(hr->dayInMonth, sizeof(hr->dayInMonth), hebrewDate->day);
    const char *monthName = numtohmonth(hebrewDate->month, hebrewDate->leap);
    strncpy(hr->monthName, monthName, sizeof(hr->monthName));
}

bool isMacharRoshChodesh(const hdate *hebrewDate)
{
    return hebrewDate->day == 29 && hebrewDate->month != 6;
}

void displayNewMonth(const hdate *hebrewDate, struct HebDates *hr)
{
    char isNewMonthIndicator[16 + 1] = "";
    const size_t sz_month = sizeof(isNewMonthIndicator);
    // Suppress "מחר ר"ח" on Shabbos Mevorchim - the molad block already
    // announces the Rosh Chodesh day(s) there, so the marker is redundant.
    if (isMacharRoshChodesh(hebrewDate) && getshabbosmevorchim(*hebrewDate) != SHABBOS_MEVORCHIM)
    {
        strncpy(isNewMonthIndicator, ",מחר ר\"ח", sz_month);
    }
    else if (getroshchodesh(*hebrewDate) == ROSH_CHODESH)
    {
        strncpy(isNewMonthIndicator, ",ר\"ח", sz_month);
    }
    // Shabbos Mevorchim is announced by displayMolad() (the "מולד <day>" line),
    // so no redundant marker is added on the Hebrew-date line here.
    else
    {
        strncpy(isNewMonthIndicator, "", sz_month);
    }

    strncpy(hr->isNewMonthIndicator, isNewMonthIndicator, sizeof(hr->isNewMonthIndicator));
}
void displayMolad(const hdate *hebrewDate, struct HebDates *hr)
{
    // Only announced on Shabbos Mevorchim (the Shabbos before Rosh Chodesh).
    if (getshabbosmevorchim(*hebrewDate) != SHABBOS_MEVORCHIM)
    {
        return;
    }
    // Announce which day(s) the new month falls on. Rosh Chodesh is 2 days when
    // the outgoing month has 30 days (its 30th is day 1 and the new month's 1st
    // is day 2), otherwise 1 day.
    const int monthLen = LastDayOfHebrewMonth(hebrewDate->month, hebrewDate->year);
    const int daysToFirst = monthLen - hebrewDate->day + 1; // from this Shabbos to the 1st
    hdate rc1 = {0};
    rc1.wday = (hebrewDate->wday + daysToFirst) % 7;
    if (monthLen == 30)
    {
        hdate rc0 = {0};
        rc0.wday = (rc1.wday + 6) % 7; // the day before (the 30th)
        snprintf(hr->molad, sizeof(hr->molad), "מולד %s ו%s", numtowday(rc0, 1), numtowday(rc1, 1));
    }
    else
    {
        snprintf(hr->molad, sizeof(hr->molad), "מולד %s", numtowday(rc1, 1));
    }
}

void displayHebrewDates(const hdate *hebrewDate, struct HebDates *hr)
{
    displayDayName(hebrewDate, hr);
    displayHebDayMonth(hebrewDate, hr);
    displayNewMonth(hebrewDate, hr);
    displayHebFestival(hebrewDate, hr);
    displayOmer(hebrewDate, hr);
    displayMolad(hebrewDate, hr);
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
    // snprintf(buff,sz,"%d:%d",date.hour,date.min);
    return buff;
}

parshah get_parahsa_name(const hdate *hDate)
{
    parshah par = getparshah(*hDate);
    if (!par)
    {
        hdate shabbos = *hDate;
        hdateaddday(&shabbos, (7 - hDate->wday));
        par = getparshah(shabbos);
    }
    return par;
}

void displayScripture(const hdate *hDate, struct Scripture *scripture)
{
    if (getbirchashashanim(*hDate))
    {
        strncpy(scripture->season, "ותן טל ומטר", sizeof(scripture->season));
    }
    else
    {
        strncpy(scripture->season, "ותן ברכה", sizeof(scripture->season));
    }

    const size_t parasha_sz = sizeof(scripture->parasha);
    char par_name[PARASHSA_SZ] = "";
    parshah par = get_parahsa_name(hDate);
    if (par)
    {
        strncpy(par_name, parshahformat(par), parasha_sz);
    }

    const int avos = getavos(*hDate);
    if ((avos))
    {
        const char* avos_str=avosformat(avos);        
        snprintf(scripture->avos, sizeof(scripture->avos), "%-15.15s %s","אבות פרק",avos_str);
    }
    chumash(*hDate,scripture->chumashbuf);
    tehillim(*hDate,scripture->tehillimbuf);//removed \n from source
}

void displayTimes(const hdate *hDate, location here, struct HebTimes *hebTimes)
{
    const hdate hebrewDate = *hDate;
    formattime(getalosbaalhatanya(hebrewDate, here), hebTimes->dawn, sizeof(hebTimes->dawn));
    formattime(getshmabaalhatanya(hebrewDate, here), hebTimes->shma, sizeof(hebTimes->shma));
    formattime(gettefilabaalhatanya(hebrewDate, here), hebTimes->tefila, sizeof(hebTimes->tefila));
    formattime(getsunrise(hebrewDate, here), hebTimes->sunrise, sizeof(hebTimes->sunrise));
    formattime(getsunset(hebrewDate, here), hebTimes->sunset, sizeof(hebTimes->sunset));
    formattime(getminchagedolabaalhatanya(hebrewDate, here), hebTimes->minhca, sizeof(hebTimes->minhca));
    formattime(getchatzosbaalhatanya(hebrewDate, here), hebTimes->chatzos, sizeof(hebTimes->chatzos));
    formattime(getplagbaalhatanya(hebrewDate, here), hebTimes->plug_hamincha, sizeof(hebTimes->plug_hamincha));

    hdate tzais;

    const int candleType = iscandlelighting(hebrewDate);
    switch (candleType)
    {
    case 1: // SHABAT_ENTRY    
        formattime(getcandlelighting(hebrewDate, here), hebTimes->candleLight, sizeof(hebTimes->candleLight));
        tzais=gettzaisbaalhatanya(hebrewDate, here);
        break;
    case 2: // FESTIVAL
        if (!isassurbemelachah(hebrewDate))
        {
            formattime(getcandlelighting(hebrewDate, here), hebTimes->candleLight, sizeof(hebTimes->candleLight));
        }
        tzais=gettzais8p5(hebrewDate, here);
        break;
    case 3://hanuka
        if (!isassurbemelachah(hebrewDate))
        {
            tzais=gettzaisbaalhatanya(hebrewDate, here);    
        }
        else
        {
            tzais=gettzais8p5(hebrewDate, here);    
        }
        formattime(gettzaisbaalhatanya(hebrewDate, here), hebTimes->candleLight, sizeof(hebTimes->candleLight));        
        break;
    default: //REGULAR days
        tzais=gettzaisbaalhatanya(hebrewDate, here);    
        break;
    }

    formattime(tzais, hebTimes->tzais, sizeof(hebTimes->tzais));
    if (isassurbemelachah(hebrewDate))
    {
       formattime(gettzais8p5(hebrewDate, here), hebTimes->endFestival, sizeof(hebTimes->endFestival));
    }
}
