#include "utils.h"
#include <Arduino.h>
// #include <time.h>                       // time() ctime()
// #include <sys/time.h>                   // struct timeval
// #include <coredecls.h>                  // settimeofday_cb()


void printTm(const char* what, const tm* tm) {
  Serial.print(what);
  PTM(isdst); PTM(yday); PTM(wday);
  PTM(year);  PTM(mon);  PTM(mday);
  PTM(hour);  PTM(min);  PTM(sec);
}
