/*
 * compute.c
 * Author(s): PatrickMa
 * 1/28/2014
 *
 * Implements compute.c
 */

#include "task.h"
#include "CircularBuffer.h"
#include "compute.h"
#include "globals.h"
#include "timebase.h"
#include "schedule.h"

// Used for debug display
#if DEBUG
#include "drivers/rit128x96x4.h"
#include "utils/ustdlib.h"
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

  if (IS_MAJOR_CYCLE) {
    ComputeData *cData = (ComputeData *) computeData;

    float temp = 5 + 0.75 * (*(int*)cbGet(cData->temperatureRaw));
    float systolic = 9 + 2 * (*(int*)cbGet(cData->systolicPressRaw));
    float diastolic = 6 + 1.5 * (*(int*)cbGet(cData->diastolicPressRaw));
    float pulseRate = 8 + 3 * (*(int*)cbGet(cData->pulseRateRaw));

    cbAdd(cData->temperatureCorrected, &temp);
    cbAdd(cData->systolicPressCorrected, &systolic);
    cbAdd(cData->diastolicPressCorrected, &diastolic);
    cbAdd(cData->pulseRateCorrected, &pulseRate);
    
    computeActive = false;  // remove self from task queue

#if DEBUG
    char num[30];
    usnprintf(num, 30, "Corrected temp: %d", (unsigned int) temp);
    RIT128x96x4StringDraw(num, 0, 50, 15);

    usnprintf(num, 30, "Corrected Syst: %d", (unsigned int) systolic);
    RIT128x96x4StringDraw(num, 0, 60, 15);

    usnprintf(num, 30, "Corrected Dia: %d", (unsigned int) diastolic);
    RIT128x96x4StringDraw(num, 0, 70, 15);

    usnprintf(num, 30, "Corrected Pulse: %d", (unsigned int) pulseRate);
    RIT128x96x4StringDraw(num, 0, 80, 15);
#endif
  }
}


