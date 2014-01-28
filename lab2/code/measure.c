/* 
 * measure.h
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Implements measure.c
 */

#include "globals.h"
#include "measure.h"
#include "drivers/rit128x96x4.h"

// Internal data structure
typedef struct measureData {
  int *temperatureRaw;
  int *systolicPressRaw;
  int *diastolicPressRaw;
  int *pulseRateRaw;
} MeasureData;

MeasureData data;                   // internal data
void *measureData = (void *)&data;  // external pointer to internal data

void initializeMeasureData() {
  measureData->temperatureRaw = &(globalDataMem.temperatureRaw);
  measureData->systolicPressRaw = &(globalDataMem.systolicPressRaw);
  measureData->diastolicPressRaw = &(globalDataMem.diastolicPressRaw);
  measureData->pulseRateRaw = &(globalDataMem.pulseRateRaw);
}

void measureTask(void *dataptr) {
  MeasureData *data = (MeasureData *) dataptr;
  data->pulseRateRaw++;

  RIT128x96x4StringDraw("Test", 0, 0, 15);
}
