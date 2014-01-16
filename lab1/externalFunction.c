#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"

//extern unsigned long *delay3; // don't need these since arguments
//extern char myFavoriteChar;
extern void delay(unsigned long aValue);

// prints out  the given character then waits a bit
void printAndWait (unsigned long * delay3, char myFavoriteChar) {
//define some local vars

  char myData[3] = {myFavoriteChar, '\0'};
  
//  for (i = 0; i < 10; i++)
//      {
//        myData[0] = i + '0';        //  convert the int i to ascii
//        myData[1] = '\0';           //  terminate the string
        
      //  k = k + 10;                //  start at oled position 15
                                   //  move 10 units to right for next
                                   //  character
           
        RIT128x96x4StringDraw(myData, k, 44, 15);
        
        delay(*delay3);                //  delay so we can read the display
      }
}
