#ifndef IDisplayWrapper_h
#define IDisplayWrapper_h
#include <stdint.h>
#include <stdio.h>

class IDisplayWrapper
{
public:
    virtual void init()=0;
    virtual void lightUp() = 0;    
    virtual size_t println(uint8_t row,bool rtl,const char *fmt, ...) = 0;
    virtual size_t display(const char *buffer,uint8_t col=0, uint8_t row=0,bool rtl=false)=0;
    virtual void clear() = 0;
};
#endif