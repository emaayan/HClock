#include <Arduino.h>

#include <TMWrapper.h>
#include <RTClib.h>
RTC_DS3231 _rtcCtrl;
// DS3231 _rtcCtrl;

#include <Wire.h>

#include <LiquidCrystal_I2C.h>

#define DEF_ADDRESS 0X27
#define DEF_COLS 20
#define DEF_ROWS 4
LiquidCrystal_I2C _lcd = LiquidCrystal_I2C(DEF_ADDRESS, DEF_COLS, DEF_ROWS);
char *printHebrew(const char str[], char dst[])
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
extern "C"
{
#include <HebDateDisplay.h>
}

void sendBrightness(uint8_t brightness)
{
    analogWrite(BRIGHTNESS_PIN, brightness);
}

uint8_t brightness;
const uint8_t brightAdjResolution = 10;
const int BRIGHT_ADDR = 1;

void initBrightness()
{
    pinMode(BRIGHTNESS_PIN, OUTPUT);
    brightness = 150; // _confSettings->initSetting(BRIGHT_ADDR, 100);
    sendBrightness(brightness);
}

void initLCD()
{
    _lcd.init();
    _lcd.clear();
    _lcd.backlight();
}

void initRTC()
{

    if (!_rtcCtrl.begin())
    {
        while (1)
            delay(10);
    }
    if (_rtcCtrl.lostPower())
    // if (!_rtcCtrl.isrunning())
    {
        _rtcCtrl.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
}

void setup()
{
    Wire.begin();
    initLCD();
    initBrightness();
    initRTC();
}

void display()
{


    DateTime dt = _rtcCtrl.now();
    TMWrapper tmw = TMWrapper(dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
    const tm tm = tmw.get_tm();

    char buffer[18] = "";
    tmw.toDateTimeString(buffer, size_t(buffer));
    _lcd.leftToRight();
    _lcd.setCursor(0, 0);
    _lcd.print(buffer);

    HebDates hr = {"", "", "", "", "", ""};
    displayHebDates(tm, true, 2, &hr);

    char bf[40] = "";    

    snprintf(bf, size_t(bf), "%s %s %s %s", hr.day_name, hr.dayInMonth, hr.monthName, hr.isNewMonthIndicator);
    char h_bf[20]="";            
    _lcd.rightToLeft();
    _lcd.setCursor(19, 1);
    memset(h_bf,' ',sizeof(h_bf));
    _lcd.print(printHebrew(bf,h_bf));

    _lcd.rightToLeft();
    _lcd.setCursor(19, 2);
    memset(h_bf,' ',sizeof(h_bf));
    _lcd.print(printHebrew(hr.festivalName,h_bf));


    _lcd.rightToLeft();
    _lcd.setCursor(19, 3);
    memset(h_bf,' ',sizeof(h_bf));
    _lcd.print(printHebrew(hr.omer_count_name,h_bf));
    
}
void loop()
{
    display();
}