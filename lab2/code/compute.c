/*
 * compute.c
 * Author(s): PatrickMa
 * 1/28/2014
 *
 * Implements compute.c
 */

#include "compute.h"
#include "globals.h"


// computeData structure internal to compute task
struct computeData {
	// raw data pointers
	int *temperatureRaw;
	int *systolicPressRaw;
	int *diastolicPressRaw;
	int *pulseRateRaw;
	
	//corrected data pointers
	float *tempCorrected;
	float *systPressCorrected;
	float *diastPressCorrected;
	float *pulseCorrected;
}

computeData cData;			// the internal data
void *computeData = (void *) &cData;   // set the external ptr to the data

/*
 * Initializes the computeData task values (pointers to variables, etc)
 */
void initializeComputeTask(void *data) {
	data->temperatureRaw = &(globalDataMem.temperatureRaw);
	data->systolicPressRaw = &(globalDataMem.systolicPressRaw);
	data->diastolicPressRaw = &(globalDataMem.diastolicPressRaw);
	data->pulseRateRaw = &(globalDataMem.pulseRateRaw);

	data->tempCorrected = &(globalDataMem.temperatureCorrected);
	data->systPressCorrected = &(globalDataMem.systolicPressCorrected);
	data->diastPressCorrected = &(globalDataMem.diastolicPressCorrected);
	data->pulseCorrected = &(globalDataMem.pulseRateCorrected);
}


/* 
 * Linearizes the raw data measurement and converts value into human 
 * readable format
 */
void computeTask(void *computeData) {
	*(computeData->temperatureCorrected) = 5 + 0.75 * (*(computeData->temperatureRaw));
	*(computeData->systPressCorrected) = 9 + 2 * (*(computeData->systolicPressRaw));
	*(computeData->diastPresscorrected) = 6 + 1.5 * (*(computeData->diastolicPressRaw));
	*(computeData->pulseCorrected) = 8 + 3 * (*(computeData->pulseRateRaw));
}


