#ifndef IDisplayWrapper_h
#define IDisplayWrapper_h

#include <stdio.h>
#define DEF_ADDRESS 0X27
#define DEF_COLS 20
#define DEF_ROWS 4


class IDisplayWrapper
{
public:
    virtual void init()=0;
    virtual void lightUp() = 0;    
    virtual void println(uint8_t row,bool rtl,const char *fmt, ...) = 0;
    virtual void display(const char *buffer,uint8_t col=0, uint8_t row=0,bool rtl=false)=0;
    virtual void clear() = 0;
};
#endif