#include "schedule.h"

#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"

#define NUM_TASKS 5

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

  initialize();  // from schedule.h

  while (1) {
    runTasks();  // from schedule.h
  }
}
 