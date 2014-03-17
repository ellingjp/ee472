/* 
 * measure.h
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Implements measure.c
 * 
 * Uses port PA4 for interrupt input for pulse rate
 */

#define DEBUG_MEASURE 0

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
#include "driverlib/adc.h"

// Used for debug display
#if DEBUG_MEASURE
#include "drivers/rit128x96x4.h"
#include "utils/ustdlib.h"
#include "compute.h"
#include "ekgCapture.h"
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
  unsigned short *measureSelect;
	tBoolean *ekgCaptureDone;
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
  data.measureSelect = &(global.measurementSelection);
	data.ekgCaptureDone = &(global.ekgCaptureDone);
  
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlADCSpeedSet(SYSCTL_ADCSPEED_250KSPS);
	//setup for temperature sensor
        ADCSequenceDisable(ADC0_BASE, 1);
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 1);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_TS);
        ADCSequenceEnable(ADC0_BASE, 1);

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
  int temp = 0;
	//
	// Trigger the sample sequence.
	//
	ADCProcessorTrigger(ADC0_BASE, 1);
	//
	// Wait until the sample sequence has completed.
	//
	while(!ADCIntStatus(ADC0_BASE, 1, false))
	{
	}
	//
	// Read the value from the ADC.
	//
	ADCSequenceDataGet(ADC0_BASE, 1, &temp);

	
  cbAdd(tbuf, &temp);
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
  short measureSelect = *(mData->measureSelect);
	if(measureSelect == 0 || measureSelect == 1)
	{
		setTemp(mData->temperatureRaw);
    }
	if(measureSelect == 0 || measureSelect == 2)
	{
		setBloodPress(mData->systolicPressRaw, mData->diastolicPressRaw);
	}
	if(measureSelect == 0 || measureSelect == 3)
	{
      int prev = *(int*) cbGet(mData->pulseRateRaw);
      
      // Only save if +- 15%
      if (rate < prev*0.85 || rate > prev*1.15) {
        cbAdd(mData->pulseRateRaw, (void *)&rate);
      }
	}
	if(measureSelect == 0 || measureSelect == 4)
	{
		*(mData->ekgCaptureDone) = false;
		vTaskResume(ekgCaptureHandle);
#if DEBUG_MEASURE
		ekgCaptureTask.runTaskFunction(ekgCaptureTask.taskDataPtr);
		RIT128x96x4StringDraw("ekgCapture go!", 0, 50, 15);
#endif
	}
	else
	{
		vTaskSuspend(ekgCaptureHandle);
#if DEBUG_MEASURE
		RIT128x96x4StringDraw("no ekg!", 0, 50, 15);
#endif
	}
    vTaskResume(computeHandle);  // run the compute task
     
#if DEBUG_MEASURE
    char num[30];
    int temp = *(int *)cbGet(mData->temperatureRaw);
    int sys = *(int *)cbGet(mData->systolicPressRaw);
    int dia = *(int *)cbGet(mData->diastolicPressRaw);
    int pulse = *(int *)cbGet(mData->pulseRateRaw);
    int batt = global.batteryState;

    usnprintf(num, 30, "<-- MEASURE DEBUG -->");
    RIT128x96x4StringDraw(num, 0, 0, 15);
    
    usnprintf(num, 30, "Raw temp: %d  ", temp);
    RIT128x96x4StringDraw(num, 0, 10, 15);

    usnprintf(num, 30, "Raw Syst: %d  ", sys);
    RIT128x96x4StringDraw(num, 0, 20, 15);

    usnprintf(num, 30, "Raw Dia: %d  ", dia);
    RIT128x96x4StringDraw(num, 0, 30, 15);

    usnprintf(num, 30, "Raw Pulse: %d  ", pulse);
    RIT128x96x4StringDraw(num, 0, 40, 15);
    
    usnprintf(num, 30, "Raw Batt: %d  ", batt);
    RIT128x96x4StringDraw(num, 0, 50, 15);
#endif
}
