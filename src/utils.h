#include <time.h>                       // time() ctime()

// #define SEROUT

#define PTM(w) \
  Serial.print(":" #w "="); \
  Serial.print(tm->tm_##w);

#ifndef SEROUT
  #define MoveCursorToLine(v) \
    Serial.print("\e[" #v ";0H"); /* Move to Line V */ \
    Serial.print("\e[?25l"); /* hide cursor */ \
    

  #define ClearLineAtCursor() \
    Serial.print("\e[2K"); // Clear line

  #define CSFromCursorDown() \
    Serial.print("\e[J");    // clear screen from cursor down

  #define ClearScreen() \
    Serial.print("\e[J2");  // Clear Entire screen
#else

  #define MoveCursorToLine(v)
    
  #define ClearLineAtCursor()

  #define CSFromCursorDown()

  #define ClearScreen()

#endif

void printTm(const char* what, const tm* tm);
 
