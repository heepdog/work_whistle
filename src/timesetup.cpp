#include "timesetup.h"
#include "utils.h"
//Command string to set ntp timezone to Centeral and use Daylignt savings
const char *TZstr = "CST+6CDT,M3.2.0/2,M11.1.0/2";
//set when time is synced
bool cbtime_set = false;
//global time variable
timeval cbtime;

void time_is_set(void)
{
  gettimeofday(&cbtime, NULL);
  cbtime_set = true;
  MoveCursorToLine(20)
      time_t tnow = time(nullptr);
  Serial.printf("--- settimeofday() was called at %s ---\n", asctime(localtime(&tnow)));
  MoveCursorToLine(0)
}

// set up ntp time 
void setup_timezones(void)
{

  settimeofday_cb(time_is_set);

  configTime(0, 0, "pool.ntp.org");

  setenv("TZ", TZstr, 1);
  tzset();
}
