#include <LCDDisplayWrapper.h>
#include <DebugUtils.h>
#include <stdint.h>
char * printHebrew(const char str[], char dst[])
{
    size_t bPos = 0;
    const size_t sz = strlen(str);
    uint8_t i;
    for (i = 0; i < sz; i++)
    {
        uint16_t ch1 = (uint8_t)str[i];
        if (ch1 == 0xD7) // probably hebrew
        {

            uint8_t ch2 = str[i + 1];
            ch2 += 16;
            i++;
            dst[bPos++] = ch2; // write single letter. this is the important function
        }
        else
        {
            if (ch1 == ' ')
            {
                dst[bPos++] = ch1; // writing a space
            }
            else
            {
                bool space_in_end = false;
                size_t j;
                for (j = i; j < sz && (uint8_t)str[j] != 0XD7; j++)
                    ; // the for do nothing, it just count how many non-hebrew charecters are
                j--;  // a for loop rise the index +1 at the last time when the condition is false

                if (str[j] == ' ')
                {
                    j--; // pass the space that should be in the end and "save" it, and it writes on the "if" below, after writing the other charecters
                    space_in_end = true;
                }
                for (int e = j; e >= i; e--)
                {
                    dst[bPos++] = (uint8_t)str[e]; // write single letter. because that function write right-to-left, it goes back, from the last char to the first.
                }
                if (space_in_end)
                {
                    dst[bPos++] = ' '; // writing a space

                    j++;
                }
                i = j;
            }
        }
    }
    return dst;
}

size_t LCDDisplayWrapper::println(const uint8_t row, const bool rtl, const char *fmt, ...)
{
    const size_t size = (rtl ? _cols * 2 : _cols) + 1;
    char buffer[size] = "";
    if (rtl)
    { // must be done cause don't know how to pad spaces
        clearRow(row, rtl);
    }

    va_list argptr;
    va_start(argptr, fmt);

    vsnprintf(buffer, sizeof(buffer), fmt, argptr);
    va_end(argptr);

    return display(buffer, 0, row, rtl);
}

size_t LCDDisplayWrapper::display(const char *buffer, const uint8_t col, const uint8_t row, const bool rtl)
{
    if (rtl)
    {         
        _lcd.rightToLeft();
        _lcd.setCursor(_cols - (col + 1), row);

         const size_t size =(_cols * 2)+1;
         char bf[size] = "";
         printHebrew(buffer,bf);
         return _lcd.print(bf);

        //  return _lcd.print(buffer);
    }
    else
    {
        _lcd.leftToRight();
        _lcd.setCursor(col, row);
        return _lcd.print(buffer);
    }
}