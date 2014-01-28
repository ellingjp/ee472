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

StatusData sData;			// the internal data
void *statusData = (void *) &sData;	// sets the external ptr to the data;

/* Initialize the StatusData task values */
void initializeStatusData (void *data) {
	StatusData *data = (StatusData *) data;
	data->batteryState = &(globalDataMem.batteryState)
}

/* Perform status tasks */
void statusTask(void *data){
	StatusData *sData = (StatusData *) data;
	*(sData->batteryStatus) = *(sData->batteryStatus) - 1;
}
