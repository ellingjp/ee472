/* 
 * display.c
 * Author(s): jarrett Gaddy
 * 1/28/2014
 *
 * Implements display.h
 */

#include "globals.h"
#include "task.h"
#include "timebase.h"
#include "display.h"
#include "inc/hw_types.h"
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
} DisplayData;

static DisplayData data;  // internal data
TCB displayTask;          // task interface

void initializeDisplayTask() {
  RIT128x96x4Init(1000000);

  // Load data
  data.temperatureCorrected = &(globalDataMem.temperatureCorrected);
  data.systolicPressCorrected = &(globalDataMem.systolicPressCorrected);
  data.diastolicPressCorrected = &(globalDataMem.diastolicPressCorrected);
  data.pulseRateCorrected = &(globalDataMem.pulseRateCorrected);
  data.batteryState = &(globalDataMem.batteryState);

  // Load TCB
  displayTask.taskRunFunction = &displayRunFunction;
  displayTask.taskDataPtr = &data;
}


void displayRunFunction(void *dataptr) {
  // only run on major cycle
  //  if (IS_MAJOR_CYCLE) {   // on major cycle
  OLEDDisplayData *data = (OLEDDisplayData *) dataptr;

  char num[30];
  sprintf(num, "Temperature: %.2f C ", *(data->temperatureCorrected));
  RIT128x96x4StringDraw(num, 0, 0, 15);

  sprintf(num, "Systolic Pressure:");
  RIT128x96x4StringDraw(num, 0, 10, 15);

  sprintf(num, "%.0f mm Hg ", *(data->systolicPressCorrected));
  RIT128x96x4StringDraw(num, 0, 20, 15);

  sprintf(num, "Diastolic Pressure:");
  RIT128x96x4StringDraw(num, 0, 30, 15);

  sprintf(num, "%.0f mm Hg ", *(data->diastolicPressCorrected));
  RIT128x96x4StringDraw(num, 0, 40, 15);

  sprintf(num, "Pulse rate: %d BPM ", (int) *(data->pulseRateCorrected));
  RIT128x96x4StringDraw(num, 0, 50, 15);

  sprintf(num, "Battery: %d %% ", *(data->batteryState)/2);
  RIT128x96x4StringDraw(num,0,60,15);
  //  }
}
