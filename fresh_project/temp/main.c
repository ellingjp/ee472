// test file for ekg capture
//

#include "globals.h"
#include "startup.h"
#include "driverlib/sysctl.h"
#include "ekgCapture.h"
#include "schedule.h"

extern tBoolean computeActive;
extern tBoolean serialActive;
extern tBoolean ekgProcessActive;

tBoolean ekgProcessActive;

void main () {
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | 
                 SYSCTL_XTAL_8MHZ);
    tBoolean ekgProcessActive = false;
  
	startup();
          SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);        // bank C



  // configure the pin C5 for 4mA output
  GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_5, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);
  GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_DIR_MODE_OUT);
      
        ekgCaptureTask.runTaskFunction(ekgCaptureTask.taskDataPtr);
}
