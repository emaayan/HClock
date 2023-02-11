#ifndef RTCLibWrapper_h
#define RTCLibWrapper_h

#include <IRTCLibWrapper.h>
#include <RTClib.h>

class RTCLibWrapper : public IRTCLibWrapper
{
public:
    void init() override;
    DateTimeValue now() override;
    struct tm nowInStdLocalTime() override;
    char *toString(DateTimeValue dtv, char *fmt) override;
    char *toString(char *fmt) override;
    void changeTime(DateTimeValue dtv) override;
    int32_t differenceInSeconds(DateTimeValue leftdtv, DateTimeValue rightdtv) override;
    DateTimeValue increment(DatePart dp, DateTimeValue dtv) override;
    DateTimeValue decrement(DatePart dp, DateTimeValue dtv) override;    
    tm convertToStdTime(DateTimeValue dtv) override;

private:
    RTC_DS3231 _rtc;
};
#endif