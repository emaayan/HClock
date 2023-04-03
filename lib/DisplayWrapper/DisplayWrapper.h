#ifndef DisplayWrapper_h
#define DisplayWrapper_h

#include <IDisplayWrapper.h>
#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
 #include <NoiascaHW/lcd_PCF8574.h> // I2C
#define DEF_ADDRESS 0X27
#define DEF_COLS 20
#define DEF_ROWS 4

class DisplayWrapper : public IDisplayWrapper
{
public:    
    void init() override;
    void lightUp() override;
    void println(uint8_t row, bool rtl, const char *fmt, ...) override;
    void display(const char *buffer, uint8_t col = 0, uint8_t row = 0, bool rtl = false) override;
    void clear() override;

private:
    void clearRow(uint8_t row,bool rtl);
    uint8_t _cols=DEF_COLS;
    uint8_t _rows=DEF_ROWS;
    LiquidCrystal_PCF8574 _lcd=LiquidCrystal_PCF8574(DEF_ADDRESS,_cols,_rows,convert_SPLC780D1_015A);    
    //LiquidCrystal_I2C _lcd=LiquidCrystal_I2C(DEF_ADDRESS,_cols,_rows);    
};
#endif