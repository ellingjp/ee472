/*
 * compute.c
 * Author(s): PatrickMa
 * 1/28/2014
 *
 * Implements compute.c
 */

#define DEBUG_COMPUTE 0

#include "CircularBuffer.h"
#include "compute.h"
#include "globals.h"
#include "timebase.h"
#include "schedule.h"

// Used for debug display
#if DEBUG_COMPUTE
#include "drivers/rit128x96x4.h"
#include "utils/ustdlib.h"
#include "ekgProcess.h"
#endif 

// computeData structure internal to compute task
typedef struct computeData {
  // raw data pointers
  CircularBuffer *temperatureRaw;
  CircularBuffer *systolicPressRaw;
  CircularBuffer *diastolicPressRaw;
  CircularBuffer *pulseRateRaw;

  //corrected data pointers
  CircularBuffer *temperatureCorrected;
  CircularBuffer *systolicPressCorrected;
  CircularBuffer *diastolicPressCorrected;
  CircularBuffer *pulseRateCorrected;

	tBoolean *measurementComplete;
	unsigned short *measurementSelect;
	tBoolean *ekgCaptureDone;
	tBoolean *ekgProcessDone;
} ComputeData;

void computeRunFunction(void *computeData);

static ComputeData data;    // the internal data
TCB computeTask = {&computeRunFunction, &data};  // task interface

/*
 * Initializes the computeData task values (pointers to variables, etc)
 */
void initializeComputeTask() {
  data.temperatureRaw = &(global.temperatureRaw);
  data.systolicPressRaw = &(global.systolicPressRaw);
  data.diastolicPressRaw = &(global.diastolicPressRaw);
  data.pulseRateRaw = &(global.pulseRateRaw);

  data.temperatureCorrected = &(global.temperatureCorrected);
  data.systolicPressCorrected = &(global.systolicPressCorrected);
  data.diastolicPressCorrected = &(global.diastolicPressCorrected);
  data.pulseRateCorrected = &(global.pulseRateCorrected);

	data.measurementComplete = &(global.measurementComplete);
	data.measurementSelect = &(global.measurementSelection);
	data.ekgCaptureDone = &(global.ekgCaptureDone);
	data.ekgProcessDone = &(global.ekgProcessDone);
}

/* 
 * Linearizes the raw data measurement and converts value into human 
 * readable format
 */
void computeRunFunction(void *computeData) {
  static tBoolean onFirstRun = true;

  if (onFirstRun) {
    initializeComputeTask();
    onFirstRun = false;
  }

  ComputeData *cData = (ComputeData *) computeData;

  float temp = (5 + 0.75 * (*(int*)cbGet(cData->temperatureRaw)))/10;
  float systolic = 9 + 2 * (*(int*)cbGet(cData->systolicPressRaw));
  float diastolic = 6 + 1.5 * (*(int*)cbGet(cData->diastolicPressRaw));
  float pulseRate = 8 + 3 * (*(int*)cbGet(cData->pulseRateRaw));

  cbAdd(cData->temperatureCorrected, &temp);
  cbAdd(cData->systolicPressCorrected, &systolic);
  cbAdd(cData->diastolicPressCorrected, &diastolic);
  cbAdd(cData->pulseRateCorrected, &pulseRate);
  
//	if (0 == *(cData->measurementSelect) || 4 == *(cData->measurementSelect)) {
//		while(!(cData->ekgCaptureDone)) {	// wait until ekg captured
//		}
//	vTaskResume(ekgProcessHandle);
//
//		RIT128x96x4StringDraw("go ekgProcess", 0, 60, 15);
//		*(cData->ekgProcessDone) = false;
//		while (!*(cData->ekgProcessDone)) {	// wait until ekg computed
//		}
//	}
	*(cData->measurementComplete) = true;

  vTaskSuspend(NULL);  // suspend self

#if DEBUG_COMPUTE
  char num[30];
  
  usnprintf(num, 30, "<-- COMPUTE DEBUG -->");
  RIT128x96x4StringDraw(num, 0, 0, 15);
  
  usnprintf(num, 30, "Corrected temp: %d", (unsigned int) temp);
  RIT128x96x4StringDraw(num, 0, 10, 15);

  usnprintf(num, 30, "Corrected Syst: %d", (unsigned int) systolic);
  RIT128x96x4StringDraw(num, 0, 20, 15);

  usnprintf(num, 30, "Corrected Dia: %d", (unsigned int) diastolic);
  RIT128x96x4StringDraw(num, 0, 30, 15);

  usnprintf(num, 30, "Corrected Pulse: %d", (unsigned int) pulseRate);
  RIT128x96x4StringDraw(num, 0, 40, 15);
#endif
}


