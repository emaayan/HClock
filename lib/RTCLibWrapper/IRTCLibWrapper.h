#ifndef IRTCLibWrapper_h
#define IRTCLibWrapper_h

#include <TMWrapper.h>
class IRTCLibWrapper
{
public:
    virtual void init() = 0;
    virtual TMWrapper now() = 0;
    virtual void changeTime(TMWrapper dtv) = 0;
};

#endif