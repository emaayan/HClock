#include <stdio.h>
#include <Arduino.h>
#ifdef DEBUG_AVR
#include <avr8-stub.h>
#include <app_api.h>
#endif
void debug(const char *fmt, ...)
{
#ifdef DEBUG_AVR
  char buffer[256] = "";
  va_list argptr;
  va_start(argptr, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, argptr);
  va_end(argptr);

  debug_message(buffer);
#endif
#ifdef DEBUG_CON
  char buffer[60] = "";
  va_list argptr;
  va_start(argptr, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, argptr);
  va_end(argptr);

  Serial.println(buffer);
#endif
}
