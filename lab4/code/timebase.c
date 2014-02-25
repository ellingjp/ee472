#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_ints.h"
#include "timer.h"
#include "sysctl.h"
#include "globals.h"
#include "interrupt.h"
#include "timebase.h"

#if DEBUG
#include "ustdlib.h"
#include "rit128x96x4.h"
#endif

unsigned int minor_cycle_ctr = 0;   // minor cycle counter

void initializeTimebase() {
  // Enable it
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  
  // Configure TimerA as periodic
  TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC);
    
  // Determine minor cycle
  TimerLoadSet(TIMER0_BASE, TIMER_A, (SysCtlClockGet() / 1000) * MINOR_CYCLE );
  
  // Enable Interrupt
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

  // Enable the intterupt (again?)
  IntEnable(INT_TIMER0A);
  
  // Enable the timer
  TimerEnable(TIMER0_BASE, TIMER_A);

}

void TimerAIntHandler(void) {
  minor_cycle_ctr++;
  
#if DEBUG
  char num[30];
  usnprintf(num, 30, "minor_cycle_ctr =  %d  ", minor_cycle_ctr);
  RIT128x96x4StringDraw(num, 0, 80, 15);
#endif
  
  TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}