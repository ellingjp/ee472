/* 
 * measure.h
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Implements measure.c
 * 
 * Uses port PA4 for interrupt input for pulse rate
 */

#include "task.h"
#include "CircularBuffer.h"
#include "globals.h"
#include "timebase.h"
#include "measure.h"
#include "schedule.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"

// Used for debug display
#if DEBUG
#include "drivers/rit128x96x4.h"
#include "utils/ustdlib.h"
#endif 

// prototype for compiler
void measureRunFunction(void *dataptr);  
void PulseRateISR(void);

// Internal data structure
typedef struct measureData {
  CircularBuffer *temperatureRaw;
  CircularBuffer *systolicPressRaw;
  CircularBuffer *diastolicPressRaw;
  CircularBuffer *pulseRateRaw;
} MeasureData;

static int pulseRate = 0;
static MeasureData data;  // internal data
TCB measureTask = {&measureRunFunction, &data};  // task interface

void initializeMeasureTask() {
#if DEBUG
  RIT128x96x4Init(1000000);
#endif
  // Load data memory
  data.temperatureRaw = &(global.temperatureRaw);
  data.systolicPressRaw = &(global.systolicPressRaw);
  data.diastolicPressRaw = &(global.diastolicPressRaw);
  data.pulseRateRaw = &(global.pulseRateRaw);


  /* Interrupt setup
   * Note: using statically registered interrupts, because they're faster
   *       this means we aren't using the dynamic GPIOPortIntRegister() function,
   *       instead, an entry in the interrupt table is populated with the address
   *       of the interrupt handler (under GPIO Port A) and this is enabled with
   *       IntEnable(INT_GPIOA) */

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

  // Set PA4 as input
  GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_4);
  
  // Enable interrupts on PA4
  GPIOPinIntEnable(GPIO_PORTA_BASE, GPIO_PIN_4);

  // Set interrupt type
  GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_4, GPIO_RISING_EDGE);
 
  // Enable interrupts to the processor.
  IntMasterEnable();

  // Enable the interrupts.
  IntEnable(INT_GPIOA);
}

void setTemp(CircularBuffer *tbuf) {
  static unsigned int i = 0;
  static tBoolean goingUp = true;

  int temp = *(int *)cbGet(tbuf);

  if (temp > 50)
    goingUp = false;
  else if (temp < 15)
    goingUp = true;

  if (goingUp) {
    if (i%2==0) temp+=2;
    else temp--;
  }
  else {
    if (i%2==0) temp-=2;
    else temp++;
  }

  cbAdd(tbuf, &temp);

  i++;
}

void setBloodPress(CircularBuffer *spbuf, CircularBuffer *dpbuf) {
  // This is written to lab spec, with a flag to indicate "complete".
  // Right now, it does nothing, but I imagine it should probably be a global
  // variable to indicate to the compute task that the pressure measurement 
  // is ready, since this measurement takes a nontrivial amount of time

  static unsigned int i = 0;

  static tBoolean sysComplete = false;    
  static tBoolean diaComplete = false;

  int syspress = *(int *)cbGet(spbuf);

  // Restart systolic measurement if diastolic is complete

  if (syspress > 100)
    sysComplete = true;

  if (! sysComplete) {
    if (i%2==0) syspress+=3;
    else syspress--;
  }

  int diapress = *(int *)cbGet(dpbuf);

  // Restart diastolic measurement if systolic is complete  
  if (diapress < 40)
    diaComplete = true;

  if (!diaComplete) {
    if (i%2==0) diapress-=2;
    else diapress++;
  }

  if (diaComplete && sysComplete) {
    sysComplete = false;
    diaComplete = false;
    syspress = SYS_RAW_INIT;
    diapress = DIA_RAW_INIT;
  }

  cbAdd(spbuf, &syspress);
  cbAdd(dpbuf, &diapress);

  i++;
}

// pulse rate interrupt handler
void PulseRateISR(void) {
  pulseRate++;
  
  GPIOPinIntClear(GPIO_PORTA_BASE, GPIO_PIN_4);
}

void measureRunFunction(void *dataptr) {
  static tBoolean onFirstRun = true;
  static int rate;
  MeasureData *mData = (MeasureData *) dataptr;

  if (onFirstRun) {
    initializeMeasureTask();
    rate = *(int*) cbGet(mData->pulseRateRaw);
    onFirstRun = false;
  }
  
  // capture pulse rate
  if (IS_PULSE_CYCLE) {
    // Divide by two so raw pulse rate matches frequency
    rate = pulseRate/2;
    pulseRate = 0;
  }
  
  // only run on major cycle
  if (IS_MAJOR_CYCLE) {
    setTemp(mData->temperatureRaw);
    setBloodPress(mData->systolicPressRaw, mData->diastolicPressRaw);

      int prev = *(int*) cbGet(mData->pulseRateRaw);
      
      // Only save if +- 15%
      if (rate < prev*0.85 || rate > prev*1.15) {
        cbAdd(mData->pulseRateRaw, (void *)&rate);
      }
    computeActive = true;   // run the compute task
     
#if DEBUG
    char num[30];
    int temp = *(int *)cbGet(mData->temperatureRaw);
    int sys = *(int *)cbGet(mData->systolicPressRaw);
    int dia = *(int *)cbGet(mData->diastolicPressRaw);
    int pulse = *(int *)cbGet(mData->pulseRateRaw);
    int batt = global.batteryState;

    usnprintf(num, 30, "Raw temp: %d  ", temp);
    RIT128x96x4StringDraw(num, 0, 0, 15);

    usnprintf(num, 30, "Raw Syst: %d  ", sys);
    RIT128x96x4StringDraw(num, 0, 10, 15);

    usnprintf(num, 30, "Raw Dia: %d  ", dia);
    RIT128x96x4StringDraw(num, 0, 20, 15);

    usnprintf(num, 30, "Raw Pulse: %d  ", pulse);
    RIT128x96x4StringDraw(num, 0, 30, 15);
    
    usnprintf(num, 30, "Raw Batt: %d  ", batt);
    RIT128x96x4StringDraw(num, 0, 40, 15);
#endif
  }
}
