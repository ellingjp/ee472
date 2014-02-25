/*
 * startup.c
 * author: Patrick ma
 * Date: 2/13/2014
 *
 * initializes the system and system variables
 */

#include "timebase.h"
#include "globals.h"
#include "driverlib/systick.h" // for systick interrupt (minor cycle)
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "startup.h"
#include "schedule.h"

/*
 * Minor Cycle Handler
 */
void SysTickIntHandler (void) {
  minor_cycle_ctr = minor_cycle_ctr + 1;
  runSchedule = true;
}

/* 
 * configure the hw interrupt for minor cycle
 */
void initializeHWCounter() {
SysTickPeriodSet(SysCtlClockGet() * MINOR_CYCLE / 1000);  // set timer period

// enable interrupts (master)
IntMasterEnable();	// this may be redundant, consider moving
SysTickIntEnable(); 	// enable systick interrupt
SysTickEnable();	// enable systic counter
}


/*
 * Initializes hw counter ans system state variables
 */
void startup() {
  initializeHWCounter();
  
  // Initialize global data
  initializeGlobalData();   // from globals.h

  computeActive = false;	// neither serial or compute task runs at start up
  serialActive = false;
	ekgProcessActive = false;

  initializeQueue(); // start up task queue with basic tasks
}

