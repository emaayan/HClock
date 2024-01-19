#include <Arduino.h>
#include <U8g2lib.h>
//nano
#define CS 10
#define DC 6
#define RESET 9

#include <SPI.h>


U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI u8g2(U8G2_R0, CS, DC, RESET); ///* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);    
void initOLED()
{
    pinMode(RESET, OUTPUT);
    // Give a low to high pulse to the OLED display to reset it
    // This is optional and not required for OLED modules not containing a reset pin
    digitalWrite(RESET, LOW);
    delay(20);
    digitalWrite(RESET, HIGH);     
}

void setup()
{   
    
    initOLED();
    // Wire.begin();
    
    u8g2.begin();    
     u8g2.enableUTF8Print();
    // Serial.begin(9600);

    //     u8g2.clearBuffer(); // clear the internal memory
    //  u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
}

void loop()
{
    u8g2.firstPage();
    do
    {         
        u8g2.setFont(u8g2_font_6x13_t_hebrew);
        u8g2.drawExtUTF8(127, 15, 1, NULL, "1שלום ");

        u8g2.setFont(u8g2_font_6x13B_t_hebrew);
        u8g2.drawExtUTF8(127, 30, 1, NULL, "2שלום ");

        u8g2.setFont(u8g2_font_cu12_t_hebrew);
        u8g2.drawExtUTF8(127, 45, 1, NULL, "3שלום ");

        u8g2.setFont(u8g2_font_unifont_t_hebrew);
        u8g2.drawExtUTF8(127, 60, 1, NULL, "4שלום ");


    //   //  u8g2.sendBuffer();

     } while (u8g2.nextPage());
}