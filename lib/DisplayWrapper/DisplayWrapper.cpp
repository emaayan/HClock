#include "DisplayWrapper.h"
#include <Utils.h>

DisplayWrapper::DisplayWrapper(uint8_t address, uint8_t lcd_cols, uint8_t lcd_rows) : _lcd(address, lcd_cols, lcd_rows, convert_SPLC780D1_015A)
{
    _cols = lcd_cols;
    _rows = lcd_rows;
}

void DisplayWrapper::init()
{
    _lcd.begin();
    _lcd.clear();
}

void DisplayWrapper::lightUp()
{
    _lcd.backlight();
}

void DisplayWrapper::println(uint8_t row, bool rtl, const char *fmt, ...)
{
    size_t size = (rtl ? _cols * 2 : _cols) + 1;
    char buffer[size];
    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, argptr);
    va_end(argptr);
    char buf[size];

    // if (rtl)
    // {
    //     char fmtbuf[7] = "%-30s";        
    //     snprintf(buf, sizeof(buf), fmtbuf, buffer);
    //     display(buf, 0, row, rtl);
    // }
    
    // else
    // {
    //     char fmtbuf[7] = "%-25s";        
    //     snprintf(buf, sizeof(buf), fmtbuf, buffer);
    //     display(buf, 0, row, rtl);
    // }
    
    //  snprintf(fmtbuf,sizeof(fmtbuf), "%%%ds", -1*(int)_cols);
    
    // debug("Format buffer %s", fmtbuf);
    // debug("buf %s", buf);

    // debug("Buffer %s", buffer);

    display(buffer, 0, row, rtl);
}

void DisplayWrapper::display(const char *buffer, uint8_t col, uint8_t row, bool rtl)
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
void DisplayWrapper::clear()
{
    _lcd.clear();
}
