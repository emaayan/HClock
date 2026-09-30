#if defined(DISPLAY_TFT_NATIVE)
#include "NativeFace.h"
extern "C"
{
#include <hdateformat.h> // numtohmonth
}
#include <TFT_eSPI.h>
#include "TftFonts.h"
#include "TftIcons.h"
#include <Preferences.h>

extern TFT_eSPI tft; // created in U8g2Tft.cpp (shared with the menu)

// ---------------------------------------------------------------- palette
static constexpr uint16_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
static const uint16_t C_BG = rgb(0x0B, 0x10, 0x20);
static const uint16_t C_PANEL = rgb(0x13, 0x1A, 0x31);
static const uint16_t C_LINE = rgb(0x22, 0x2B, 0x4A);
static const uint16_t C_PROGRESS = rgb(0x3B, 0x47, 0x75);
static const uint16_t C_TEXT = rgb(0xEE, 0xF1, 0xFA);
static const uint16_t C_DATE = rgb(0xC9, 0xD1, 0xEA);
static const uint16_t C_DIM = rgb(0x7C, 0x86, 0xA6);
static const uint16_t C_YOMTOV = rgb(0xFF, 0xD4, 0x46);
static const uint16_t C_HOLIDAY = rgb(0x4F, 0xA3, 0xCC);
static const uint16_t C_FAST = rgb(0xFD, 0x7E, 0x14);
static const uint16_t C_RC = rgb(0x9B, 0x6B, 0xF0);
static const uint16_t C_CANDLE = rgb(0xFF, 0xB5, 0x47);
static const uint16_t C_TAL = rgb(0x6F, 0xD3, 0xC1);
static const uint16_t C_BRACHA = rgb(0xA7, 0xD7, 0x7F);
static const uint16_t C_PLAIN = rgb(0xAE, 0xB8, 0xDA);

struct Zman
{
    const char *label;  // logical order Hebrew
    const char *label2; // optional second line
    const TftIcon *icon;
    uint16_t color;
};
static const Zman ZMANIM[8] = {
    {"עלות השחר", nullptr, &icon_dawn_20, rgb(0x8F, 0x95, 0xF2)},
    {"הנץ", nullptr, &icon_sunrise_20, rgb(0xFF, 0xB2, 0x7A)},
    {"סוף ק\"ש", nullptr, &icon_shema_20, rgb(0x7C, 0xC4, 0xFF)},
    {"סוף זמן תפילה", "(גר\"א)", &icon_tefila_20, rgb(0x7C, 0xC4, 0xFF)},
    {"חצות", nullptr, &icon_noon_20, rgb(0xFF, 0xD6, 0x6B)},
    {"פלג המנחה", nullptr, &icon_plag_20, rgb(0xFF, 0xA2, 0x4C)},
    {"שקיעה", nullptr, &icon_sunset_20, rgb(0xFF, 0x7A, 0x6B)},
    {"צאת הכוכבים", nullptr, &icon_stars_20, rgb(0xB7, 0x9C, 0xFF)},
};

// mix `a` over `b`, amount 0..255 of `a`
static uint16_t mix(uint16_t a, uint16_t b, uint8_t amount)
{
    return tft.alphaBlend(amount, a, b);
}

// ---------------------------------------------------------------- RTL text
// TFT_eSPI draws left to right, so Hebrew is reordered to visual order:
// runs of digits/latin stay as they are, everything else is reversed and the
// run order is flipped. Parentheses are mirrored.
static bool isLtr(uint32_t c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ':' || c == '/' ||
           c == '.' || c == 0xB0;
}

static size_t decode(const char *s, uint32_t *out, size_t max)
{
    size_t n = 0;
    const uint8_t *p = (const uint8_t *)s;
    while (*p && n < max)
    {
        uint32_t c = *p++;
        if (c >= 0xE0)
        {
            c = ((c & 0x0F) << 12) | ((p[0] & 0x3F) << 6) | (p[1] & 0x3F);
            p += 2;
        }
        else if (c >= 0xC0)
        {
            c = ((c & 0x1F) << 6) | (p[0] & 0x3F);
            p += 1;
        }
        out[n++] = c;
    }
    return n;
}

static void encode(const uint32_t *cp, size_t n, char *out, size_t max)
{
    size_t o = 0;
    for (size_t i = 0; i < n && o + 4 < max; i++)
    {
        const uint32_t c = cp[i];
        if (c < 0x80)
            out[o++] = c;
        else if (c < 0x800)
        {
            out[o++] = 0xC0 | (c >> 6);
            out[o++] = 0x80 | (c & 0x3F);
        }
        else
        {
            out[o++] = 0xE0 | (c >> 12);
            out[o++] = 0x80 | ((c >> 6) & 0x3F);
            out[o++] = 0x80 | (c & 0x3F);
        }
    }
    out[o] = 0;
}

static void toVisual(const char *logical, char *out, size_t max)
{
    uint32_t in[64], vis[64];
    const size_t n = decode(logical, in, 64);
    size_t v = 0;
    // walk runs from the end of the logical string to the start
    size_t end = n;
    while (end > 0)
    {
        size_t start = end;
        const bool ltr = isLtr(in[end - 1]);
        while (start > 0 && isLtr(in[start - 1]) == ltr)
            start--;
        if (ltr)
            for (size_t i = start; i < end; i++)
                vis[v++] = in[i];
        else
            for (size_t i = end; i > start; i--)
            {
                uint32_t c = in[i - 1];
                vis[v++] = c == '(' ? ')' : c == ')' ? '(' : c;
            }
        end = start;
    }
    encode(vis, v, out, max);
}

// ---------------------------------------------------------------- drawing target
// Everything is drawn through G at (x - OX, y - OY). Normally G is the screen;
// inside a Region it is an off-screen sprite covering that region, which is
// pushed to the screen in one go, so a redraw never shows a blank area.
static TFT_eSPI *G = &tft;
static int OX = 0, OY = 0;
static const uint8_t *_tftFont = nullptr; // smooth font loaded on the screen object
static const uint8_t *_sprFont = nullptr; // smooth font loaded on the current sprite

static void useFont(const uint8_t *font)
{
    G->setTextWrap(false, false); // wrapping would move overflowing glyphs to the start of the next line
    const uint8_t *&loaded = (G == &tft) ? _tftFont : _sprFont;
    if (loaded == font)
        return;
    if (loaded)
        G->unloadFont();
    G->loadFont(font);
    loaded = font;
}

class Region
{
    TFT_eSprite spr;
    int x, y;
    bool ok;

public:
    Region(int x_, int y_, int w, int h, uint16_t bg) : spr(&tft), x(x_), y(y_)
    {
        spr.setColorDepth(16);
        ok = spr.createSprite(w, h) != nullptr;
        if (ok)
        {
            spr.setTextWrap(false, false);
            spr.fillSprite(bg);
            G = &spr;
            OX = x;
            OY = y;
            _sprFont = nullptr;
        }
        else
        {
            tft.fillRect(x, y, w, h, bg); // not enough memory: draw directly (may flicker)
        }
    }
    ~Region()
    {
        if (!ok)
            return;
        if (_sprFont)
        {
            spr.unloadFont();
            _sprFont = nullptr;
        }
        spr.pushSprite(x, y);
        spr.deleteSprite();
        G = &tft;
        OX = OY = 0;
    }
};

static void gRound(int x, int y, int w, int h, int r, uint16_t c, uint16_t bg)
{
    G->fillSmoothRoundRect(x - OX, y - OY, w, h, r, c, bg);
}
static void gOutline(int x, int y, int r, int ir, int w, int h, uint16_t fg, uint16_t bg)
{
    G->drawSmoothRoundRect(x - OX, y - OY, r, ir, w, h, fg, bg);
}
static void gLine(float ax, float ay, float bx, float by, float wd, uint16_t c, uint16_t bg)
{
    G->drawWideLine(ax - OX, ay - OY, bx - OX, by - OY, wd, c, bg);
}
static void gDot(float x, float y, float r, uint16_t c, uint16_t bg)
{
    G->fillSmoothCircle(x - OX, y - OY, r, c, bg);
}

// Hebrew text, right edge at x, baseline at y. Returns the drawn width.
static int drawHe(const char *s, int x, int y, const uint8_t *font, uint16_t fg, uint16_t bg)
{
    char vis[200];
    toVisual(s, vis, sizeof(vis));
    useFont(font);
    G->setTextColor(fg, bg);
    G->setTextDatum(R_BASELINE);
    G->drawString(vis, x - OX, y - OY);
    return G->textWidth(vis);
}

static int widthHe(const char *s, const uint8_t *font)
{
    char vis[200];
    toVisual(s, vis, sizeof(vis));
    useFont(font);
    return G->textWidth(vis);
}

// Latin / digits, anchored by datum (L_BASELINE, C_BASELINE, R_BASELINE).
static int drawLtr(const char *s, int x, int y, const uint8_t *font, uint16_t fg, uint16_t bg, uint8_t datum)
{
    useFont(font);
    G->setTextColor(fg, bg);
    G->setTextDatum(datum);
    G->drawString(s, x - OX, y - OY);
    return G->textWidth(s);
}

static void drawIcon(const TftIcon &ic, int x, int y, uint16_t fg, uint16_t bg)
{
    if (G != &tft) // into a sprite: plain pixel writes to memory
    {
        for (int r = 0; r < ic.h; r++)
            for (int c = 0; c < ic.w; c++)
            {
                const uint8_t a = pgm_read_byte(&ic.alpha[r * ic.w + c]);
                if (a)
                    G->drawPixel(x - OX + c, y - OY + r, mix(fg, bg, a));
            }
        return;
    }
    uint16_t line[24];
    tft.startWrite();
    for (int r = 0; r < ic.h; r++)
    {
        for (int c = 0; c < ic.w; c++)
        {
            const uint8_t a = pgm_read_byte(&ic.alpha[r * ic.w + c]);
            const uint16_t px = a ? mix(fg, bg, a) : bg;
            line[c] = px; // setSwapBytes(true) is set at display init
        }
        tft.pushImage(x, y + r, ic.w, 1, line);
    }
    tft.endWrite();
}

static int minutesOf(const char *hhmm)
{
    int h, m;
    if (!hhmm || sscanf(hhmm, "%d:%d", &h, &m) != 2)
        return -1;
    return h * 60 + m;
}

// ---------------------------------------------------------------- band: festival / parasha + chip
struct Band
{
    char parts[4][48]; // festival, ראש חודש, omer / parasha, molad: shown joined with " · "
    int nparts;
    const TftIcon *icon;
    uint16_t color;
    char chip[40];
    const TftIcon *chipIcon;
    uint16_t chipColor;
};

static bool isSukkos(yomtov y)
{
    return y == SUKKOS_DAY1 || y == SUKKOS_DAY2 || (y >= CHOL_HAMOED_SUKKOS_DAY1 && y <= HOSHANA_RABBAH) ||
           y == SHMEINI_ATZERES || y == SIMCHAS_TORAH || y == EREV_SUKKOS;
}
static bool isYomTov(yomtov y)
{
    return (y >= PESACH_DAY1 && y <= SIMCHAS_TORAH);
}

static void buildBand(const hdate &hd, const HebDates &hr, const HebTimes &ht, const Scripture &scr, Band &b)
{
    const yomtov y = getyomtov(hd);
    const bool rc = getroshchodesh(hd) != CHOL;
    const bool fast = istaanis(hd);

    // text: every part that applies, e.g. "חנוכה · ראש חודש טבת (א׳)" or
    // "חול המועד פסח · ב׳ בעומר"; the week's parasha only when nothing else applies
    b.nparts = 0;
    auto add = [&](const char *s) {
        if (s && s[0] && b.nparts < 4)
            snprintf(b.parts[b.nparts++], sizeof(b.parts[0]), "%s", s);
    };
    const char *fest = hr.festivalName;
    add(fest);
    if (rc && !strstr(fest, "ראש חודש"))
    {
        // on the 30th, ראש חודש belongs to the next month; a two-day ראש חודש gets (א׳) / (ב׳)
        hdate next = hd, prev = hd;
        hdateaddday(&next, 1);
        hdateaddday(&prev, -1);
        const hdate &m = hd.day == 30 ? next : hd;
        const char *part = hd.day == 30 ? " (א׳)" : (prev.day == 30 ? " (ב׳)" : "");
        char s[48];
        snprintf(s, sizeof(s), "ראש חודש %s%s", numtohmonth(m.month, m.leap), part);
        add(s);
    }
    if (!strstr(fest, "בעומר"))
        add(hr.omer_count_name);
    if (b.nparts == 0 && strlen(scr.parasha))
    {
        char s[48];
        snprintf(s, sizeof(s), "פרשת %s", scr.parasha);
        add(s);
    }
    add(hr.molad); // שבת מברכים

    if (fast)
    {
        b.color = C_FAST;
        b.icon = &icon_fast_24;
    }
    else if (isYomTov(y))
    {
        b.color = C_YOMTOV;
        b.icon = isSukkos(y) ? &icon_sukkah_24 : &icon_candles_24;
    }
    else if (y != CHOL)
    {
        b.color = C_HOLIDAY;
        b.icon = isSukkos(y) ? &icon_sukkah_24 : (y >= CHANUKAH_DAY1 && y <= CHANUKAH_DAY8) ? &icon_candles_24 : &icon_shema_24;
    }
    else if (rc)
    {
        b.color = C_RC;
        b.icon = &icon_moon_24;
    }
    else
    {
        b.color = C_PLAIN;
        b.icon = &icon_shema_24;
    }

    // chip: the day's reminder: יעלה ויבוא, מחר ראש חודש, else the Amidah season
    // (candle lighting and צאת are shown large in the header instead)
    if (rc)
    {
        snprintf(b.chip, sizeof(b.chip), "יעלה ויבוא");
        b.chipIcon = &icon_moon_18;
        b.chipColor = C_RC;
    }
    else if (strstr(hr.isNewMonthIndicator, "מחר"))
    {
        snprintf(b.chip, sizeof(b.chip), "מחר ראש חודש");
        b.chipIcon = &icon_moon_18;
        b.chipColor = C_RC;
    }
    else
    {
        snprintf(b.chip, sizeof(b.chip), "%s", scr.season);
        const bool tal = strstr(scr.season, "טל") != nullptr;
        b.chipIcon = tal ? &icon_rain_18 : &icon_wheat_18;
        b.chipColor = tal ? C_TAL : C_BRACHA;
    }
}

static void drawBand(const Band &b)
{
    Region region(0, 92, 480, 42, C_BG);
    const uint16_t fill = mix(b.color, C_BG, 40);
    gRound(10, 96, 460, 34, 8, mix(b.color, C_BG, 140), C_BG);
    gRound(11, 97, 458, 32, 7, fill, mix(b.color, C_BG, 140));
    drawIcon(*b.icon, 440, 101, b.color, fill);

    const int cw = strlen(b.chip) ? widthHe(b.chip, font_small13) + 34 : 0;
    const int room = 432 - (cw ? 16 + cw + 12 : 20); // text runs right-to-left from x=432 up to the chip

    // join the parts; if too wide use the smaller font, then drop parts from the end
    char text[200] = "";
    const uint8_t *font = font_heb18;
    for (int n = b.nparts; n > 0; n--)
    {
        text[0] = 0;
        for (int i = 0; i < n; i++)
        {
            if (i)
                strlcat(text, " · ", sizeof(text));
            strlcat(text, b.parts[i], sizeof(text));
        }
        if (widthHe(text, font_heb18) <= room)
        {
            font = font_heb18;
            break;
        }
        if (widthHe(text, font_small13) <= room)
        {
            font = font_small13;
            break;
        }
    }
    drawHe(text, 432, font == font_heb18 ? 119 : 117, font, b.color, fill);

    if (cw)
    {
        const uint16_t chipFill = mix(b.chipColor, fill, 46);
        gRound(16, 101, cw, 24, 12, chipFill, fill);
        drawIcon(*b.chipIcon, 16 + cw - 24, 104, b.chipColor, chipFill);
        drawHe(b.chip, 16 + cw - 28, 118, font_small13, b.chipColor, chipFill);
    }
}

// ---------------------------------------------------------------- control bar
static const uint16_t C_BAR = rgb(0x14, 0x1B, 0x33);
static const uint16_t C_BTN = rgb(0x22, 0x2C, 0x52);
static const int BAR_Y = 226, BTN_Y = 248, BTN_H = 60;
static const uint32_t BAR_MS = 5000;
static bool _barShown = false;
static uint32_t _barUntil = 0;

struct BarButton
{
    int x, w;
    const TftIcon *icon;
    const char *label; // nullptr: icon only
    FaceAction action;
};
// right to left: menu, brighter, dimmer, and a small close button at the left end
static const BarButton BUTTONS[4] = {
    {348, 122, &icon_menu_24, "תפריט", FACE_MENU},
    {218, 122, &icon_brighter_24, "בהיר", FACE_BRIGHTER},
    {88, 122, &icon_dimmer_24, "עמום", FACE_DIMMER},
    {10, 70, &icon_close_24, nullptr, FACE_CLOSE},
};

static void drawBar()
{
    tft.fillRect(0, BAR_Y, 480, 320 - BAR_Y, C_BAR);
    tft.drawFastHLine(0, BAR_Y, 480, rgb(0x2B, 0x35, 0x60));
    tft.fillSmoothRoundRect(190, BAR_Y + 7, 100, 4, 2, C_PROGRESS, C_BAR);
    for (const BarButton &b : BUTTONS)
    {
        tft.fillSmoothRoundRect(b.x, BTN_Y, b.w, BTN_H, 12, C_BTN, C_BAR);
        if (b.label)
        {
            drawIcon(*b.icon, b.x + b.w - 34, BTN_Y + 18, C_TEXT, C_BTN);
            drawHe(b.label, b.x + b.w - 40, BTN_Y + 38, font_heb21, C_TEXT, C_BTN);
        }
        else
            drawIcon(*b.icon, b.x + (b.w - 24) / 2, BTN_Y + 18, C_TEXT, C_BTN);
    }
}

// ---------------------------------------------------------------- cache
static bool _valid = false;
static char _lastDate[12], _lastTime[8], _lastTemp[8], _lastEvent[48], _lastDay[24], _lastHdate[48], _lastBandKey[240];
static int _lastMinute = -1;
static char _cardKey[8][24]; // what each zman card shows now; a card is redrawn only when this changes

void nativeFaceInvalidate()
{
    _valid = false;
}

void nativeFaceRelease()
{
    if (_tftFont)
    {
        tft.unloadFont();
        _tftFont = nullptr;
    }
    _barShown = false;
}

bool nativeFaceBarVisible()
{
    return _barShown;
}

FaceAction nativeFaceTap(int x, int y)
{
    if (!_valid)
        return FACE_NONE;
    if (!_barShown)
    {
        _barShown = true;
        _barUntil = millis() + BAR_MS;
        drawBar();
        return FACE_NONE;
    }
    _barUntil = millis() + BAR_MS;
    FaceAction action = FACE_NONE;
    if (y < BAR_Y)
        action = FACE_CLOSE; // a tap on the face above the bar closes it
    else if (y >= BTN_Y && y < BTN_Y + BTN_H)
        for (const BarButton &b : BUTTONS)
            if (x >= b.x && x < b.x + b.w)
                action = b.action;
    if (action == FACE_CLOSE)
        _barUntil = millis(); // hidden on the next draw, which also repaints the cards
    return action;
}

void nativeTouchInit()
{
    // calibration from TFTTouchTest (NVS "tfttest"/"cal"); fallback: this panel's values
    uint16_t cal[5] = {269, 3644, 271, 3427, 7};
    Preferences prefs;
    if (prefs.begin("tfttest", true))
    {
        prefs.getBytes("cal", cal, sizeof(cal));
        prefs.end();
    }
    tft.setTouch(cal);
}

// ---------------------------------------------------------------- the face
bool nativeFaceDraw(const TMWrapper &tmw, float temp, const hdate &hd, const HebDates &hr, const HebTimes &ht,
                    const Scripture &scr)
{
    const tm t = tmw.get_tm();
    const int now = t.tm_hour * 60 + t.tm_min;

    if (_barShown && (int32_t)(millis() - _barUntil) >= 0)
    {
        _barShown = false;
        tft.fillRect(0, 226, 480, 94, C_BG); // clear the bar, then repaint the cards it covered
        for (auto &k : _cardKey)
            k[0] = 0;
    }

    if (!_valid)
    {
        tft.fillScreen(C_BG);
        _lastDate[0] = _lastTime[0] = _lastTemp[0] = _lastEvent[0] = _lastDay[0] = _lastHdate[0] = _lastBandKey[0] = 0;
        _lastMinute = -1;
        for (auto &k : _cardKey)
            k[0] = 0;
        _valid = true;
    }

    // ---- header, left: date, time, temperature (same order as the OLED) ----
    // candle lighting / end of Shabbat, Yom Tov or a fast, shown large under the time
    char event[48] = "";
    const TftIcon *eventIcon = &icon_candles_24;
    if (strlen(ht.candleLight))
        snprintf(event, sizeof(event), "הדלקת נרות %s", ht.candleLight);
    else if (strlen(ht.endFestival))
    {
        // libzmanim numbers the weekdays from Shabbat = 0
        const bool shabbat = hd.wday == 0, yomTov = isYomTov(getyomtov(hd));
        const char *what = istaanis(hd)          ? "צאת הצום"
                           : shabbat && yomTov ? "צאת שבת וחג"
                           : shabbat           ? "צאת שבת"
                                               : "צאת החג";
        snprintf(event, sizeof(event), "%s %s", what, ht.endFestival);
        eventIcon = &icon_havdala_24;
    }

    char date[12], time[8], tempStr[8];
    snprintf(date, sizeof(date), "%02d/%02d/%02d", t.tm_mday, t.tm_mon + 1, (t.tm_year + 1900) % 100);
    snprintf(time, sizeof(time), "%02d:%02d", t.tm_hour, t.tm_min);
    snprintf(tempStr, sizeof(tempStr), "%.0f°", temp);
    if (strcmp(date, _lastDate) || strcmp(time, _lastTime) || strcmp(tempStr, _lastTemp) || strcmp(event, _lastEvent))
    {
        Region region(0, 0, 350, 90, C_BG);
        int x = 14;
        // widest case (date 116 + time 129 + icon 20 + temperature 29 px) ends at 328 < 350
        x += drawLtr(date, x, 50, font_date30, C_DATE, C_BG, L_BASELINE) + 10;
        x += drawLtr(time, x, 54, font_time52, C_TEXT, C_BG, L_BASELINE) + 10;
        drawIcon(icon_therm_20, x, 32, C_DIM, C_BG);
        drawLtr(tempStr, x + 20, 50, font_heb21, C_DIM, C_BG, L_BASELINE);
        if (event[0]) // right-to-left: text from the left margin, icon after it on the right
        {
            const int w = widthHe(event, font_heb25);
            drawHe(event, 14 + w, 86, font_heb25, C_CANDLE, C_BG);
            drawIcon(*eventIcon, 14 + w + 8, 64, C_CANDLE, C_BG);
        }
        strcpy(_lastDate, date);
        strcpy(_lastTime, time);
        strcpy(_lastTemp, tempStr);
        strcpy(_lastEvent, event);
    }

    // ---- header, right: weekday and Hebrew date ----
    char day[24], hdateStr[48];
    snprintf(day, sizeof(day), "יום %s", hr.day_name);
    // no ", ר"ח" suffix here: it does not fit next to the time (up to 182 px); the band
    // marks ראש חודש and the chip shows "מחר ראש חודש" the day before
    snprintf(hdateStr, sizeof(hdateStr), "%s ב%s", hr.dayInMonth, hr.monthName);
    if (strcmp(day, _lastDay) || strcmp(hdateStr, _lastHdate))
    {
        Region region(350, 0, 130, 90, C_BG);
        const bool rc = getroshchodesh(hd) != CHOL;
        drawHe(day, 466, 30, font_heb25, C_TEXT, C_BG);
        const uint8_t *font = widthHe(hdateStr, font_heb21) <= 114 ? font_heb21 : font_heb18;
        drawHe(hdateStr, 466, 58, font, rc ? C_RC : C_TEXT, C_BG);
        char year[16] = "";
        numtohchar(year, sizeof(year) - 1, hd.year); // 5787 -> תשפ״ז
        drawHe(year, 466, 84, font_heb18, C_DIM, C_BG);
        strcpy(_lastDay, day);
        strcpy(_lastHdate, hdateStr);
    }

    // ---- event band ----
    Band band = {};
    buildBand(hd, hr, ht, scr, band);
    char bandKey[240];
    snprintf(bandKey, sizeof(bandKey), "%s|%s|%s|%s|%s|%u", band.parts[0], band.parts[1], band.parts[2], band.parts[3],
             band.chip, band.color);
    if (strcmp(bandKey, _lastBandKey))
    {
        drawBand(band);
        strcpy(_lastBandKey, bandKey);
    }

    // ---- timeline and zmanim cards: once a minute ----
    const char *times[8] = {ht.dawn, ht.sunrise, ht.shma, ht.tefila, ht.chatzos, ht.plug_hamincha, ht.sunset, ht.tzais};
    int mins[8];
    for (int i = 0; i < 8; i++)
        mins[i] = minutesOf(times[i]);

    int next = -1;
    for (int i = 0; i < 8; i++)
        if (mins[i] > now)
        {
            next = i;
            break;
        }

    // timeline: morning on the right, evening on the left; the marker moves once a minute
    if (now != _lastMinute)
    {
        _lastMinute = now;
        Region region(0, 136, 480, 18, C_BG);
        const int t0 = mins[0], t1 = mins[7];
        if (t0 >= 0 && t1 > t0)
        {
            auto X = [&](int m) { return 460 - (float)(m - t0) / (t1 - t0) * 440; };
            gLine(20, 146, 460, 146, 4, C_LINE, C_BG);
            const float nx = constrain(X(now), 20, 460);
            if (now > t0)
                gLine(nx, 146, 460, 146, 4, C_PROGRESS, C_BG);
            for (int i = 0; i < 8; i++)
                if (mins[i] >= 0)
                    gDot(X(mins[i]), 146, 4, ZMANIM[i].color, C_BG);
            gDot(nx, 146, 7, C_TEXT, C_BG);
            gDot(nx, 146, 5, C_BG, C_TEXT);
        }
    }

    // cards: 4 x 2, chronological, right to left. Each card is redrawn only when
    // its content changes (usually just the next zman's countdown, once a minute).
    // While the control bar is up they wait, and repaint when it hides.
    if (!_barShown)
        for (int i = 0; i < 8; i++)
        {
            const bool past = next == -1 || i < next;
            char cd[8] = "";
            if (i == next)
            {
                const int left = mins[next] - now;
                snprintf(cd, sizeof(cd), "%d:%02d", left / 60, left % 60);
            }
            char key[24];
            snprintf(key, sizeof(key), "%s|%c|%s", times[i], past ? 'p' : (i == next ? 'n' : 'f'), cd);
            if (strcmp(key, _cardKey[i]) == 0)
                continue;
            strcpy(_cardKey[i], key);

            const int col = i % 4, row = i / 4;
            const int x = 470 - (col + 1) * 115, y = 160 + row * 78, w = 110, h = 72;
            Region region(x, y - 6, w, h + 6, C_BG); // includes the badge strip above the card
            const uint8_t level = past ? 107 : 255;  // past zmanim at ~42%
            const uint16_t accent = mix(ZMANIM[i].color, C_BG, level);
            const uint16_t panel = mix(C_PANEL, C_BG, level);
            const uint16_t text = mix(C_TEXT, C_BG, level);
            gRound(x, y, w, h, 9, panel, C_BG);
            if (i == next)
                gOutline(x, y, 9, 7, w, h, accent, C_BG);
            drawIcon(*ZMANIM[i].icon, x + w - 28, y + 8, accent, panel);
            const bool two = ZMANIM[i].label2 != nullptr;
            // two-line labels sit higher so every card keeps its time on the same baseline
            drawHe(ZMANIM[i].label, x + w - 32, y + (two ? 19 : 25), font_small13, accent, panel);
            if (two)
                drawHe(ZMANIM[i].label2, x + w - 32, y + 33, font_small13, accent, panel);
            drawLtr(strlen(times[i]) ? times[i] : "--:--", x + w / 2, y + 60, font_card27, text, panel, C_BASELINE);
            if (i == next) // countdown badge on the card's top edge
            {
                gRound(x + 8, y - 6, 46, 14, 7, ZMANIM[i].color, C_BG);
                drawLtr(cd, x + 31, y + 5, font_small13, C_BG, ZMANIM[i].color, C_BASELINE);
            }
        }

    // night: after צאת הכוכבים or before עלות השחר
    return (mins[7] >= 0 && now >= mins[7]) || (mins[0] >= 0 && now < mins[0]);
}
#endif
