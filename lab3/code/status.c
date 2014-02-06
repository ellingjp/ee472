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