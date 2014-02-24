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

#define SAMPLE_PERIOD (1 / 9375)	// # seconds between taking samples to get a god measure of < 3750 Hz
#define EKG_SEQ 0	// The sequence number assigned to the ekg sensor
#define EKG_CH ADC_CTL_CH0	// the EKG analog input channel

static int sampleNum;
static tBoolean firstRun = true;

// ekgCapture data structure. Internal to the task
typedef struct ekgCaptureData {
 unsigned int[NUM_EKG_SAMPLES] *ekgRawData;	// ADC output is 32-bit
}EKGCaptureData

void ekgCaptureRunFunction(void *ekgCaptureData);

static EKGCaptureData data; 	// the interal data
TCB ekgCaptureTask = {&ekgCaptureRunFunction, &data}; // task interface


void initializeEKGTask() {
	data.ekgRawData = &(global.ekgRawData);
	sampleNum = 0;

	// TODO enable read from GPIO pin (move this and below to startup?)
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);
	ADCSequenceDisable(ADC_BASE, EKG_SEQ);
	ADCSequenceConfigure(
			ADC_BASE, 
			EKG_SEQ, 
			ADC_TRIGGER_PROCESSOR, 
			EKG_PRIORITY); // TODO use ALWAYS trigger and do control in run function?
	ADCSequenceStepConfigure(
			ADC_BASE, 
			EKG_SEQ, 
			0, 
			EKG_CH | ADC_CTL_IE | ADC_CTL_END);	// input ch, interrupt en, end seq
	ADCSequenceEnable(ADC_BASE, EKG_SEQ);
	// enable ADC peripheral clock
	// set ADC sample rate
	// disable ADC (to safely change the ADC settings)
	// set the ekg ADC settings for: processor trigger and priority 0 (highest)
	// configure step setup:
	// for ekg, pick an analog channel, then interrupt enable, and end bit
	// TODO create an ADC interrupt handler to copy the ADC value to the next element of the ekg raw array
}

// reads the ADC output FIFO to the current ekgRaw element. Does update the
// index
void ADCIntHandler() {
	ADCIntClear(ADC_BASE, EKG_SEQ);
	
	long samps = ADCSequenceDataGet(ADC_BASE, EKG_SEQ, (ekgRawData + sampleNum));
	sampleNum = sampleNum + samps;	// increase by however many you got
	if (sampleNum > NUM_EKG_SAMPLES)
		ekgComplete = true;
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
	// TODO setup timer
	while (!ekgComplete) {
		//delay for SAMPLE_PERIOD
		ADCProcessorTrigger(ADC_BASE, EKG_SEQ);

		while(!ADCIntStatus(ADC_BASE, EKG_SEQ, false)) {	//wait until converted
		}
	// TODO chose EITHER to do conversion & capture in an interrupt handler or in the function call.	
		ADCIntClear(ADC_BASE, EKG_SEQ);
		
		long samps = ADCSequenceDataGet(ADC_BASE, EKG_SEQ, (ekgRawData + sampleNum));
		sampleNum = sampleNum + samps;	// increase by however many you got
		if (sampleNum > NUM_EKG_SAMPLES)
			ekgComplete = true;
	}

	ekgProcessActive = true;
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

