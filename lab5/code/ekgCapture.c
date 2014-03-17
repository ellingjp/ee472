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
	tBoolean *ekgCaptureDone;
}EKGCaptureData;

static EKGCaptureData data; 	// the interal data

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
	data.ekgCaptureDone = &(global.ekgCaptureDone);

	// Enable read from GPIO pin (move this and below to startup?)
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_7);
        
	// Setup ADC0 for EKG capture
        SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
        SysCtlADCSpeedSet(SYSCTL_ADCSPEED_500KSPS);
	ADCSequenceDisable(ADC0_BASE, EKG_SEQ);
	ADCSequenceConfigure(ADC0_BASE, EKG_SEQ, ADC_TRIGGER_PROCESSOR, 1);
	ADCSequenceStepConfigure(ADC0_BASE, EKG_SEQ, 0, ADC_CTL_IE | ADC_CTL_END | EKG_CH);
	ADCIntRegister(ADC0_BASE, EKG_SEQ, ADC0IntHandler);
        ADCIntEnable(ADC0_BASE, EKG_SEQ);
        
	// configure timer (uses both 16-bit timers) for periodic timing 
//	SysCtlPeripheralEnable(EKG_TIMER_PERIPH);
//	TimerDisable(EKG_TIMER_BASE, EKG_TIMER);
//	TimerConfigure(EKG_TIMER_BASE, EKG_CFG );
//	TimerControlTrigger(EKG_TIMER_BASE, EKG_TIMER, true);
//	TimerLoadSet(EKG_TIMER_BASE, EKG_TIMER, (SysCtlClockGet() / SAMPLE_FREQ));
//	TimerEnable(EKG_TIMER_BASE, EKG_TIMER);

#if DEBUG_EKG
        long clk = SysCtlClockGet();
	long timeLoad =  TimerLoadGet(EKG_TIMER_BASE, EKG_TIMER);
	usnprintf(num, 30, "load: %ul", timeLoad);
	RIT128x96x4StringDraw(num, 0, 10, 15);
        usnprintf(num, 30, "clock: %ul", clk);
	RIT128x96x4StringDraw(num, 0, 20, 15);
#endif
}

void delay_in_ms(int ms) {
  for (volatile int i = 0; i < ms; i++)
    for (volatile int j = 0; j < 50; j++);
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

	EKGCaptureData *eData = (EKGCaptureData *) ekgCaptureData;
	ekgComplete = false;	// reset the adc counters

	ADCSequenceEnable(ADC0_BASE, EKG_SEQ);
	while (!ekgComplete) {
          ADCProcessorTrigger(ADC0_BASE, EKG_SEQ);
          delay_in_ms(1);
	}       
	ADCSequenceDisable(ADC0_BASE, EKG_SEQ);

	*(eData->ekgCaptureDone) = true;	// we want to process our measurement

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
	unsigned long value;
	static int i = 0;
debugPin47();
	// Read the value from the ADC.
        ADCIntClear(ADC0_BASE, EKG_SEQ);
	while (1 !=  ADCSequenceDataGet(ADC0_BASE, EKG_SEQ, &value));
	(data.ekgRawDataAddr)[i++] = (signed int) value;
        
	// Done sampling if we've taken enough samples
	if (i >= NUM_EKG_SAMPLES) {
		i = 0;
		ekgComplete = true;
	}

}
