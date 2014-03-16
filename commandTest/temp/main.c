// test file for ekg capture
//

#define DEBUG 1

#include "globals.h"
//#include "startup.h"
#include "driverlib/sysctl.h"
#include "commandTask.h"
//#include "schedule.h"

// Used for debug display
#if DEBUG
	#include "drivers/rit128x96x4.h"
	#include "utils/ustdlib.h"
#endif 


void main () {
	// SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | 
	//                SYSCTL_XTAL_8MHZ);

	// NOTE: actual clock speed is pll / 2/ div = 400M / 2/ 10
	SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ); 

 initializeGlobalData(); // initialize global data
#if DEBUG
 RIT128x96x4Init(1000000);
#endif

#if DEBUG
	char num[30];
	usnprintf(num, 30, "begin CommandTest");
	RIT128x96x4StringDraw(num, 0, 0, 15);
#endif

	strncpy(global.commandStr, "G D", COMMAND_LENGTH - 1);	// this is the test command

	commandTask.runTaskFunction(commandTask.taskDataPtr);
	
#if DEBUG
	usnprintf(num, 30, "done");
	RIT128x96x4StringDraw(num, 0, 80, 15);
#endif
}
