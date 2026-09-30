#ifndef HCLOCK_SERIAL_CONSOLE_H
#define HCLOCK_SERIAL_CONSOLE_H

// Serial remote control for testing the clock and the tcMenu menu without
// touching the buttons (115200 baud, one command per line):
//
//   u / d        menu up / down         (same as the up/down buttons)
//   o            OK click               (same as a short press of OK)
//   h            OK hold                (long press: opens the menu from the clock)
//   + / -        brightness up / down   (the clock screen's long-press actions)
//   s            screenshot: dumps the U8g2 frame buffer (see below)
//   i            status: menu/clock, current menu item, contrast
//   p <x> <y>    tap the TFT clock face at x,y (native TFT build only)
//   T2026-09-27 17:40:00   set the RTC
//   ?            help
//
// Screenshot format, parsed by the PC side:
//   SCREENSHOT <width> <height>
//   <one line per 8-pixel page: width bytes as hex, bit 0 = top pixel>
//   END
// On the native TFT face only the menu passes through U8g2, so the clock face
// itself is checked with the camera instead.

#include <Arduino.h>

static char _conLine[48];
static uint8_t _conLen = 0;

static void consoleHelp()
{
    Serial.println(F("commands: u d o h + - s i p<x y> T<YYYY-MM-DD HH:MM:SS> ?"));
}

static void consoleScreenshot()
{
    const uint8_t *buf = _disp.getBufferPtr();
    const int tw = _disp.getBufferTileWidth(), th = _disp.getBufferTileHeight();
    const int w = tw * 8;
    Serial.printf("SCREENSHOT %d %d\n", w, th * 8);
    char hex[3];
    for (int page = 0; page < th; page++)
    {
        for (int x = 0; x < w; x++)
        {
            snprintf(hex, sizeof(hex), "%02X", buf[page * w + x]);
            Serial.print(hex);
        }
        Serial.println();
    }
    Serial.println(F("END"));
}

static void consoleStatus()
{
    MenuItem *cur = menuMgr.findCurrentActive(); // the highlighted item
    char name[24] = "";
    if (cur)
        cur->copyNameToBuffer(name, sizeof(name));
    const tm now = getTM();
    const hdate hd = convertToHebDate(now, _isIsrael, _tz);
    Serial.printf("mode=%s menu=\"%s\" contrast=%u time=%02d:%02d hdate=%d/%d/%d\n", inMenu ? "menu" : "clock", name,
                  _contrast, now.tm_hour, now.tm_min, hd.day, hd.month, hd.year);
}

static void consoleSetTime(const char *arg)
{
    int Y, M, D, h, m, s;
    if (sscanf(arg, "%d-%d-%d %d:%d:%d", &Y, &M, &D, &h, &m, &s) != 6)
    {
        Serial.println(F("usage: T2026-09-27 17:40:00"));
        return;
    }
    _rtc.changeTime(TMWrapper(Y - 1900, M - 1, D, h, m, s, false));
    Serial.printf("RTC set to %04d-%02d-%02d %02d:%02d:%02d\n", Y, M, D, h, m, s);
}

static void consoleCommand(const char *cmd)
{
    RotaryEncoder *enc = switches.getEncoder();
    switch (cmd[0])
    {
    case 'u':
        if (enc)
            enc->increment(-1);
        break;
    case 'd':
        if (enc)
            enc->increment(1);
        break;
    case 'o':
        menuMgr.onMenuSelect(false);
        break;
    case 'h':
        menuMgr.onMenuSelect(true);
        break;
    case '+':
        brightnessUp();
        break;
    case '-':
        brightnessDown();
        break;
    case 's':
        consoleScreenshot();
        return;
    case 'i':
        consoleStatus();
        return;
    case 'T':
        consoleSetTime(cmd + 1);
        return;
#if defined(DISPLAY_TFT_NATIVE)
    case 'p':
    {
        int x, y;
        if (sscanf(cmd + 1, "%d %d", &x, &y) == 2)
            handleFaceTap(x, y);
        else
            Serial.println(F("usage: p <x> <y>"));
        break;
    }
#endif
    case '?':
        consoleHelp();
        return;
    default:
        Serial.printf("unknown command '%s'\n", cmd);
        consoleHelp();
        return;
    }
    Serial.printf("ok %c\n", cmd[0]);
}

void consoleBegin()
{
    Serial.begin(115200);
    Serial.println(F("HClock serial console ready"));
    consoleHelp();
}

// Called from loop(): collects a line, then runs it.
void consolePoll()
{
    while (Serial.available())
    {
        const char c = Serial.read();
        if (c == '\r' || c == '\n')
        {
            if (_conLen)
            {
                _conLine[_conLen] = 0;
                consoleCommand(_conLine);
                _conLen = 0;
            }
        }
        else if (_conLen < sizeof(_conLine) - 1)
        {
            _conLine[_conLen++] = c;
        }
    }
}

#endif
