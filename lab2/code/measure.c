#include "globals.h"
#include "measure.h"
#include "drivers/rit128x96x4.h"

void initializeMeasureData(MeasureData *data) {
  data->temperatureRaw = &(globalDataMem.temperatureRaw);
  data->systolicPressRaw = &(globalDataMem.systolicPressRaw);
  data->diastolicPressRaw = &(globalDataMem.diastolicPressRaw);
  data->pulseRateRaw = &(globalDataMem.pulseRateRaw);
}

void measureTask(void *dataptr) {
    MeasureData *data = (MeasureData *) dataptr;
    data->pulseRateRaw++;
        
    RIT128x96x4StringDraw("Test", 0, 0, 15);
}