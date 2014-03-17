/* 
 * display.c
 * Author(s): jarrett Gaddy
 * 2/10/2014
 *
 * Implements display.h
 */

#include "globals.h"
#include "timebase.h"
#include "display.h"
#include "inc/hw_types.h"
#include "drivers/rit128x96x4.h"
#include "utils/ustdlib.h"
#include <stdlib.h>

#define DISPLAY_OFF 0

// Internal data structure
typedef struct oledDisplayData {
  CircularBuffer *temperatureCorrected;
  CircularBuffer *systolicPressCorrected;
  CircularBuffer *diastolicPressCorrected;
  CircularBuffer *pulseRateCorrected;
  CircularBuffer *ekgFrequencyResult;
  unsigned short *batteryState;
  unsigned short *mode;
  unsigned short *measurementSelection;
  unsigned short *scroll;
  tBoolean *alarmAcknowledge;
  tBoolean *select;
  unsigned short *selection;
} DisplayData;

void displayRunFunction(void *dataptr);  // prototype for compiler

static DisplayData data;  // internal data
TCB displayTask = {&displayRunFunction, &data};          // task interface

void initializeDisplayTask() {
  // Load data
  data.temperatureCorrected = &(global.temperatureCorrected);
  data.systolicPressCorrected = &(global.systolicPressCorrected);
  data.diastolicPressCorrected = &(global.diastolicPressCorrected);
  data.pulseRateCorrected = &(global.pulseRateCorrected);
  data.batteryState = &(global.batteryState);
  data.ekgFrequencyResult = &(global.ekgFrequencyResult);

  data.mode = &(global.mode);
  data.measurementSelection = &(global.measurementSelection);  
  data.scroll = &(global.scroll);
  data.alarmAcknowledge = &(global.alarmAcknowledge);
  data.select = &(global.select);
}


void displayRunFunction(void *dataptr) {
  static tBoolean onFirstRun = true;
   
  if (onFirstRun) {
    initializeDisplayTask();
    onFirstRun = false;
  }
  
  DisplayData *dData = (DisplayData *) dataptr;

  tBoolean selection = *(dData->select);
  int scroll = *(dData->scroll);

#if !DISPLAY_OFF
  char num[40];
  //char buf1[30];
  char buf2[30];
  
  if(0 == *(dData->mode))
  {
    if(false == selection)
    {
      RIT128x96x4StringDraw("Make Selection                        ", 0, 0, 15);
      RIT128x96x4StringDraw("  Blood Pressure                      ", 0, 10, 15);
      RIT128x96x4StringDraw("  Temperature                         ", 0, 20, 15);
      RIT128x96x4StringDraw("  Pulse Rate                          ", 0, 30, 15);
      RIT128x96x4StringDraw("  EKG                                 ", 0, 40, 15);
      RIT128x96x4StringDraw("  Battery                             ", 0, 50, 15);
      RIT128x96x4StringDraw("                                      ", 0, 60, 15);
	  RIT128x96x4StringDraw("                                      ", 0, 70, 15);
      RIT128x96x4StringDraw("->", 0, 10*((scroll%5+1)), 15);
    }
    else
    {
      RIT128x96x4StringDraw("                                      ", 0, 0, 15);    
      if(0 == scroll%5)
        RIT128x96x4StringDraw("Blood Pressure:", 0, 0, 15);
      else if(1 == scroll%5)
        RIT128x96x4StringDraw("Temperature:", 0, 0, 15);
      else if(2 == scroll%5)
        RIT128x96x4StringDraw("Pulse Rate:", 0, 0, 15);
      else if(3 == scroll%5)
        RIT128x96x4StringDraw("EKG:", 0, 0, 15);
	  else if(4 == scroll%5)
        RIT128x96x4StringDraw("Battery:", 0, 0, 15);
		
      else RIT128x96x4StringDraw("oops", 0, 0, 15);//just in case



      if(0 == scroll%5)
        usnprintf(buf2,30, "Systolic: %d mm Hg ", (int) *( (float*) cbGet(dData->systolicPressCorrected)));
      else if(1 == scroll%5)
        usnprintf(buf2,30,"%d C ", (int) *( (float*) cbGet(dData->temperatureCorrected)));
      else if(2 == scroll%5)
        usnprintf(buf2,30, "%d BPM ", (int) *( (float*) cbGet(dData->pulseRateCorrected)));
	  else if(3 == scroll%5)
        usnprintf(buf2,30, "%d Hz ", (int) *( (float*) cbGet(dData->ekgFrequencyResult)));
      else if(4 == scroll%5)
        usnprintf(buf2,30, "%d %%  ", (int) *(dData->batteryState)/2);
      //else buf2 = "oops"; //just in case      

      RIT128x96x4StringDraw("                                      ", 0, 10, 15);
      RIT128x96x4StringDraw(buf2, 0, 10, 15);
      if(0 == scroll%5)
      {
        usnprintf(buf2,30, "Diastolic: %d mm Hg        ", (int)*( (float*) cbGet(dData->diastolicPressCorrected)));
      
        RIT128x96x4StringDraw(buf2, 0, 20, 15);
      }
      else RIT128x96x4StringDraw("                                      ", 0, 20, 15);
      RIT128x96x4StringDraw("                                      ", 0, 30, 15);
      RIT128x96x4StringDraw("                                      ", 0, 40, 15);
      RIT128x96x4StringDraw("                                      ", 0, 50, 15);
      RIT128x96x4StringDraw("                                      ", 0, 60, 15);
	  RIT128x96x4StringDraw("                                      ", 0, 70, 15);

    }
  }
  else//(1 == *(data->mode)
  {
    usnprintf(num,40,"Temperature: %d C           ", (int) *( (float*) cbGet(dData->temperatureCorrected)));
    RIT128x96x4StringDraw(num, 0, 0, 15);

    usnprintf(num,40, "Systolic Pressure:             ");
    RIT128x96x4StringDraw(num, 0, 10, 15);

    usnprintf(num,40, "%d mm Hg                   ", (int) *( (float*) cbGet(dData->systolicPressCorrected)));
    RIT128x96x4StringDraw(num, 0, 20, 15);

    usnprintf(num,40, "Diastolic Pressure:            ");
    RIT128x96x4StringDraw(num, 0, 30, 15);

    usnprintf(num,40, "%d mm Hg                      ",(int) *( (float*) cbGet(dData->diastolicPressCorrected)));
    RIT128x96x4StringDraw(num, 0, 40, 15);

    usnprintf(num,40, "Pulse rate: %d BPM              ",(int) *( (float*) cbGet(dData->pulseRateCorrected)));
    RIT128x96x4StringDraw(num, 0, 50, 15);
	
	usnprintf(num,40, "EKG: %d Hz                      ",(int) *( (float*) cbGet(dData->ekgFrequencyResult)));
    RIT128x96x4StringDraw(num, 0, 60, 15);

    usnprintf(num,40, "Battery: %d %%                 ",(int) *(dData->batteryState)/2);
    RIT128x96x4StringDraw(num,0, 70,15);
  }
#endif
}
