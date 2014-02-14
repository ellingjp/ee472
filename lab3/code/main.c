#include "schedule.h"
#include "timebase.h"
#include "startup.h"
#include "globals.h"

#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "inc/hw_memmap.h"
#include "driverlib/systick.h" // for systick interrupt (minor cycle)
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"


#define NUM_TASKS 5
#define BOARD_CLK_RATE	8000000	// 8MHz

#ifdef DEBUG
  void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif


int main(void) {
  // Set the clocking to run directly from the crystal.
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
      SYSCTL_XTAL_8MHZ);

  startup();    // initialize system state and variables

  while (1) {
      runTasks();  // from schedule.h
  }
}
