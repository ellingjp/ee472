// test file for ekg capture
//

#include "globals.h"
#include "startup.h"
#include "driverlib/sysctl.h"
#include "ekgCapture.h"
#include "schedule.h"

// Used for debug display
#if DEBUG
	#include "drivers/rit128x96x4.h"
	#include "utils/ustdlib.h"
#endif 

extern tBoolean computeActive;
extern tBoolean serialActive;
extern tBoolean ekgProcessActive;

tBoolean ekgProcessActive;

void main () {
 // SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | 
 //                SYSCTL_XTAL_8MHZ);
  
  // NOTE: actual clock speed is pll / 2/ div = 400M / 2/ 10
  SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ); 
    tBoolean ekgProcessActive = false;
  
	startup();
          SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);        // bank C

  // configure the pin C5 for 4mA output
  GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_5, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);
  GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_DIR_MODE_OUT);
      
        ekgCaptureTask.runTaskFunction(ekgCaptureTask.taskDataPtr);
        #if DEBUG
	char num[30];
        usnprintf(num, 30, "done");
    RIT128x96x4StringDraw(num, 0, 60, 15);
#endif
}
