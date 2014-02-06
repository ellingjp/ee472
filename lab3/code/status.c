/*
 * status.c
 * Author(s): PatrickMa
 * 1/28/2014
 *
 * implements status.h
 */

#include "status.h"
#include "globals.h"
#include "timebase.h"

// StatusData structure internal to compute task
typedef struct {
<<<<<<< HEAD
  int *batteryState;
} StatusData;

static StatusData data;  // the internal data
TCB statusTask;          // task interface

void statusRunFunction(void *data);  // prototype for compiler

/* Initialize the StatusData task values */
void initializeStatusTask() {
  // Load data
  data.batteryState = &(globalDataMem.batteryState);

  // Load TCB
  statusTask.runTaskFunction = &statusRunFunction;
  statusTask.taskDataPtr = &data;
}

/* Perform status tasks */
void statusRunFunction(void *data){
  if (IS_MAJOR_CYCLE) {
    StatusData *sData = (StatusData *) data;
      if (*(sData->batteryState) > 0)
        *(sData->batteryState) = *(sData->batteryState) - 1;
  }
}
=======
	int *batteryState;
} StatusData;

static StatusData data;			// the internal data
void *statusData = (void *) &data;	// sets the external ptr to the data;

/* Initialize the StatusData task values */
void initializeStatusTask (void *data) {
	StatusData *sdata = (StatusData *) data;
	sdata->batteryState = &(globalDataMem.batteryState);
}

/* Perform status tasks */
void statusTask(void *data){
  if (IS_MAJOR_CYCLE) {
	StatusData *sData = (StatusData *) data;
	if (*(sData->batteryState) > 0)
          *(sData->batteryState) = *(sData->batteryState) - 1;
  }
}
>>>>>>> a6bd9b48cfc3011aaf42afba151673c7e5cc9c86
