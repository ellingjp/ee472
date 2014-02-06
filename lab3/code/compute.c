/*
 * compute.c
 * Author(s): PatrickMa
 * 1/28/2014
 *
 * Implements compute.c
 */

<<<<<<< HEAD
#include "task.h"
=======
>>>>>>> a6bd9b48cfc3011aaf42afba151673c7e5cc9c86
#include "compute.h"
#include "globals.h"
#include "timebase.h"

// Used for debug display
#if DEBUG
#include "drivers/rit128x96x4.h"
#include <stdlib.h>
#include <stdio.h>
#endif 

// computeData structure internal to compute task
typedef struct computeData {
  // raw data pointers
  int *temperatureRaw;
  int *systolicPressRaw;
  int *diastolicPressRaw;
  int *pulseRateRaw;

  //corrected data pointers
  float *tempCorrected;
  float *systPressCorrected;
  float *diastPressCorrected;
  float *pulseCorrected;
} ComputeData;

<<<<<<< HEAD
static ComputeData data;    // the internal data
TCB computeTask;            // task interface

void computeRunFunction(void *computeData);  // prototype for compiler
  
/*
 * Initializes the computeData task values (pointers to variables, etc)
 */
void initializeComputeTask() {
  data.temperatureRaw = &(globalDataMem.temperatureRaw);
  data.systolicPressRaw = &(globalDataMem.systolicPressRaw);
  data.diastolicPressRaw = &(globalDataMem.diastolicPressRaw);
  data.pulseRateRaw = &(globalDataMem.pulseRateRaw);

  data.tempCorrected = &(globalDataMem.temperatureCorrected);
  data.systPressCorrected = &(globalDataMem.systolicPressCorrected);
  data.diastPressCorrected = &(globalDataMem.diastolicPressCorrected);
  data.pulseCorrected = &(globalDataMem.pulseRateCorrected);

  computeTask.runTaskFunction = &computeRunFunction;
  computeTask.taskDataPtr = &data;
}

=======
static ComputeData data;              // the internal data
void *computeData = (void *) &data;   // set the external ptr to the data

/*
 * Initializes the computeData task values (pointers to variables, etc)
 */
void initializeComputeTask(void *data) {
  ComputeData *cData = (ComputeData *) data;
  cData->temperatureRaw = &(globalDataMem.temperatureRaw);
  cData->systolicPressRaw = &(globalDataMem.systolicPressRaw);
  cData->diastolicPressRaw = &(globalDataMem.diastolicPressRaw);
  cData->pulseRateRaw = &(globalDataMem.pulseRateRaw);

  cData->tempCorrected = &(globalDataMem.temperatureCorrected);
  cData->systPressCorrected = &(globalDataMem.systolicPressCorrected);
  cData->diastPressCorrected = &(globalDataMem.diastolicPressCorrected);
  cData->pulseCorrected = &(globalDataMem.pulseRateCorrected);
}


>>>>>>> a6bd9b48cfc3011aaf42afba151673c7e5cc9c86
/* 
 * Linearizes the raw data measurement and converts value into human 
 * readable format
 */
<<<<<<< HEAD
void computeRunFunction(void *computeData) {
=======
void computeTask(void *computeData) {
>>>>>>> a6bd9b48cfc3011aaf42afba151673c7e5cc9c86
  if (IS_MAJOR_CYCLE) {
    ComputeData *cData = (ComputeData *) computeData;
    *(cData->tempCorrected) = 5 + 0.75 * (*(cData->temperatureRaw));
    *(cData->systPressCorrected) = 9 + 2 * (*(cData->systolicPressRaw));
    *(cData->diastPressCorrected) = 6 + 1.5 * (*(cData->diastolicPressRaw));
    *(cData->pulseCorrected) = 8 + 3 * (*(cData->pulseRateRaw));

#if DEBUG
    char num[30];
    sprintf(num, "Corrected temp: %f", *(cData->tempCorrected));
    RIT128x96x4StringDraw(num, 0, 40, 15);

    sprintf(num, "Raw Syst: %f", *(cData->systPressCorrected));
    RIT128x96x4StringDraw(num, 0, 50, 15);

    sprintf(num, "Raw Dia: %f", *(cData->diastPressCorrected));
    RIT128x96x4StringDraw(num, 0, 60, 15);

    sprintf(num, "Raw Pulse: %f", *(cData->pulseCorrected));
    RIT128x96x4StringDraw(num, 0, 70, 15);
#endif
  }
}


