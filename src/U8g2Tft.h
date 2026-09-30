#ifndef U8G2_TFT_H
#define U8G2_TFT_H

// A U8g2 "display" that renders onto the 3.5" 480x320 SPI TFT (ILI9488, TFT_eSPI).
//
// U8g2 keeps drawing into its normal 256x128 1-bit full buffer, so the clock
// screen and the tcMenu U8g2 renderer produce exactly what the 2.7" OLED shows.
// On sendBuffer() every changed 8-pixel tile row is scaled by 1.875 (256x128 ->
// 480x240, smoothed edges) and pushed to the TFT. setContrast() drives the
// backlight and setPowerSave() switches it off/on.

#include <U8g2lib.h>

#define U8G2_TFT_WIDTH 256
#define U8G2_TFT_HEIGHT 128

void u8g2TftInvalidate();

extern "C" uint8_t u8x8_d_tft(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

class U8G2_TFT : public U8G2
{
public:
    explicit U8G2_TFT(const u8g2_cb_t *rotation) : U8G2()
    {
        u8g2_SetupDisplay(&u8g2, u8x8_d_tft, u8x8_cad_empty, u8x8_byte_empty, u8x8_dummy_cb);
        u8g2_SetupBuffer(&u8g2, _buf, U8G2_TFT_HEIGHT / 8, u8g2_ll_hvline_vertical_top_lsb, rotation);
    }

private:
    uint8_t _buf[U8G2_TFT_WIDTH * U8G2_TFT_HEIGHT / 8];
};

#endif
