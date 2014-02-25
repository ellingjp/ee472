/* Author: patrick Ma
 * 2/21/14
 *
 * ekgcapture.c
 *
 * Reads and stores data from ekg sensor via ADC. Stores data into a memory
 * buffer.
 */

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
#if DEBUG
	#include "drivers/rit128x96x4.h"
	#include "utils/ustdlib.h"
#endif 

#define SAMPLE_PERIOD (1 / 9375)	// # seconds between taking samples to get a good measure of < 3750 Hz
#define EKG_TIMER TIMER_A	// the timer used for EKG sample collection
#define EKG_SEQ 0	// The sequence number assigned to the ekg sensor
#define EKG_CH ADC_CTL_CH0	// the EKG analog input channel
#define EKG_PRIORITY 0	// the ekg sequence capture priority

static unsigned long sampleNum; // counter for data collection
static tBoolean firstRun = true;
static tBoolean ekgComplete;

// ekgCapture data structure. Internal to the task
typedef struct ekgCaptureData {
 unsigned int (*ekgRawDataAddr);	// ADC output is 32-bit
}EKGCaptureData;

void ekgCaptureRunFunction(void *ekgCaptureData);
void ADCIntHandler();

static EKGCaptureData data; 	// the interal data
TCB ekgCaptureTask = {&ekgCaptureRunFunction, &data}; // task interface


// reads the ADC output FIFO to the current ekgRaw element. After taking enough
// sample measurements, doesn't do anything, signals collection is complete.
void ADC0IntHandler() {
	if (sampleNum < NUM_EKG_SAMPLES) {
		long samps = ADCSequenceDataGet(ADC0_BASE, EKG_SEQ, (unsigned long*)(data.ekgRawDataAddr + sampleNum));
		sampleNum = sampleNum + samps;	// increase by however many you got

#if DEBUG
		char num[30];
		usnprintf(num, 30, "ints handled, %d", sampleNum);
		RIT128x96x4StringDraw(num, 0, 20, 15);
#endif
	} else {

#if DEBUG
		char num[30];
		usnprintf(num, 30, "total ints %d", sampleNum);
		RIT128x96x4StringDraw(num, 0, 30, 15);

#endif
		ekgComplete = true;	// Done taking samples, let the task know
//		sampleNum = 0;
	}
	ADCIntClear(ADC0_BASE, EKG_SEQ);	// clear the interrupt
}

/* sets up task specific variables, etc
 */
void initializeEKGTask() {
#if DEBUG
	RIT128x96x4Init(1000000);
#endif
	data.ekgRawDataAddr =  &(global.ekgRaw);
        sampleNum = 0;
        ekgComplete = false;

	// TODO enable read from GPIO pin (move this and below to startup?)
	
	// configure ADC sequence

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_7);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);
	ADCSequenceDisable(ADC0_BASE, EKG_SEQ);
	ADCSequenceConfigure(	// configure when we want to run
			ADC0_BASE, 
			EKG_SEQ, 
			ADC_TRIGGER_TIMER, 
			EKG_PRIORITY); 
	ADCSequenceStepConfigure(	// input ch, interrupt en, end seq
			ADC0_BASE, 
			EKG_SEQ, 
			0,	// we're only using the first step
			EKG_CH | ADC_CTL_IE | ADC_CTL_END);	
	IntEnable(INT_ADC0SS0);
	ADCSequenceEnable(ADC0_BASE, EKG_SEQ);
	// enable ADC peripheral clock
	// set ADC sample rate
	// disable ADC (to safely change the ADC settings)
	// set the ekg ADC settings for: processor trigger and priority 0 (highest)
	// configure step setup:
	// for ekg, pick an analog channel, then interrupt enable, and end bit
	
	// configure timerA for periodic timing 
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerDisable(TIMER0_BASE, TIMER_A);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR | TIMER_CFG_A_PERIODIC);
	TimerControlTrigger(TIMER0_BASE, TIMER_A, true);
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() * MINOR_CYCLE);
#if DEBUG
	char num[30];
      long a =  TimerLoadGet(TIMER0_BASE, TIMER_A);
        usnprintf(num, 30, "timer: %d", a);
    RIT128x96x4StringDraw(num, 0, 50, 15);

#endif
}

/*
 * captures a sequence of samples (given by NUM_EKG_SAMPLES) via the ADC and stores
 * the results into a buffer
 */
void ekgCaptureRunFunction(void *ekgCaptureData) {
	if (firstRun) {
		firstRun = false;
		initializeEKGTask();
	}

	ADCIntEnable(ADC0_BASE, EKG_SEQ);
	TimerEnable(TIMER0_BASE, EKG_TIMER);
//ADCProcessorTrigger(ADC0_BASE, EKG_SEQ);
	while (!ekgComplete) {	// ADC is capturing signal measurements
//           ADCProcessorTrigger(ADC0_BASE, EKG_SEQ);
//#if DEBUG
//	char num[30];
//    usnprintf(num, 30, "running %d", sampleNum);
//    RIT128x96x4StringDraw(num, 0, 40, 15);
//
//#endif
	}

	TimerDisable(TIMER0_BASE, EKG_TIMER);
	ADCIntDisable(ADC0_BASE, EKG_SEQ); 

	ekgProcessActive = true;	// we want to process our measurement
#if DEBUG
	char num[30];
    usnprintf(num, 30, "finished ADC get, %d", sampleNum);
    RIT128x96x4StringDraw(num, 0, 10, 15);

#endif
}

/* pseudo code:
 * set up 2 cbuffers 256 elements, >10-bit/element
 * 
 * set an ADC channel to convert samples
 * create timer to run at frequency 2.5 * 3.75kHz, sending interrupt to sample
 * on interrupt, sample, get ADC result, store in raw buffer
 *
 * task:
 * start timer
 * collect 256 samples
 * set ekg process flag
 * exit
 */

