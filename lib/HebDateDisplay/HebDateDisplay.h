#ifndef D71F0B9E_85BC_4D5F_80E1_CFDE9EBD7426
#define D71F0B9E_85BC_4D5F_80E1_CFDE9EBD7426
#include <hebrewcalendar.h>
// extern "C"
// {
// #include <hebrewcalendar.h>
// }
typedef struct
{
    char day_name[6 * 2];
    char dayInMonth[3 * 2];
    char monthName[10];
    char isNewMonthIndicator[9 + 1];
    char festivalName[40];
    char omer_count_name[20];
} HebDates;

typedef struct
{
    char dawn[6];
    char shma[6];
    char tefila[6];
    char sunrise[6];
    char sunset[6];
    char candleLight[6];
    char tzais[6];

} HebTimes;
void displayHebrewDayMonth(const hdate &hebrewDate, HebDates *hr);
void displayFestival(const hdate &hebrewDate, HebDates *hr);
void displayOmer(const hdate &hebrewDate, HebDates *hr);
void displayTimes(const hdate &hebrewDate, location here, HebTimes *hebTimes);
#endif /* D71F0B9E_85BC_4D5F_80E1_CFDE9EBD7426 */
