/* Author: patrick Ma
 * 2/21/14
 *
 * ekgcapture.c
 *
 * Reads and stores data from ekg sensor via ADC. Stores data into a memory
 * buffer.
 */

#include "driverlib/hw_types.h"
#include "driverlib/hw_memmap.h"
#include "driverlib/adc.h"	// for ADC use

#include "schedule.h"
#include "globals.h"
#include "ekgCapture.h"

#define SAMPLE_PERIOD (1 / 9375)	// # seconds between taking samples to get a good measure of < 3750 Hz
#define EKG_TIMER TIMER_A	// the timer used for EKG sample collection
#define EKG_SEQ 0	// The sequence number assigned to the ekg sensor
#define EKG_CH ADC_CTL_CH0	// the EKG analog input channel

static int sampleNum; // counter for data collection
static tBoolean firstRun = true;

// ekgCapture data structure. Internal to the task
typedef struct ekgCaptureData {
 unsigned int[NUM_EKG_SAMPLES] *ekgRawData;	// ADC output is 32-bit
}EKGCaptureData

void ekgCaptureRunFunction(void *ekgCaptureData);

static EKGCaptureData data; 	// the interal data
TCB ekgCaptureTask = {&ekgCaptureRunFunction, &data}; // task interface


// reads the ADC output FIFO to the current ekgRaw element. After taking enough sample measurements, doesn't do anything, signals collection is complete.
void ADCIntHandler() {
	ADCIntClear(ADC_BASE, EKG_SEQ);
	if (sampleNum < NUM_EKG_SAMPLES) {
	long samps = ADCSequenceDataGet(ADC_BASE, EKG_SEQ, (ekgRawData + sampleNum));
	sampleNum = sampleNum + samps;	// increase by however many you got
	} else {
		ekgComplete = true;	// Done taking samples, let the task know
	}
}

/* sets up task specific variables, etc
 */
void initializeEKGTask() {
	data.ekgRawData = &(global.ekgRawData);
	sampleNum = 0;

	// TODO enable read from GPIO pin (move this and below to startup?)
	
	// configure ADC sequence
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);
	ADCSequenceDisable(ADC_BASE, EKG_SEQ);
	ADCSequenceConfigure(	// configure when we want to run
			ADC_BASE, 
			EKG_SEQ, 
			ADC_TRIGGER_TIMER, 
			EKG_PRIORITY); 
	ADCSequenceStepConfigure(	// input ch, interrupt en, end seq
			ADC_BASE, 
			EKG_SEQ, 
			0, 
			EKG_CH | ADC_CTL_IE | ADC_CTL_END);	
	ADCSequenceEnable(ADC_BASE, EKG_SEQ);
	// enable ADC peripheral clock
	// set ADC sample rate
	// disable ADC (to safely change the ADC settings)
	// set the ekg ADC settings for: processor trigger and priority 0 (highest)
	// configure step setup:
	// for ekg, pick an analog channel, then interrupt enable, and end bit
	
	// configure timerA for periodic timing 
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR | TIMER_CFG_A_PERIODIC );
	TimerControlTrigger(TIMER0_BASE, TIMER_A, true);
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() * SAMPLE_PERIOD);
}

/*
 * captures a sequence of samples (given by NUM_EKG_SAMPLES) via the ADC and stores
 * the results into a buffer
 */
void ekgCaptureRunFunction(void *ekgCaptureData) {
	if firstRun {
		firstRun = false;
		initializeEKGTask();
	}

	ADCIntEnable(ADC_BASE, EKG_SEQ);
	TimerEnable(TIMER0_BASE, EKG_TIMER);

	while (!ekgComplete) {	// ADC is capturing signal measurements
	}

	TimerDisable(TIMER0_BASE, EKG_TIMER);
	ADCIntDisable(ADC_BASE, EKG_SEQ); 

	ekgProcessActive = true;	// we want to process our measurement
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

