#include <stdio.h>
#include <Arduino.h>
#ifdef DEBUG_AVR
#include <avr8-stub.h>
#include <app_api.h>
#endif

void debug(const char *fmt, ...)
{
  const size_t sz=50;
  char buffer[sz] = "";
    
#ifdef DEBUG_AVR
  va_list argptr;
  va_start(argptr, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, argptr);
  va_end(argptr);
  debug_message(buffer);
#endif
#ifdef DEBUG_CON
  va_list argptr;
  va_start(argptr, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, argptr);
  va_end(argptr);
  Serial.println(buffer);
#endif
}
