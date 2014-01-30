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
	if (sData->batteryState > 0)
          *(sData->batteryState) = *(sData->batteryState) - 1;
  }
}
