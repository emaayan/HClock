#include <stdio.h>
#include <Arduino.h>
#ifdef DEBUG_AVR
#include <avr8-stub.h>
#include <app_api.h>
#endif

void debug(const char *fmt, ...)
{
  char buffer[20] = "";
  va_list argptr;
  va_start(argptr, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, argptr);
  va_end(argptr);

#ifdef DEBUG_AVR
  debug_message(buffer);
#endif
#ifdef DEBUG_CON
  Serial.println(buffer);
#endif
}
