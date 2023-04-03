#ifndef RTCLibWrapper_h
#define RTCLibWrapper_h

#include <IRTCLibWrapper.h>
#include <Wire.h>

#include <RTClib.h>
#include <DebugUtils.h>

class RTCLibWrapper : public IRTCLibWrapper
{
public:
    void init() override;
    TMWrapper now() override;
    void changeTime(TMWrapper dtv) override;

private:
    RTC_DS3231 _rtcCtrl;
};
#endif