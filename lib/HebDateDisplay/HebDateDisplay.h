#ifndef D71F0B9E_85BC_4D5F_80E1_CFDE9EBD7426
#define D71F0B9E_85BC_4D5F_80E1_CFDE9EBD7426
#include <stdbool.h>
#include <hebrewcalendar.h>

struct HebDates
{
    char day_name[10 + 1];
    char dayInMonth[6 + 1];
    char monthName[12 + 1];
    char isNewMonthIndicator[16 + 1];
    char festivalName[40];
    char omer_count_name[20];
} typedef HebDates;

#define TIME_SZ 6
struct HebTimes
{
    char dawn[TIME_SZ];
    char shma[TIME_SZ];
    char tefila[TIME_SZ];
    char minhca[TIME_SZ];
    char sunrise[TIME_SZ];
    char sunset[TIME_SZ];
    char candleLight[TIME_SZ];
    char endFestival[TIME_SZ];
    char tzais[TIME_SZ];
    char chatzos[TIME_SZ];

} typedef HebTimes;
#define PARASHSA_SZ 40
struct Scripture
{
    char season[25];
    char parasha[PARASHSA_SZ];
    char avos[40];
    char chumashbuf[100];
    char tehillimbuf[100];

} typedef Scripture;
hdate convertToHebDate(const struct tm, const bool isIsrael, const float tz);
hdate displayHebDates(const struct tm tm, const bool isIsrael, const float tz, struct HebDates *hr);
void displayHebrewDates(const hdate *hebrewDate, struct HebDates *hr);
// void displayHebrewDayMonth(const hdate *hebrewDate, struct HebDates *hr);
void displayHebFestival(const hdate *hebrewDate, struct HebDates *hr);
void displayScripture(const hdate *hDate, struct Scripture *scripture);
void displayOmer(const hdate *hebrewDate, struct HebDates *hr);
void displayTimes(const hdate *hebrewDate, location here, struct HebTimes *hebTimes);

#endif /* D71F0B9E_85BC_4D5F_80E1_CFDE9EBD7426 */
