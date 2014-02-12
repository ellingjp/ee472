#include "schedule.h"
#include "timebase.h"

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

static tBoolean runSchedule = false;

/*
 * Minor Cycle Handler
 */
void SysTickIntHandler (void) {
  minor_cycle_ctr = minor_cycle_ctr + 1;
  // run scheduler?
  runSchedule = true;
}

/* 
 * configure the hw interrupt for minor cycle
 */
void initializeHWCounter() {
SysTickPeriodSet(SysCtrlPeriodGet() * MINOR_CYCLE / 1000);	// set timer period

// enable interrupts (master)
IntMasterEnable();	// this may be redundant, consider moving
SysTickIntEnable(); 	// enable systick interrupt
SysTickEnable();	// enable systic counter
}

int main(void) {
  // Set the clocking to run directly from the crystal.
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
      SYSCTL_XTAL_8MHZ);

//  initializeHWCounter();	// start the hw timer

  initialize();  // from schedule.h

  while (1) {
 //   if (runSchedule) {
 //     runSchedule = false;
      runTasks();  // from schedule.h
 //   }  
  }
}
