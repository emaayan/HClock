#ifndef HCLOCK_NATIVE_FACE_H
#define HCLOCK_NATIVE_FACE_H

// Native 480x320 clock face drawn with TFT_eSPI (design: HClock TFT Face
// mockup). Only the drawing step is here; dates and zmanim come from the same
// HebDates / HebTimes / Scripture structs the OLED builds use.
//
// Regions are cached and only redrawn when their content changes, so calling
// nativeFaceDraw() on every render tick is cheap.

#include <TMWrapper.h>
extern "C"
{
#include <HebDateDisplay.h>
}

// Draws (or updates) the face. Returns true when it is night (after צאת
// הכוכבים or before עלות השחר), for backlight dimming.
bool nativeFaceDraw(const TMWrapper &tmw, float temp, const hdate &hd, const HebDates &hr, const HebTimes &ht,
                    const Scripture &scr);

// Forces a full redraw on the next nativeFaceDraw() (after the menu used the screen).
void nativeFaceInvalidate();

// Call before the menu draws: unloads the face's smooth font (the menu uses the
// built-in TFT_eSPI fonts) and drops the control bar.
void nativeFaceRelease();

// ---- touch ----
enum FaceAction
{
    FACE_NONE,
    FACE_MENU,
    FACE_BRIGHTER,
    FACE_DIMMER,
    FACE_CLOSE // the bar's ✕ or a tap above it; handled inside the face
};

// A new tap on the clock face (screen coordinates). The first tap raises the
// control bar; taps on its buttons return the action. The bar closes with its
// ✕ button, a tap above it, or 5 s after the last tap.
FaceAction nativeFaceTap(int x, int y);

// True while the control bar is up (the face stays at full brightness then).
bool nativeFaceBarVisible();

// Loads the touch calibration saved by TFTTouchTest into the TFT driver.
void nativeTouchInit();

#endif
