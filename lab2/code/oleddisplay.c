/* 
 * oledDisplay.c
 * Author(s): jarrett Gaddy
 * 1/28/2014
 *
 * Implements oledDisplay.h
 */

#include "globals.h"
#include "timebase.h"
#include "oledDisplay.h"
#include "bool.h"
#include "drivers/rit128x96x4.h"
#include <stdlib.h>
#include <stdio.h>


// Internal data structure
typedef struct oledDisplayData {
  float *temperatureCorrected;
  float *systolicPressCorrected;
  float *diastolicPressCorrected;
  float *pulseRateCorrected;
  int *batteryState;
} OLEDDisplayData;

static OLEDDisplayData data;            // internal data
void *oledDisplayData = (void *)&data;  // external pointer to internal data

void initializeDisplayTask(void *data) {
  RIT128x96x4Init(1000000);
  
  OLEDDisplayData *mdata = (OLEDDisplayData *)data;
  mdata->temperatureCorrected = &(globalDataMem.temperatureCorrected);
  mdata->systolicPressCorrected = &(globalDataMem.systolicPressCorrected);
  mdata->diastolicPressCorrected = &(globalDataMem.diastolicPressCorrected);
  mdata->pulseRateCorrected = &(globalDataMem.pulseRateCorrected);
  mdata->batteryState = &(globalDataMem.batteryState);
}


void oledDisplayTask(void *dataptr) {
  // only run on major cycle
  if (IS_MAJOR_CYCLE) {   // on major cycle
    OLEDDisplayData *data = (OLEDDisplayData *) dataptr;

    char num[30];
    sprintf(num, "Temperature: %.2f C", *(data->temperatureCorrected));
    RIT128x96x4StringDraw(num, 0, 0, 15);
    
    sprintf(num, "Systolic Pressure: %.0f mm Hg", *(data->systolicPressCorrected));
    RIT128x96x4StringDraw(num, 0, 10, 15);
    
    sprintf(num, "Diastolic Pressure: %.0f mm Hg", *(data->diastolicPressCorrected));
    RIT128x96x4StringDraw(num, 0, 20, 15);
    
    sprintf(num, "Pulse rate: %d BPM", (int) *(data->pulseRateCorrected));
    RIT128x96x4StringDraw(num, 0, 30, 15);
	
    sprintf(num, "Battery: %d until death", *(data->batteryState));
    RIT128x96x4StringDraw(num,0,40,15);
  }
}
