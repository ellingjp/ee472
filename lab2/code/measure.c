#include "globals.h"
#include "measure.h"
#include "drivers/rit128x96x4.h"

MeasureData measureData;

void initializeMeasureData() {
  measureData->temperatureRaw = &(globalDataMem.temperatureRaw);
  measureData->systolicPressRaw = &(globalDataMem.systolicPressRaw);
  measureData->diastolicPressRaw = &(globalDataMem.diastolicPressRaw);
  measureData->pulseRateRaw = &(globalDataMem.pulseRateRaw);
}

void measureTask(MeasureData *dataptr) {
  MeasureData *data = (MeasureData *) dataptr;
  data->pulseRateRaw++;

  RIT128x96x4StringDraw("Test", 0, 0, 15);
}
