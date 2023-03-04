#include <DisplayWrapper.h>
#include <DebugUtils.h>

DisplayWrapper::DisplayWrapper(uint8_t address, uint8_t lcd_cols, uint8_t lcd_rows) : _lcd(address, lcd_cols, lcd_rows, convert_SPLC780D1_015A)
{
    _cols = lcd_cols;
    _rows = lcd_rows;
}

void DisplayWrapper::init()
{
    _lcd.init();    
    _lcd.clear();
}

void DisplayWrapper::lightUp()
{
    _lcd.backlight();

}

void DisplayWrapper::println(uint8_t row, bool rtl, const char *fmt, ...)
{    
    const size_t size =(rtl ? _cols * 2 : _cols) + 1;    
    char buffer[size]="";
     if (rtl){//must be done cause don't know how to pad spaces
        char blank[_cols]="";
        memset(blank,' ',sizeof(blank));
        display(blank, 0, row, rtl);
    }
    
    va_list argptr;
    va_start(argptr, fmt);    
    
    vsnprintf(buffer, sizeof(buffer), fmt, argptr);    
    va_end(argptr);      


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
