/*
 * ekgProcess.c
 * Author: Patrick Ma
 * 2/23/2014
 *
 * Processes the raw ekg samples and produces a frequency value
 *
 */

#include "globals.h"
#include "ekgProcess.h"
#include "CircularBuffer.h"
#include "optfft.h"

static tBoolean firstRun = true;

// ekgProcess data structure. Internal to this task
typedef struct egkProcessData {
	unsigned int[] *ekgRawData;
	unsigned int[] *ekgTempBuff;
	CircularBuffer *ekgFreqResult;
} EKGProcessData

void ekgProcessRunFunction(void *ekgProcessData);

TCB ekgProcessTask = {&ekgProcessRunFunction, &data};	// task interface
static EKGProcessData data;	// internal data object

void initializeEKGProcess() {
	data.ekgRawData = &(global.ekgRawData);
	data.ekgImgData = &(global.ekgTempBuff);
	data.ekgFreqResult = &(global.ekgFrequencyResult);
}

/*
 * Reads in the EKG samples and performs a fast Fourier transform (FFT) on the
 * data to extract the primary frequency of the signal
 */
void ekgProcessRunRunction(void *ekgProcessData) {
	if (firstRun) {
		firstRun = false;
		initializeEKGProcess();
	}

	int fftResult = optfft( (data -> ekgRawData), (data -> ekgImgData));
	cbAdd(ekgFreqResult, (void*) &fftResult);

	ekgProcessActive = false;
}
