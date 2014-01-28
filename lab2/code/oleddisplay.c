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
  int *temperatureCorrected;
  int *systolicPressCorrected;
  int *diastolicPressCorrected;
  int *pulseRateCorrected;
  int *batteryState;
} oledDisplayData;

oledDisplayData data;                   // internal data
void *oledDisplayData = (void *)&data;  // external pointer to internal data

void initializeoledDisplayData(void *data) {
  RIT128x96x4Init(1000000);

  
  oledDisplayData *mdata = (oledDisplayData *)data;
  mdata->temperatureCorrected = &(globalDataMem.temperatureRaw);
  mdata->systolicPressCorrected = &(globalDataMem.systolicPressRaw);
  mdata->diastolicPressCorrected = &(globalDataMem.diastolicPressRaw);
  mdata->pulseRateCorrected = &(globalDataMem.pulseRateRaw);
  mdata->batteryState = &(globalDataMem.batteryState);
}


void oledDisplayTask(void *dataptr) {
  // only run on major cycle
  if (IS_MAJOR_CYCLE) {   // on major cycle
    oledDisplayData *data = (oledDisplayData *) dataptr;


    char num[30];
    sprintf(num, "Temperature: %.2f C", *(data->temperatureRaw));
    RIT128x96x4StringDraw(num, 0, 0, 15);
    
    sprintf(num, "Systolic Pressure: %i mm Hg", *(data->systolicPressRaw));
    RIT128x96x4StringDraw(num, 0, 10, 15);
    
    sprintf(num, "Diastolic Pressure: %i mm Hg", *(data->diastolicPressRaw));
    RIT128x96x4StringDraw(num, 0, 20, 15);
    
    sprintf(num, "Pulse rate: %d BPM", *(data->pulseRateRaw));
    RIT128x96x4StringDraw(num, 0, 30, 15);
	
	sprintf(num, "Battery: %d until death", *(data->batteryState));
	RIT28x96x24StringDraw(num,0,40,15);
	
	
  }
}
