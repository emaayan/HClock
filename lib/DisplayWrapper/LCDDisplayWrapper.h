#ifndef DisplayWrapper_h
#define DisplayWrapper_h

#include <IDisplayWrapper.h>
#include <Wire.h>
// #include <NoiascaHW/lcd_PCF8574.h> // I2C
 #include <LiquidCrystal_I2C.h>

#define DEF_ADDRESS 0X27
#define DEF_COLS 20
#define DEF_ROWS 4

class LCDDisplayWrapper : public IDisplayWrapper
{
public:
    void init() override
    {
        _lcd.init();
        clear();
    };
    void lightUp() override
    {
        _lcd.backlight();
    };
    virtual size_t println(uint8_t row, bool rtl, const char *fmt, ...) override;
    size_t display(const char *buffer, uint8_t col = 0, uint8_t row = 0, bool rtl = false) override;
    void clear() override
    {
        _lcd.clear();
    };

private:
    void clearRow(uint8_t row, bool rtl)
    {
        const size_t size = _cols + 1;
        char blank[size] = "";
        memset(blank, ' ', sizeof(blank) - 1); //-1 must be for nul terminating string
        display(blank, 0, row, rtl);
    };
    const uint8_t _cols = DEF_COLS;
    const uint8_t _rows = DEF_ROWS;
   // LiquidCrystal_PCF8574 _lcd = LiquidCrystal_PCF8574(DEF_ADDRESS, _cols, _rows, convert_SPLC780D1_015A);
     LiquidCrystal_I2C _lcd=LiquidCrystal_I2C(DEF_ADDRESS,_cols,_rows);
};
#endif