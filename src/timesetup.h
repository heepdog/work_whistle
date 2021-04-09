#include <Arduino.h>
#include <time.h>                       // time() ctime()
#include <sys/time.h>                   // struct timeval
#include <coredecls.h>                  // settimeofday_cb()

_VOID _EXFUN(tzset,	(_VOID));
int	_EXFUN(setenv,(const char *__string, const char *__value, int __overwrite));

// TZ string setup
// TZ string information:
// https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
extern const char *TZstr; 

extern timeval cbtime;			// time set in callback
extern bool cbtime_set;

void time_is_set(void);

void setup_timezones(void);
