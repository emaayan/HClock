#include <Arduino.h>
#include <hformat.h>


const char *p_yomtovformat(yomtov current)
{
    switch (current)
    {
    case CHOL:
        break;
    case PESACH_DAY1:
    case PESACH_DAY2:
        return PSTR("פסח");
    case SHVEI_SHEL_PESACH:
        return PSTR("שביעי של פסח");
    case ACHRON_SHEL_PESACH:
        return PSTR("אחרון של פסח");
    case SHAVOUS_DAY1:
    case SHAVOUS_DAY2:
        return PSTR("שבועות");
    case ROSH_HASHANAH_DAY1:
    case ROSH_HASHANAH_DAY2:
        return PSTR("ראש השנה");
    case YOM_KIPPUR:
        return PSTR("יום כיפור");
    case SUKKOS_DAY1:
    case SUKKOS_DAY2:
        return PSTR("סוכות");
    case SHMEINI_ATZERES:
        return PSTR("שמיני עצרת");
    case SIMCHAS_TORAH:
        return PSTR("שמחת תורה");
    case CHOL_HAMOED_PESACH_DAY1:
    case CHOL_HAMOED_PESACH_DAY2:
    case CHOL_HAMOED_PESACH_DAY3:
    case CHOL_HAMOED_PESACH_DAY4:
    case CHOL_HAMOED_PESACH_DAY5:
        return PSTR("חול המועד פסח");
    case CHOL_HAMOED_SUKKOS_DAY1:
    case CHOL_HAMOED_SUKKOS_DAY2:
    case CHOL_HAMOED_SUKKOS_DAY3:
    case CHOL_HAMOED_SUKKOS_DAY4:
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
    case CHANUKAH_DAY2:
    case CHANUKAH_DAY3:
    case CHANUKAH_DAY4:
    case CHANUKAH_DAY5:
    case CHANUKAH_DAY6:
    case CHANUKAH_DAY7:
    case CHANUKAH_DAY8:
        return PSTR("חנוכה");
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
        return PSTR("שבעה עשר בתמוז");
    case TISHA_BAV:
        return PSTR("תשעה באב");
    case TZOM_GEDALIA:
        return PSTR("צום גדליה");
    case ASARAH_BTEVES:
        return PSTR("עשרה בטבת");
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
        return PSTR("ראש חודש");
    case MACHAR_CHODESH:
        return PSTR("מחר חודש");
    case SHABBOS_MEVORCHIM:
        return PSTR("שבת מברכים");
    }
    return "\0";
}

char *displayFestival(const yomtov yom_tov, char *buff, size_t szBuff)
{
    const char *yom_tov_name = p_yomtovformat(yom_tov);
    strncpy_P(buff, yom_tov_name, szBuff);
    return buff;
}