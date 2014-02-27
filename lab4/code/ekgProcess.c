/*
 * ekgProcess.c
 * Author: Patrick Ma
 * 2/23/2014
 *
 * Processes the raw ekg samples and produces a frequency value
 *
 */

#define DEBUG_PROC 1

#include "globals.h"
#include "ekgProcess.h"
#include "CircularBuffer.h"
#include "optfft.h"

// Used for debug display
#if DEBUG_PROC
#include "drivers/rit128x96x4.h"
#include "utils/ustdlib.h"
static char num[30];
#endif 

extern tBoolean ekgProcessActive;

static tBoolean firstRun = true;

// ekgProcess data structure. Internal to this task
typedef struct egkProcessData {
	signed int *ekgRawData;
	signed int *ekgImgData;
	CircularBuffer *ekgFreqResult;
} EKGProcessData;

static EKGProcessData data;	// internal data object

void ekgProcessRunFunction(void *ekgProcessData);

TCB ekgProcessTask = {&ekgProcessRunFunction, &data};	// task interface

void initializeEKGProcess() {
	data.ekgRawData = (global.ekgRaw);
	data.ekgImgData = (global.ekgTemp);
	data.ekgFreqResult = &(global.ekgFrequencyResult);

#if DEBUG_PROC
	RIT128x96x4Init(1000000);
	RIT128x96x4StringDraw("* EKGProcess Debug *", 0, 0, 15);
#endif
}


/*
 * Reads in the EKG samples and performs a fast Fourier transform (FFT) on the
 * data to extract the primary frequency of the signal
 */
void ekgProcessRunFunction(void *ekgProcessData) {
	if (firstRun) {
		firstRun = false;
		initializeEKGProcess();
	}
	// reset img array
	int i = 0;
	for (i = 0; i < NUM_EKG_SAMPLES; i++){
	data.ekgImgData[i] = 0;
	}
	// need to bit shift >> 4 (divide 16) then subtract 32
	i = 0;
	int t = 0;	// debug
	for (i = 0; i < NUM_EKG_SAMPLES; i++) {
#if DEBUG_PROC
		usnprintf(num, 30, "%d \n", data.ekgRawData[i]);
		RIT128x96x4StringDraw(num, 0, 10, 15);
#endif
                int d = (data.ekgRawData[i] >> 4) - 31;
		data.ekgRawData[i] = d;
#if DEBUG_PROC
		if ((int)data.ekgRawData[i] > (int)data.ekgRawData[t])
			t = i;
		usnprintf(num, 30, "%d : %d ", data.ekgRawData[i], data.ekgRawData[t]);
		RIT128x96x4StringDraw(num, 0, 20, 15);
#endif
	}
        
        
	signed int max_index = optfft( data.ekgRawData, data.ekgImgData);
	//post processing
	int freq = (10000) * max_index / 8;

#if DEBUG_PROC
	usnprintf(num, 30, ": %d  ", max_index);
	RIT128x96x4StringDraw(num, 0, 30, 15);
#endif
//	cbAdd(data.ekgFreqResult, (void*) &freq);

	ekgProcessActive = false;
}
