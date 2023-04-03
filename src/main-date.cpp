#include <Arduino.h>

#include "DisplayWrapper.h"
#include "RTCLibWrapper.h"

// #include <hebdate.h>
// #include "Gauss.h"
extern "C"
{
#include <hebrewcalendar.h>
#include <hdateformat.h>
}
#ifdef AVR_DEBUG
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr8-stub.h>
#include <app_api.h>
#endif
#include <DebugUtils.h>

DisplayWrapper disp = DisplayWrapper();
RTCLibWrapper rtc;
void setup()
{
#ifdef AVR_DEBUG
  debug_init();
#endif
#ifdef DEBUG_CON
  Serial.begin(115200);
#endif
  
  rtc.init();


  debug(" Test");
  disp.init();
  disp.lightUp();
}

void loop()
{

  //delay(1000);

  TMWrapper dtv = rtc.now();
//פסח
  // dtv.tm_mon = 5;
  // dtv.tm_mday =5;
  // dtv.tm_year=2023;

  // //חנוכה
  // dtv.tm_mon = 12;
  // dtv.tm_mday =24;// 5;
  // dtv.tm_year=2022;

 //תענית אסתר
  // dtv.month = 3;
  // dtv.day =6;// 5;
  // dtv.year=2023;

//  dtv.tm_mon = 9;
//   dtv.tm_mday =18;// 5;
//   dtv.tm_year=2023;

//   dtv.tm_mon = 5;
//   dtv.tm_mday =9;// 5;
//   dtv.tm_year=2023;

  char fmt[20] = "";
  disp.display(dtv.toDateTimeString(fmt,sizeof(fmt)));

  float timezone = 2.0; //-4.0;
  long int offset = (long int)3600 * timezone;

  tm ltm =dtv.get_tm();// rtc.convertToStdTime(dtv);

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

  
  // yomtov yom_tov = getyomtov(hebrewDate);
  // if (yom_tov)
  // {
  //   disp.display(yomtovformat(yom_tov), 0, 2, true);
  // }
     
  // yomtov isTomorrowNewMonth  = getmacharchodesh(hebrewDate);
  // if (isTomorrowNewMonth){
  //   disp.display(yomtovformat(isTomorrowNewMonth),0,3,true);
  // }
  // yomtov newMonth= getroshchodesh(hebrewDate);
  // if(newMonth){
  //   disp.display(yomtovformat(newMonth),0,3,true);
  // }
  // int omer_count=getomer(hebrewDate);
  // if (omer_count)
	// {
	// 	char omer[7] = {"\0"};
	// 	numtohchar(omer, 6, omer_count);  
  //   char omer_count[12]={"\0"};
  //   sprintf(omer_count,"%s בעומר",omer);
  //   disp.display(omer_count,11,3,true);
	// 	//printf("%-15.15sספירת העומר: %s\n", "", omer);
    
	// }
  //parshah p= getparshah(hebrewDate);
}