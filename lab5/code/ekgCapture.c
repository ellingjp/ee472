/* Author: patrick Ma
 * 2/21/14
 *
 * ekgcapture.c
 *
 * Reads and stores data from ekg sensor via ADC. Stores data into a memory
 * buffer.
 */

#define DEBUG_EKG 1	// ekg task debug

#include "FreeRTOS.h"
#include "task.h"

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"	// for ADC use
#include "driverlib/timer.h"	// for hw timer use
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"

#include "schedule.h"
#include "globals.h"
#include "ekgCapture.h"
#include "timebase.h"

// Used for debug display
#if DEBUG_EKG
#include "drivers/rit128x96x4.h"
#include "utils/ustdlib.h"
#endif 

#if DEBUG_EKG
static char num[30];	// used for display
#endif

static tBoolean firstRun = true;
static tBoolean ekgComplete = false;
extern xTaskHandle ekgProcessHandle;

// ekgCapture data structure. Internal to the task
typedef struct ekgCaptureData {
	signed int *ekgRawDataAddr;	// raw output array address
} EKGCaptureData;

static EKGCaptureData data; 	// the internal data

void ekgCaptureRunFunction(void *ekgCaptureData);     // Compiler function prototypes
TCB ekgCaptureTask = {&ekgCaptureRunFunction, &data}; // task interface
void ADC0IntHandler();

/* 
 * sets up task specific variables, etc
 */
void initializeEKGTask() {
#if DEBUG_EKG
	RIT128x96x4Init(1000000);
	RIT128x96x4StringDraw("* EKGCapture Debug *", 0, 0, 15);
#endif
	data.ekgRawDataAddr = global.ekgRaw;

	// Enable read from GPIO pin (move this and below to startup?)
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_7);
        
	// Setup ADC0 for EKG capture
	ADCSequenceDisable(ADC0_BASE, EKG_SEQ);
	ADCSequenceConfigure(ADC0_BASE, EKG_SEQ, ADC_TRIGGER_TIMER, 1);
	ADCSequenceStepConfigure(ADC0_BASE, EKG_SEQ, 0, ADC_CTL_IE | ADC_CTL_END | EKG_CH);
	ADCIntEnable(ADC0_BASE, EKG_SEQ);
	ADCIntRegister(ADC0_BASE, EKG_SEQ, ADC0IntHandler);
        
	// configure timer (uses both 16-bit timers) for periodic timing 
	SysCtlPeripheralEnable(EKG_TIMER_PERIPH);
	TimerDisable(EKG_TIMER_BASE, EKG_TIMER);
	TimerConfigure(EKG_TIMER_BASE, EKG_CFG );
	TimerControlTrigger(EKG_TIMER_BASE, EKG_TIMER, true);
	TimerLoadSet(EKG_TIMER_BASE, EKG_TIMER, (SysCtlClockGet() / SAMPLE_FREQ));
	TimerEnable(EKG_TIMER_BASE, EKG_TIMER);

#if DEBUG_EKG
	long timeLoad =  TimerLoadGet(EKG_TIMER_BASE, EKG_TIMER);
	int secs = (int) ((float) timeLoad / SysCtlClockGet());
	usnprintf(num, 30, "timer: %d s %d c", secs, timeLoad);
	RIT128x96x4StringDraw(num, 0, 0, 15);
#endif
}

/*
 * captures a sequence of samples (given by NUM_EKG_SAMPLES) via the ADC and
 * stores the results into a buffer
 */
void ekgCaptureRunFunction(void *ekgCaptureData) {
	if (firstRun) {
		firstRun = false;
		initializeEKGTask();
	}
        
	ekgComplete = false;	// reset the adc counters

	ADCSequenceEnable(ADC0_BASE, EKG_SEQ);
	while (!ekgComplete) {
	}       
	ADCSequenceDisable(ADC0_BASE, EKG_SEQ);

#if DEBUG_EKG
	usnprintf(num, 30, "end ADC get");
	RIT128x96x4StringDraw(num, 0, 40, 15);

	// let's check our values

	int a = global.ekgRaw[0];
	int b = global.ekgRaw[50];
	int c = (int) *(data.ekgRawDataAddr + 255);
	usnprintf(num, 30, "%d, %d, %d", a, b, c);
	RIT128x96x4StringDraw(num, 3, 50, 15);
#endif
        
        vTaskResume(ekgProcessHandle);
}

void ADC0IntHandler() {
  	ADCIntClear(ADC0_BASE, EKG_SEQ);
        
	unsigned long value;
	static int i = 0;

	// Read the value from the ADC.
	while (1 != ADCSequenceDataGet(ADC0_BASE, EKG_SEQ, &value));
	(data.ekgRawDataAddr)[i++] = (signed int) value;
        
	// Done sampling if we've taken enough samples
	if (i >= NUM_EKG_SAMPLES) {
		i = 0;
		ekgComplete = true;
	}
}
