#ifndef D5B23716_949F_4659_9C38_BE5BA7298569
#define D5B23716_949F_4659_9C38_BE5BA7298569

#include <ClockController.h>
#include <RTCLibWrapper.h>
#include <U8g2lib.h>
#include <SettingsLib.hpp>

//#include <OledDisplayWrapper.h>
extern "C"
{
#include <HebDateDisplay.h>
}

#define CS 5
#define DC 17    // 16
#define RESET 16 // 4 // 17

// ---- Display selection (pick one at compile time via a build flag) ----
//   -D DISPLAY_SSD1363  -> 2.7"  256x128 OLED (SSD1363)
//   (no flag / default) -> 2.42" 128x64  OLED (SSD1309)
// Both are 4-wire HW SPI on the same CS/DC/RESET pins, so only the
// constructor and the logical screen dimensions change.
// NOTE: SSD1363 requires U8g2 >= 2.36.x.
#if defined(DISPLAY_SSD1363)
#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 128
#define DISPLAY_CONTRAST 255 // SSD1363 is 4-bit grayscale; low contrast reads as blank
#define CONTRAST_MIN 60      // floor so it can't be dimmed to invisible
U8G2_SSD1363_256X128_F_4W_HW_SPI _disp(U8G2_R0, CS, DC, RESET);
#else
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DISPLAY_CONTRAST 10
#define CONTRAST_MIN 1
U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI _disp(U8G2_R0, CS, DC, RESET);
#endif
#define CONTRAST_STEP 20
// OledDisplayWrapper _disp;
RTCLibWrapper _rtc = RTCLibWrapper();
const uint8_t COLOR_INDEX = 1; // WHITE

// ---- display brightness (setContrast), adjustable at runtime and persisted ----
SettingsLib _dispSettings("display");
uint8_t _contrast = DISPLAY_CONTRAST;

void applyBrightness()
{
    _disp.setContrast(_contrast);
}
void saveBrightness()
{
    _dispSettings.save("contrast", _contrast);
}
void initBrightness()
{
    _contrast = (uint8_t)_dispSettings.load("contrast", DISPLAY_CONTRAST);
    applyBrightness();
}
void brightnessUp()
{
    int v = _contrast + CONTRAST_STEP;
    _contrast = v > 255 ? 255 : v;
    applyBrightness();
    saveBrightness();
}
void brightnessDown()
{
    int v = _contrast - CONTRAST_STEP;
    _contrast = v < CONTRAST_MIN ? CONTRAST_MIN : v;
    applyBrightness();
    saveBrightness();
}

void init()
{
    _rtc.init();
    _disp.begin();
    _disp.setDrawColor(COLOR_INDEX);
    _disp.clearBuffer();
    _disp.setFont(u8g2_font_ncenB08_tr);
    initBrightness();
}

u8g2_uint_t writeUTF8(const uint8_t *font, const u8g2_uint_t line, const u8g2_uint_t x, const char *buff, bool rtl = false)
{
    _disp.setFont(font);
    //_disp.display(buff, x, line, rtl);
    return _disp.drawExtUTF8(x, line, rtl ? 1 : 0, NULL, buff);
}

u8g2_uint_t writeUTF8(const uint8_t *font, const u8g2_uint_t line, const char *buff, bool rtl = false)
{
    const u8g2_uint_t x = rtl ? SCREEN_WIDTH : 0;
    //_disp.setFont(font);
    //_disp.display(buff, x, line, rtl);
    return writeUTF8(font, line, x, buff, rtl);
}

uint16_t writeMessage(const char *fmt, ...)
{
    const size_t sz = 50;
    char buffer[sz] = "";
    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, argptr);
    va_end(argptr);

    _disp.clear();
    u8g2_uint_t t = _disp.drawStr(0, sz, buffer);
    _disp.sendBuffer();
    // size_t t = _disp.println(0, false, buffer);
    return t;
}

uint16_t writeMessage(u8g2_uint_t x, u8g2_uint_t y, const char *fmt, ...)
{
    const size_t sz = 50;
    char buffer[sz] = "";
    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, argptr);
    va_end(argptr);

    _disp.clear();
    u8g2_uint_t t = _disp.drawStr(x, y, buffer);
    _disp.sendBuffer();
    // size_t t = _disp.println(0, false, buffer);
    return t;
}

TMWrapper _tmw(0, 0, 0);
tm getTM()
{
    const TMWrapper tmw = _rtc.now();
    const tm tm = tmw.get_tm();
    return tm;
}

location _loc = {32.109333, 34.855499, 0};
float _tz = 0;
bool _isIsrael = true;
void setLocation(location loc, bool isIsrael)
{
    _loc = loc;
    _isIsrael = isIsrael;
}
void settz(const char *value)
{
    setenv("TZ", value, 1);
    tzset();
    _tz = 0;
    // setenv("TZ", "IST-2IDT,M3.4.4/26,M10.5.0", 1);
    // setenv("TZ", "IST-2", 1);
    // tzset();
}

#if defined(DISPLAY_SSD1363)
// Larger fonts for the 256x128 panel. Note: U8g2's biggest stock Hebrew font is
// unifont (~16px); there is no 2x Hebrew, so Hebrew tops out here.
static const uint8_t *_num_font = u8g2_font_9x15_tr;
static const uint8_t *_heb_font = u8g2_font_unifont_t_hebrew;
#else
static const uint8_t *_num_font = u8g2_font_5x7_tr;
static const uint8_t *_heb_font = u8g2_font_6x13_t_hebrew;
#endif
void writeTime(char *hebtime, const char *title, int x, int y)
{
    const size_t sz =  1;// (sizeof(title) / 2) - 1;
    //const size_t sz = strlen(title);  
    writeUTF8(_heb_font, x, y, title, false);
    writeUTF8(_num_font, x, y - (sz * 25), hebtime);
}

// One zmanim cell for the big screen, justified within a column: the value is
// left-aligned at leftX, the Hebrew label is right-aligned to rightX.
void writeZman(u8g2_uint_t leftX, u8g2_uint_t rightX, u8g2_uint_t y, const char *label, const char *value)
{
    writeUTF8(_num_font, y, leftX, value);
    _disp.setFont(_heb_font);
    const u8g2_uint_t lw = _disp.getUTF8Width(label);
    writeUTF8(_heb_font, y, rightX - lw, label);
}
uint8_t _screen_state = 0;
void setScreenState(uint8_t screen_state)
{

    if (screen_state == _screen_state)
    {
        _screen_state = 0;
    }
    else
    {
        _screen_state = screen_state;
    }
}
uint8_t getScreenState()
{
    return _screen_state;
}

void resetScreen()
{
    _screen_state = 0;
}

void mainScreen(const HebDates hr, HebTimes ht)
{
    writeUTF8(_heb_font, 35, hr.festivalName, true);

    if (strlen(ht.candleLight) > 0)
    {
        writeTime(ht.candleLight, ":נ\"ה", 35, 25);
    }   
    else if (strlen(ht.endFestival) > 0)
    {
        writeTime(ht.endFestival,  ":צומ", 35, 25);//  צאת שבת/חג
    }

    writeUTF8(_heb_font, 45, hr.omer_count_name, true);

    writeTime(ht.plug_hamincha, ":מ\"פ", 47, 25);
    writeTime(ht.sunrise, ":ץנ", 60, 105);
    writeTime(ht.tzais, ":כ\"צ", 60, 25); 
    // writeTime(ht.sunset, ":'קש", 60, 25);
}

void leftScreen(const HebDates hr, HebTimes ht)
{
    writeTime(ht.dawn, ":ש\"ע", 35, 25);
    writeTime(ht.shma, ":ש\"ק", 47, 25);
    writeTime(ht.tefila, ":ת", 60, 25);

    writeTime(ht.chatzos, ":ח", 60, 110);
    // writeTime(ht.sunset, ":ש", 60, 25);
}

void rightScreen(Scripture scripture)
{
    writeUTF8(_heb_font, 36, scripture.season, true);

    if (strnlen(scripture.parasha, sizeof(scripture.parasha)) > 0)
    {
        writeUTF8(_heb_font, 48, scripture.parasha, true);
    }
    else
    {
        writeUTF8(_heb_font, 48, scripture.chumashbuf, true);
    }

    if (strnlen(scripture.avos, sizeof(scripture.avos))> 0)
    {
        writeUTF8(_heb_font, 60, scripture.avos, true);
    }
    else
    {
        writeUTF8(_heb_font, 60, scripture.tehillimbuf, true);
    }
}
TMWrapper getNow()
{
    const TMWrapper tmw = _rtc.now();
    return tmw;
}
void setNow(TMWrapper tmw)
{
    _rtc.changeTime(tmw);
}
#if defined(DISPLAY_SSD1363)
// 256x128 layout: header (2 rows) + all 8 zmanim in two columns + a context
// line, so morning and evening times show together (no long-press needed).
void combinedScreen(const TMWrapper tmw, float temp, const HebDates hr, HebTimes ht, Scripture scr)
{
    // ---- header ----
    char dt[15] = "";
    tmw.toDateTimeString(dt, sizeof(dt));
    char l1[24] = "";
    snprintf(l1, sizeof(l1), "%s %.0fC", dt, temp);
    writeUTF8(_num_font, 13, 0, l1);             // date / time / temp, top-left
    writeUTF8(_heb_font, 13, hr.day_name, true); // weekday, top-right

    char dayMonth[52] = "";
    snprintf(dayMonth, sizeof(dayMonth), "%s ב%s %s", hr.dayInMonth, hr.monthName, hr.isNewMonthIndicator);
    writeUTF8(_heb_font, 31, dayMonth, true); // Hebrew date, row 2 (right)

    // ---- row 2 (left): molad on Shabbos Mevorchim -> "מולד <day>", left-justified ----
    if (strlen(hr.molad) > 0)
    {
        _disp.setFont(_heb_font);
        const u8g2_uint_t hw = _disp.getUTF8Width(hr.molad);
        writeUTF8(_heb_font, 31, hw, hr.molad, true); // RTL block, left edge at x=0
    }

    // ---- row 3 (left): seasonal Amidah insertion (ותן טל ומטר / ותן ברכה) ----
    if (strnlen(scr.season, sizeof(scr.season)) > 0)
    {
        _disp.setFont(_heb_font);
        const u8g2_uint_t sw = _disp.getUTF8Width(scr.season);
        writeUTF8(_heb_font, 49, sw, scr.season, true); // left-justified
    }

    // ---- row 3 (right): context line (festival, else omer, else parasha) ----
    if (strlen(hr.festivalName) > 0)
        writeUTF8(_heb_font, 49, hr.festivalName, true);
    else if (strlen(hr.omer_count_name) > 0)
        writeUTF8(_heb_font, 49, hr.omer_count_name, true);
    else if (strnlen(scr.parasha, sizeof(scr.parasha)) > 0)
        writeUTF8(_heb_font, 49, scr.parasha, true);

    // ---- zmanim: two symmetric columns (8px margins, 16px gutter), pushed to bottom ----
    // left column spans x[8..120], right column spans x[136..248]
    const u8g2_uint_t lLeft = 0, lRight = 112;
    const u8g2_uint_t rLeft = 136, rRight = 248;
    const u8g2_uint_t r1 = 68, r2 = 86, r3 = 104, r4 = 122;

    // left column: morning sequence
    // (labels are stored letter-reversed so LTR drawing renders correct RTL)
    writeZman(lLeft, lRight, r1, ":ש\"תולע", ht.dawn);   // עלות"ש
    writeZman(lLeft, lRight, r2, ":ש\"ק", ht.shma);       // ק"ש
    writeZman(lLeft, lRight, r3, ":הליפת", ht.tefila);    // תפילה
    writeZman(lLeft, lRight, r4, ":תוצח", ht.chatzos);    // חצות

    // right column: day / evening sequence
    if (strlen(ht.candleLight) > 0)
        writeZman(rLeft, rRight, r1, ":נ\"ה", ht.candleLight);
    else if (strlen(ht.endFestival) > 0)
        writeZman(rLeft, rRight, r1, ":צומ", ht.endFestival);
    writeZman(rLeft, rRight, r2, ":מ\"פ", ht.plug_hamincha);
    writeZman(rLeft, rRight, r3, ":ץנ", ht.sunrise);
    writeZman(rLeft, rRight, r4, ":כ\"צ", ht.tzais);
}
#endif

void onPageLoop(const TMWrapper tmw,float temp, HebDates hr, HebTimes ht, Scripture scr)
{
#if defined(DISPLAY_SSD1363)
    // Big screen shows everything at once; long-press states are unused here.
    combinedScreen(tmw, temp, hr, ht, scr);
#else
    char dt[15] = "";
    tmw.toDateTimeString(dt, sizeof(dt));
    char l1[22] = "";
    snprintf(l1, sizeof(l1), "%s %.0fC",dt, temp);

    writeUTF8(_num_font, 15, l1);
    writeUTF8(_heb_font, 15, hr.day_name, true);

    char dayMonth[50 + 1] = "";
    snprintf(dayMonth, sizeof(dayMonth), "%s ב%s %s", hr.dayInMonth, hr.monthName, hr.isNewMonthIndicator);

    writeUTF8(_heb_font, 25, dayMonth, true);

    switch (_screen_state)
    {
    case 0:
        mainScreen(hr, ht);
        break;
    case 1:
        leftScreen(hr, ht);
        break;
    case 2:
        rightScreen(scr);
        break;
    default:
        break;
    }
#endif
}

void display()
{

    const TMWrapper tmw = _rtc.now();
    const tm tm = tmw.get_tm();
    long diff=abs(tmw.diff(_tmw));
    if (diff > 60)
    {
        _tmw = tmw;
    }
    
   // if(diff>2)//2 seconds to have responsive for screen changes
    {
        HebDates hr = {"", "", "", "", "", ""};
        const hdate hd = displayHebDates(tm, _isIsrael, _tz, &hr);
        HebTimes ht = {"", "", "", "", "", "", "", "", "", ""};
        displayTimes(&hd, _loc, &ht);
        Scripture scr = {"", "", "", "", ""};
        displayScripture(&hd, &scr);
        
        const float temp=_rtc.getTemperature();
        _disp.setDrawColor(COLOR_INDEX);         
        _disp.firstPage();
        do
        {
            onPageLoop(tmw,temp, hr, ht, scr);
        }
        while (_disp.nextPage());
    }
}

#endif /* D5B23716_949F_4659_9C38_BE5BA7298569 */
