/*
 * ekgProcess.c
 * Author: Patrick Ma
 * 2/23/2014
 *
 * Processes the raw ekg samples and produces a frequency value
 *
 */

#define DEBUG_PROC 0

#include "FreeRTOS.h"
#include "task.h"

#include "globals.h"
#include "ekgProcess.h"
#include "CircularBuffer.h"
#include "optfft.h"
#include <stdio.h>
#include <string.h>

// Used for debug display
#if DEBUG_PROC
#include "drivers/rit128x96x4.h"
#include "utils/ustdlib.h"
static char num[30];
#endif 

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
void ekgProcessRunFunction(void *ekgData) {
	EKGProcessData *eData = (EKGProcessData *) ekgData;
	if (firstRun) {
		firstRun = false;
		initializeEKGProcess();
	}

	// need to bit shift >> 4 (divide 16) then subtract 32
	int i = 0;
	int t = 0;	// debug
	for (i = 0; i < NUM_EKG_SAMPLES; i++) {
#if DEBUG_PROC
		usnprintf(num, 30, "%d \n", eData->ekgRawData[i]);
		RIT128x96x4StringDraw(num, 0, 10, 15);
#endif
		int d = ((eData->ekgRawData)[i] >> 4) - 31;
		eData->ekgRawData[i] = d;
#if DEBUG_PROC
		if (eData->ekgRawData[i] > eData->ekgRawData[t])
			t = i;
		usnprintf(num, 30, "%d : %d \n", eData->ekgRawData[i], eData->ekgRawData[t]);
		RIT128x96x4StringDraw(num, 0, 20, 15);
#endif
	}
        
                	// reset Imaginary array
	memset(eData->ekgImgData, 0, sizeof(signed int) * NUM_EKG_SAMPLES);
//	memset(eData->ekgRawData, 0, sizeof(signed int) * NUM_EKG_SAMPLES);
        
	signed int max_index = optfft( eData->ekgRawData, eData->ekgImgData );
	//post processing
	int freq = ((9166) * max_index) / NUM_EKG_SAMPLES;

#if DEBUG_PROC
	usnprintf(num, 30, "%d : %d  ", max_index, freq);
	RIT128x96x4StringDraw(num, 0, 30, 15);
#endif
	cbAdd(eData->ekgFreqResult, &freq);

        vTaskSuspend(NULL);
}
