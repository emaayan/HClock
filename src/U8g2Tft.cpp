#if defined(DISPLAY_TFT)
#include "U8g2Tft.h"
#include <TFT_eSPI.h>

// Pins and panel type come from build flags (see [env:esp32_tft]).
#ifndef PIN_BL
#define PIN_BL 32
#endif
#ifndef TFT_ROTATION
#define TFT_ROTATION 1 // 1 = landscape; 3 = landscape turned 180° (USB at the top in the enclosure)
#endif
#ifndef TFT_FG_COLOR
#define TFT_FG_COLOR 0x5DDF // light blue, close to the OLED's text colour
#endif

#if defined(DISPLAY_TFT_NATIVE)
extern TFT_eSPI tft; // created by generated/HClockTFT_menu.cpp
#else
TFT_eSPI tft;
#endif

static const int SRC_W = U8G2_TFT_WIDTH, SRC_H = U8G2_TFT_HEIGHT;
static const int DST_W = 480, DST_H = 240;       // 1.875x in both directions
static const int DST_Y0 = (320 - DST_H) / 2;     // centred vertically
static const int BL_CHANNEL = 0;

static const u8x8_display_info_t tft_display_info = {
    /* chip_enable_level = */ 0,
    /* chip_disable_level = */ 1,
    /* post_chip_enable_wait_ns = */ 0,
    /* pre_chip_disable_wait_ns = */ 0,
    /* reset_pulse_width_ms = */ 0,
    /* post_reset_wait_ms = */ 0,
    /* sda_setup_time_ns = */ 0,
    /* sck_pulse_width_ns = */ 0,
    /* sck_clock_hz = */ 0,
    /* spi_mode = */ 0,
    /* i2c_bus_clock_100kHz = */ 0,
    /* data_setup_time_ns = */ 0,
    /* write_pulse_width_ns = */ 0,
    /* tile_width = */ SRC_W / 8,
    /* tile_height = */ SRC_H / 8,
    /* default_x_offset = */ 0,
    /* flipmode_x_offset = */ 0,
    /* pixel_width = */ SRC_W,
    /* pixel_height = */ SRC_H};

// Copy of what is currently on the TFT, in U8g2 tile layout (byte = 8 vertical pixels).
static uint8_t shadow[SRC_W * SRC_H / 8];

// Each destination pixel covers 8/15 of a source pixel, so it overlaps source
// pixel s0 by w0/8 and s0+1 by (8-w0)/8. Same tables for x and y.
static uint8_t srcX[DST_W], wX[DST_W];
static uint8_t srcY[DST_H], wY[DST_H];
static uint16_t shade[65]; // background..foreground blend for coverage 0..64
static uint16_t line[DST_W];
static uint8_t backlight = 255;
static bool powerSave = false;

static void buildTables()
{
    for (int d = 0; d < DST_W; d++)
    {
        const int s0 = (8 * d) / 15;
        const int end0 = 15 * s0 + 15;
        srcX[d] = s0;
        wX[d] = (8 * d + 8 <= end0) ? 8 : end0 - 8 * d;
    }
    for (int d = 0; d < DST_H; d++)
    {
        const int s0 = (8 * d) / 15;
        const int end0 = 15 * s0 + 15;
        srcY[d] = s0;
        wY[d] = (8 * d + 8 <= end0) ? 8 : end0 - 8 * d;
    }
    const uint16_t fg = TFT_FG_COLOR;
    const int fr = fg >> 11, fgc = (fg >> 5) & 0x3F, fb = fg & 0x1F;
    for (int c = 0; c <= 64; c++)
        shade[c] = ((fr * c / 64) << 11) | ((fgc * c / 64) << 5) | (fb * c / 64);
}

static inline int px(int x, int y)
{
    if (x >= SRC_W || y >= SRC_H)
        return 0;
    return (shadow[(y >> 3) * SRC_W + x] >> (y & 7)) & 1;
}

static void applyBacklight()
{
    const uint8_t v = powerSave ? 0 : backlight;
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWrite(PIN_BL, v);
#else
    ledcWrite(BL_CHANNEL, v);
#endif
}

// Redraw the destination lines affected by source tile row `ty`, columns [tx0..tx1].
static void blitTileRow(int ty, int tx0, int tx1)
{
    const int sx0 = tx0 * 8, sx1 = tx1 * 8 + 7;
    int dx0 = (sx0 * 15) / 8 - 1, dx1 = ((sx1 + 1) * 15) / 8 + 1;
    if (dx0 < 0)
        dx0 = 0;
    if (dx1 > DST_W - 1)
        dx1 = DST_W - 1;
    const int w = dx1 - dx0 + 1;

    tft.startWrite();
    for (int dy = 0; dy < DST_H; dy++)
    {
        const int sy0 = srcY[dy], wy0 = wY[dy], wy1 = 8 - wy0;
        const bool touches = (sy0 >> 3) == ty || (wy1 && ((sy0 + 1) >> 3) == ty);
        if (!touches)
            continue;
        for (int dx = dx0; dx <= dx1; dx++)
        {
            const int sx = srcX[dx], wx0 = wX[dx], wx1 = 8 - wx0;
            int cov = wx0 * wy0 * px(sx, sy0);
            if (wx1)
                cov += wx1 * wy0 * px(sx + 1, sy0);
            if (wy1)
            {
                cov += wx0 * wy1 * px(sx, sy0 + 1);
                if (wx1)
                    cov += wx1 * wy1 * px(sx + 1, sy0 + 1);
            }
            line[dx - dx0] = shade[cov];
        }
        tft.pushImage(dx0, DST_Y0 + dy, w, 1, line);
    }
    tft.endWrite();
}

// Clears the TFT and makes the next sendBuffer() redraw every tile (used when
// the menu takes the screen back from the native clock face).
void u8g2TftInvalidate()
{
    tft.fillScreen(TFT_BLACK);
    memset(shadow, 0x55, sizeof(shadow)); // differs from any real tile content
}

extern "C" uint8_t u8x8_d_tft(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_DISPLAY_SETUP_MEMORY:
        u8x8_d_helper_display_setup_memory(u8x8, &tft_display_info);
        return 1;
    case U8X8_MSG_DISPLAY_INIT:
        u8x8_d_helper_display_init(u8x8);
        buildTables();
#if ESP_ARDUINO_VERSION_MAJOR >= 3
        ledcAttach(PIN_BL, 5000, 8);
#else
        ledcSetup(BL_CHANNEL, 5000, 8);
        ledcAttachPin(PIN_BL, BL_CHANNEL);
#endif
        tft.init();
        tft.setRotation(TFT_ROTATION);
        tft.setSwapBytes(true);
        tft.fillScreen(TFT_BLACK);
        memset(shadow, 0, sizeof(shadow));
        applyBacklight();
        return 1;
    case U8X8_MSG_DISPLAY_SET_POWER_SAVE:
        powerSave = arg_int != 0;
        applyBacklight();
        return 1;
    case U8X8_MSG_DISPLAY_SET_CONTRAST:
        backlight = arg_int;
        applyBacklight();
        return 1;
    case U8X8_MSG_DISPLAY_DRAW_TILE:
    {
        const u8x8_tile_t *t = (const u8x8_tile_t *)arg_ptr;
        const int ty = t->y_pos;
        int changedMin = SRC_W, changedMax = -1;
        for (int rep = 0; rep < arg_int; rep++)
        {
            for (int i = 0; i < t->cnt; i++)
            {
                const int tx = t->x_pos + rep * t->cnt + i;
                if (tx >= SRC_W / 8 || ty >= SRC_H / 8)
                    continue;
                uint8_t *dst = &shadow[ty * SRC_W + tx * 8];
                const uint8_t *src = t->tile_ptr + i * 8;
                if (memcmp(dst, src, 8) != 0)
                {
                    memcpy(dst, src, 8);
                    if (tx < changedMin)
                        changedMin = tx;
                    if (tx > changedMax)
                        changedMax = tx;
                }
            }
        }
        if (changedMax >= 0)
            blitTileRow(ty, changedMin, changedMax);
        return 1;
    }
    }
    return 0;
}
#endif
