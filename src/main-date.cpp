#include <Arduino.h>

#include "DisplayWrapper.h"
#include "RTCLibWrapper.h"

// #include <hebdate.h>
// #include "Gauss.h"

#include <hebrewcalendar.h>
#include <hdateformat.h>
#ifdef AVR_DEBUG
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr8-stub.h>
#include <app_api.h>
#endif
#include <DebugUtils.h>
DisplayWrapper disp = DisplayWrapper(DEF_ADDRESS, DEF_COLS, DEF_ROWS);
RTCLibWrapper rtc;
void setup()
{
#ifdef AVR_DEBUG
  debug_init();
#endif
  rtc.init();

#ifndef AVR_DEBUG
  Serial.begin(115200);
#endif
  debug(" Test");
  disp.init();
  disp.lightUp();
}

void loop()
{

  delay(1000);

  RTCLibWrapper::DateTimeValue dtv = rtc.now();
//פסח
  dtv.month = 5;
  dtv.day =5;
  dtv.year=2023;

  //חנוכה
  dtv.month = 12;
  dtv.day =24;// 5;
  dtv.year=2022;

 //תענית אסתר
  // dtv.month = 3;
  // dtv.day =6;// 5;
  // dtv.year=2023;

 dtv.month = 9;
  dtv.day =18;// 5;
  dtv.year=2023;

  dtv.month = 5;
  dtv.day =9;// 5;
  dtv.year=2023;

  char fmt[] = "DD/MM/YYYY hh:mm:ss";
  disp.display(rtc.toString(dtv, fmt));

  float timezone = 2.0; //-4.0;
  long int offset = (long int)3600 * timezone;

  tm ltm = rtc.convertToStdTime(dtv);

  hdate hebrewDate = convertDate(ltm);
  //disp.print(0, 3, "%d %d %d %d %d %d",hebrewDate.year, hebrewDate.month, hebrewDate.day, hebrewDate.wday, hebrewDate.dayofyear,hebrewDate.leap);
  // hebrewDate.year = 5783;
  // hebrewDate.month = 2;
  // hebrewDate.day =29;// 14;
  // hebrewDate.wday = 7;
  // hebrewDate.dayofyear = 22;

  hebrewDate.EY = 1; // if it's in israel TODO: cofigure per location

  hebrewDate.offset = offset;

  const char * day_name= numtowday(hebrewDate,1);
  disp.display(day_name,0,1,true);

  //char today[17] = {"\0"};
  //hdate_short_format(today, sizeof(today), hebrewDate);
  //disp.display(today, 6, 1, true);

  
  yomtov yom_tov = getyomtov(hebrewDate);
  if (yom_tov)
  {
    disp.display(yomtovformat(yom_tov), 0, 2, true);
  }
     
  yomtov isTomorrowNewMonth  = getmacharchodesh(hebrewDate);
  if (isTomorrowNewMonth){
    disp.display(yomtovformat(isTomorrowNewMonth),0,3,true);
  }
  yomtov newMonth= getroshchodesh(hebrewDate);
  if(newMonth){
    disp.display(yomtovformat(newMonth),0,3,true);
  }
  int omer_count=getomer(hebrewDate);
  if (omer_count)
	{
		char omer[7] = {"\0"};
		numtohchar(omer, 6, omer_count);  
    char omer_count[12]={"\0"};
    sprintf(omer_count,"%s בעומר",omer);
    disp.display(omer_count,11,3,true);
		//printf("%-15.15sספירת העומר: %s\n", "", omer);
    
	}
  //parshah p= getparshah(hebrewDate);
}