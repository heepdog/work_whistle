#include <time.h>                       // time() ctime()



#define PTM(w) \
  Serial.print(":" #w "="); \
  Serial.print(tm->tm_##w);

#define MoveCursorToLine(v) \
  Serial.print("\e[" #v ";0H"); /* Move to Line V */ \
  Serial.print("\e[?25l"); /* hide cursor */ \
  

#define ClearLineAtCursor() \
  Serial.print("\e[2K"); // Clear line

#define CSFromCursorDown() \
  Serial.print("\e[J");    // clear screen from cursor down

#define ClearScreen() \
  serial.print("\e[J2");  // Clear Entire screen


void printTm(const char* what, const tm* tm);
 
