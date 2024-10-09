#ifndef D5B23716_949F_4659_9C38_BE5BA7298569
#define D5B23716_949F_4659_9C38_BE5BA7298569

#include <ClockController.h>
#include <RTCLibWrapper.h>
#include <U8g2lib.h>

//#include <OledDisplayWrapper.h>
extern "C"
{
#include <HebDateDisplay.h>
}

#define CS 5
#define DC 17    // 16
#define RESET 16 // 4 // 17

U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI _disp(U8G2_R0, CS, DC, RESET); 
// OledDisplayWrapper _disp;
RTCLibWrapper _rtc = RTCLibWrapper();
const uint8_t COLOR_INDEX = 1; // WHITE
void init()
{
//    SPI.begin(SCK, MISO, MOSI, SS);
    _rtc.init();

    //    _disp.init();

    _disp.begin();
    _disp.setDrawColor(COLOR_INDEX);
    _disp.clearBuffer();
    _disp.setFont(u8g2_font_ncenB08_tr);
}

u8g2_uint_t writeUTF8(const uint8_t *font, const u8g2_uint_t line, const u8g2_uint_t x, const char *buff, bool rtl = false)
{
    _disp.setFont(font);
    //_disp.display(buff, x, line, rtl);
    return _disp.drawExtUTF8(x, line, rtl ? 1 : 0, NULL, buff);
}

u8g2_uint_t writeUTF8(const uint8_t *font, const u8g2_uint_t line, const char *buff, bool rtl = false)
{
    const u8g2_uint_t x = rtl ? 128 : 0;
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

static const uint8_t *_num_font = u8g2_font_5x7_tr;
static const uint8_t *_heb_font = u8g2_font_6x13_t_hebrew;
void writeTime(char *hebtime, const char *title, int x, int y)
{
    const size_t sz = (sizeof(title) / 2) - 1;
    writeUTF8(_heb_font, x, y, title, false);
    writeUTF8(_num_font, x, y - (sz * 25), hebtime);
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
        writeTime(ht.endFestival, ":צ", 35, 25);
    }

    writeUTF8(_heb_font, 45, hr.omer_count_name, true);

    writeTime(ht.minhca, ":ג\"מ", 47, 25);
    writeTime(ht.sunrise, ":ץנ", 60, 105);
    writeTime(ht.sunset, ":'קש", 60, 25);
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

    if (strnlen(scripture.parasha, sizeof(scripture.parasha) > 0))
    {
        writeUTF8(_heb_font, 48, scripture.parasha, true);
    }
    else
    {
        writeUTF8(_heb_font, 48, scripture.chumashbuf, true);
    }

    if (strnlen(scripture.avos, sizeof(scripture.avos) > 0))
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
void onPageLoop(const TMWrapper tmw, HebDates hr, HebTimes ht, Scripture scr)
{
    char dt[18] = "";
    tmw.toDateTimeString(dt, sizeof(dt));

    writeUTF8(_num_font, 15, dt);
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
}
void display()
{

    const TMWrapper tmw = _rtc.now();
    const tm tm = tmw.get_tm();
    if (abs(tmw.diff(_tmw)) > 60)
    {
        _tmw = tmw;
    }
    HebDates hr = {"", "", "", "", "", ""};
    const hdate hd = displayHebDates(tm, _isIsrael, _tz, &hr);
    HebTimes ht = {"", "", "", "", "", "", "", "", "", ""};
    displayTimes(&hd, _loc, &ht);
    Scripture scr = {"", "", "", "", ""};
    displayScripture(&hd, &scr);
    
    _disp.setDrawColor(COLOR_INDEX);
    _disp.firstPage();

    do
    {
        onPageLoop(tmw, hr, ht, scr);
    }

    while (_disp.nextPage());
}

#endif /* D5B23716_949F_4659_9C38_BE5BA7298569 */
