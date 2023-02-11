#include <Arduino.h>
#include <Wire.h>

#ifdef DEBUG_AVR
#include <avr8-stub.h>
#include <app_api.h>
#endif

#include <DebugUtils.h>
#include <NoiascaHW/lcd_PCF8574.h>

// LCDWrapper *_disp = new LCDWrapper();
// DisplayWrapper _disp;
int _cols = 20;
int _rows = 4;
LiquidCrystal_PCF8574 _lcd(0x027, _cols, _rows, convert_SPLC780D1_015A);
void setup()
{
#ifdef DEBUG_AVR
    debug_init();
#endif
#ifdef DEBUG_CON
    Serial.begin(115200);
#endif
    Wire.begin();

    _lcd.init();
    _lcd.clear();
    _lcd.backlight();

    // _disp.init();
    // _disp.lightUp();
}


size_t count_chars(const char *s)
{
    size_t len = 0;
    for (; *s; ++s)
        if ((*s & 0xC0) != 0x80)
            ++len;
    return len;
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
void println(uint8_t row, bool rtl, const char *fmt, ...)
{
    const size_t size = (_cols * 2) + 1; // (rtl ? _cols * 2 : _cols) + 1;
    char buffer[size] = "";
    if (rtl){
        memset(buffer,' ',size-1);
        display(buffer, 0, row, rtl);
    }
    va_list argptr;
    va_start(argptr, fmt);

    vsnprintf(buffer, sizeof(buffer), fmt, argptr);
    va_end(argptr);
    //   debug_message(buffer);
    ///size_t count = count_chars(buffer);

 //   memset(buffer + count, ' ', _cols - count);
    debug("' %s '", buffer);
    
     display(buffer, 0, row, rtl);
}



void loop()
{

    int i = millis();

    delay(1000);

    _lcd.clear();

    println(0, false, "%-10s %d", "01/02/2002 1 ", i);
    println(1, false, "%-20s", "this is a test");    
    delay(1000);
    println(1, false, "%-20s", "bingo so");

    delay(1000);
    println(2, true, "%-20s", "חולון המועד סוכות");
        
    delay(1000);
     println(2, true, "%-20s", "לג בעומר");
    
    // _lcd.clear();
};