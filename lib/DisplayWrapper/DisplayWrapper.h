#ifndef DisplayWrapper_h
#define DisplayWrapper_h

#include "IDisplayWrapper.h"
#include <Wire.h>
#include <NoiascaHW/lcd_PCF8574.h>                         // I2C

class DisplayWrapper : public IDisplayWrapper
{
public:
    DisplayWrapper(uint8_t address=DEF_ADDRESS,uint8_t cols=DEF_COLS, uint8_t rows=DEF_COLS);
    void init();
    void lightUp();    
    void println(uint8_t row,bool rtl,const char *fmt, ...);
    void display(const char *buffer,uint8_t col=0, uint8_t row=0,bool rtl=false);
    void clear();

private:
   LiquidCrystal_PCF8574 _lcd;
   uint8_t _cols;
   uint8_t _rows;
};
#endif