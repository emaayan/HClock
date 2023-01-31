#include <Arduino.h>
#include <Wire.h>
#include "IDisplayWrapper.h"
#include "DisplayWrapper.h"
#ifdef AVR_DEBUG
// #include <avr/io.h>
// #include <avr/interrupt.h>
#include <avr8-stub.h>
#include <app_api.h>
#endif
#include <Utils.h>
#include <NoiascaHW/lcd_PCF8574.h>

class LCDWrapper
{

public:
    void init()
    {
        _lcd.begin();
        _lcd.clear();
    }

    void lightUp()
    {
        _lcd.backlight();
    }
    void display()
    {
        _lcd.begin();
        _lcd.clear();
        _lcd.backlight();        
    }
    void println(uint8_t row, bool rtl, const char *fmt, ...)
    {
        size_t size = (rtl ? _cols * 2 : _cols) + 1;
        char buffer[size];
        va_list argptr;
        va_start(argptr, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, argptr);
        va_end(argptr);

        char fmtbuf[6];       
        snprintf(fmtbuf,sizeof(fmtbuf), "%%%ds", -1*(int)size);    
        debug("Test %s",fmtbuf);
        char buf[size];
        
        sprintf(buf, fmtbuf, buffer);
        display(buf, 0, row, rtl);
    }
    void display(const char *buffer, uint8_t col, uint8_t row, bool rtl)
    {
        if (rtl)
        {
            _lcd.rightToLeft();
            _lcd.setCursor(_cols - (col + 1), row);
        }
        else
        {
            _lcd.leftToRight();
            _lcd.setCursor(col, row);
        }
        _lcd.print(buffer);
    }

private:
    int _cols = 20;
    int _rows = 4;
    LiquidCrystal_PCF8574 _lcd=LiquidCrystal_PCF8574(0x27, 20, 4, convert_SPLC780D1_015A);
};

//LCDWrapper *_disp = new LCDWrapper();
IDisplayWrapper *_disp = new DisplayWrapper();
void setup()
{
#ifdef AVR_DEBUG
    debug_init();
#endif
    Wire.begin();
    _disp->init();
    _disp->lightUp();
}

void loop()
{
    delay(500);
    _disp->println(0, false, "%s%c", "13/04/2020 11:33:22", 'Y');
    delay(1000);
    _disp->println(1,true,"%s","שלום לך");
    delay(1000);
    _disp->println(1,true,"%s"," לך");
    
}