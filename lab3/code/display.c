/* 
 * display.c
 * Author(s): jarrett Gaddy
 * 2/10/2014
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
  CircularBuffer *temperatureCorrected;
  CircularBuffer *systolicPressCorrected;
  CircularBuffer *diastolicPressCorrected;
  CircularBuffer *pulseRateCorrected;
  unsigned short *batteryState;
  unsigned short *mode;
  unsigned short *measurementSelection;
  unsigned short *scroll;
  tBoolean *alarmAcknowledge;
  tBoolean *select;
  unsigned short *selection;
} DisplayData;

static DisplayData data;  // internal data
TCB displayTask;          // task interface

void displayRunFunction(void *dataptr);  // prototype for compiler
  
void initializeDisplayTask() {
  RIT128x96x4Init(1000000);

  // Load data
  data.temperatureCorrected = &(global.temperatureCorrected);
  data.systolicPressCorrected = &(global.systolicPressCorrected);
  data.diastolicPressCorrected = &(global.diastolicPressCorrected);
  data.pulseRateCorrected = &(global.pulseRateCorrected);
  data.batteryState = &(global.batteryState);

  
  data.mode = &(global.mode);
  data.measurementSelection = &(global.measurementSelection);  
  data.scroll = &(global.scroll);
  data.alarmAcknowledge = &(global.alarmAcknowledge);
  data.select = &(global.select);
  

  // Load TCB
  displayTask.runTaskFunction = &displayRunFunction;
  displayTask.taskDataPtr = &data;
}


void displayRunFunction(void *dataptr) {
  // only run on major cycle
  //  if (IS_MAJOR_CYCLE) {   // on major cycle
  DisplayData *dData = (DisplayData *) dataptr;
  
  tBoolean selection = *(dData->select);
  int scroll = *(dData->scroll);

  char num[40];
  char buf1[30];
  char buf2[30];
	if(0 == *(dData->mode))
	{
		if(false == selection)
		{
			RIT128x96x4StringDraw("Make Selection                        ", 0, 0, 15);
			RIT128x96x4StringDraw("  Blood Pressure                      ", 0, 10, 15);
			RIT128x96x4StringDraw("  Temperature                         ", 0, 20, 15);
			RIT128x96x4StringDraw("  Pulse Rate                          ", 0, 30, 15);
			RIT128x96x4StringDraw("  Battery                             ", 0, 40, 15);
			RIT128x96x4StringDraw("                                      ", 0, 50, 15);
			RIT128x96x4StringDraw("                                      ", 0, 60, 15);
			RIT128x96x4StringDraw("->", 0, 10*((scroll%4+1)), 15);
		}
		else
		{
		
			if(0 == scroll%4)
				buf1 = "Blood Pressure:";
			else if(1 == scroll%4)
				buf1 = "Temperature:";
			else if(2 == scroll%4)
				buf1 = "Pulse Rate:";
			else if(3 == scroll%4)
				buf1 = " Battery:";
			else buf1 = "oops"; //just in case
			RIT128x96x4StringDraw("                                      ", 0, 0, 15);
			RIT128x96x4StringDraw(buf1, 0, 0, 15);
			
			if(0 == scroll%4)
				usprintf(buf2,30, "Systolic: %.0f mm Hg ", (*(int*)cbGet(dData->systolicPressCorrected)));
			else if(1 == scroll%4)
				usprintf(buf2,30,"%.2f C ", (*(int*)cbGet(dData->temperatureCorrected)));
			else if(2 == scroll%4)
				usprintf(buf2,30, "%d BPM ", (*(int*)cbGet(dData->pulseRateCorrected)));
			else if(3 == scroll%4)
				usprintf(buf2,30, "%d %%  ", (*(dData->batteryState))/2);
			else buf2 = "oops"; //just in case			
			
			RIT128x96x4StringDraw("                                      ", 0, 10, 15);
			RIT128x96x4StringDraw(buf2, 0, 10, 15);
			if(1 == scroll%4)
				usprintf(buf2,30, "Diastolic: %.0f mm Hg        ", (*(int*)cbGet(dData->diastolicPressCorrected)));
			else buf2 = "                                                             ";
			RIT128x96x4StringDraw(buf2, 0, 20, 15);
			RIT128x96x4StringDraw("                                      ", 0, 30, 15);
			RIT128x96x4StringDraw("                                      ", 0, 40, 15);
			RIT128x96x4StringDraw("                                      ", 0, 50, 15);
			RIT128x96x4StringDraw("                                      ", 0, 60, 15);
		
		}
	}
	  
	else//(1 == *(data->mode)
	{
	  usprintf(num,40,"Temperature: %.2f C           ", (*(int*)cbGet(dData->temperatureCorrected)));
	  RIT128x96x4StringDraw(num, 0, 0, 15);

	  usprintf(num,40, "Systolic Pressure:             ");
	  RIT128x96x4StringDraw(num, 0, 10, 15);

	  usprintf(num,40, "%.0f mm Hg                   ", (*(int*)cbGet(dData->systolicPressCorrected)));
	  RIT128x96x4StringDraw(num, 0, 20, 15);

	  usprintf(num,40, "Diastolic Pressure:            ");
	  RIT128x96x4StringDraw(num, 0, 30, 15);

	  usprintf(num,40, "%.0f mm Hg                      ", (*(int*)cbGet(dData->diastolicPressCorrected)));
	  RIT128x96x4StringDraw(num, 0, 40, 15);

	  usprintf(num,40, "Pulse rate: %d BPM              ", (*(int*)cbGet(dData->pulseRateCorrected)));
	  RIT128x96x4StringDraw(num, 0, 50, 15);

	  usprintf(num,40, "Battery: %d %%                 ", *(dData->batteryState)/2);
	  RIT128x96x4StringDraw(num,0, 60,15);
	}
  //  }
}
