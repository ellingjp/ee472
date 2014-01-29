/*
 * status.c
 * Author(s): PatrickMa
 * 1/28/2014
 *
 * implements status.h
 */

#include "status.h"
#include "globals.h"

// StatusData structure internal to compute task
typedef struct {
	int *batteryState;
} StatusData;

static StatusData data;			// the internal data
void *statusData = (void *) &data;	// sets the external ptr to the data;

/* Initialize the StatusData task values */
void initializeStatusData (void *data) {
	StatusData *sdata = (StatusData *) data;
	sdata->batteryState = &(globalDataMem.batteryState);
}

/* Perform status tasks */
void statusTask(void *data){
	StatusData *sData = (StatusData *) data;
	*(sData->batteryState) = *(sData->batteryState) - 1;
}
