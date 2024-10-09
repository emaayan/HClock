 #ifndef F43C662F_B396_4A91_97E2_06EB4811F9E6
#define F43C662F_B396_4A91_97E2_06EB4811F9E6

#include <IDisplayWrapper.h>
#include <U8g2lib.h>

#define CS 5
#define DC 17                  // 16
#define RESET 16               // 4 // 17
const uint8_t COLOR_INDEX = 1; // WHITE
class OledDisplayWrapper : public IDisplayWrapper
{

public:
    void init() override
    {
        _disp.begin();
        _disp.setDrawColor(COLOR_INDEX);
        _disp.clearBuffer();
        _disp.setFont(u8g2_font_ncenB08_tr);
    };

    void lightUp() override{

    };
    void clear() override
    {
        _disp.clear();
    };
    void setFont(const uint8_t *font)
    {
        _disp.setFont(font);
    }

    void setDrawColor(const uint8_t color)
    {
        _disp.setDrawColor(color);
    }

    size_t println(const uint8_t row, const bool rtl, const char *fmt, ...)
    {
        const size_t sz = 50;
        char buffer[sz] = "";
        va_list argptr;
        va_start(argptr, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, argptr);
        va_end(argptr);

        _disp.clear();
        u8g2_uint_t t = _disp.drawStr(row, sz, buffer);
        _disp.sendBuffer();
        return t;
    }

    size_t display(const char *buffer, const uint8_t col, const uint8_t row, const bool rtl)
    {
        return _disp.drawExtUTF8(col, row, rtl ? 1 : 0, NULL, buffer);
    }
    u8g2_uint_t writeUTF8(const uint8_t *font, const u8g2_uint_t line, const u8g2_uint_t x, const char *buff, bool rtl = false)
    {
        _disp.setFont(font);
        //_disp.display(buff, x, line, rtl);
         return _disp.drawExtUTF8(x, line, rtl ? 1 : 0, NULL, buff);
    }

    // u8g2_uint_t writeUTF8(const uint8_t *font, const u8g2_uint_t line, const char *buff, bool rtl = false)
    // {
    //     const u8g2_uint_t x = rtl ? 128 : 0;
    //     _disp.setFont(font);
    //     _disp.display(buff, x, line, rtl);
    //     return writeUTF8(font, line, x, buff, rtl);
    // }

    template <typename DispCallback>
    size_t display(DispCallback callback)
    {
        do
        {
            callback();

        } while (_disp.nextPage());
    }

private:
    U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI _disp = U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI(U8G2_R0, CS, DC, RESET);
};

#endif /* F43C662F_B396_4A91_97E2_06EB4811F9E6 */