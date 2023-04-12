#ifndef D71F0B9E_85BC_4D5F_80E1_CFDE9EBD7426
#define D71F0B9E_85BC_4D5F_80E1_CFDE9EBD7426
#include <stdbool.h>
#include <hebrewcalendar.h>

struct HebDates
{    
    char day_name[10+1];
    char dayInMonth[6+1];
    char monthName[12+1];
    char isNewMonthIndicator[9 + 1];
    char festivalName[40];
    char omer_count_name[20];
} typedef HebDates;

struct  HebTimes
{
    char dawn[6];
    char shma[6];
    char tefila[6];
    char sunrise[6];
    char sunset[6];
    char candleLight[6];
    char tzais[6];

} typedef HebTimes ;
hdate convertToHebDate(const struct  tm, const bool isIsrael,const float tz);
void displayHebDates(const struct tm tm, const bool isIsrael, const float tz, struct HebDates *hr);
void displayHebrewDates(const hdate *hebrewDate,  struct HebDates *hr);
//void displayHebrewDayMonth(const hdate *hebrewDate, struct HebDates *hr);
void displayHebFestival(const hdate *hebrewDate,  struct HebDates *hr);
void displayOmer(const hdate *hebrewDate,  struct HebDates *hr);
void displayTimes(const hdate *hebrewDate, location here, struct HebTimes *hebTimes);

#endif /* D71F0B9E_85BC_4D5F_80E1_CFDE9EBD7426 */
