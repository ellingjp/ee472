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
//  runSchedule = true;
}


/*
 * Initializes hw counter ans system state variables
 */
void startup() {
  IntMasterEnable();
  initializeTimebase();
  
  // Initialize global data
  initializeGlobalData();   // from globals.h

  //computeActive = false;	// neither serial or compute task runs at start up
  //serialActive = false;
  //ekgProcessActive = false;

//  initializeQueue(); // start up task queue with basic tasks
}

