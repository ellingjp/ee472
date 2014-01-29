/* 
 * warning.c
 * Author(s): jarrett Gaddy
 * 1/28/2014
 *
 * Implements warning.h
 */

#include "globals.h"
#include "timebase.h"
#include "warning.h"
#include "bool.h"
#include "drivers/rit128x96x4.h"
#include <stdlib.h>
#include <stdio.h>


// Internal data structure
typedef struct WarningData {
  float *temperatureCorrected;
  float *systolicPressCorrected;
  float *diastolicPressCorrected;
  float *pulseRateCorrected;
  int *batteryState;
} WarningData;

static WarningData data;                   // internal data



void *warningData = (void *)&data;  // external pointer to internal data

void initializeWarningTask(void *data) {

  
  WarningData *mdata = (WarningData *)data;
  mdata->temperatureCorrected = &(globalDataMem.temperatureCorrected);
  mdata->systolicPressCorrected = &(globalDataMem.systolicPressCorrected);
  mdata->diastolicPressCorrected = &(globalDataMem.diastolicPressCorrected);
  mdata->pulseRateCorrected = &(globalDataMem.pulseRateCorrected);
  mdata->batteryState = &(globalDataMem.batteryState);
}


void warningTask(void *dataptr) {
  // only run on major cycle
static Bool tempWarn = false;
static Bool tempAlarm = false;
static Bool sysWarn = false;
static Bool sysAlarm = false;
static Bool diaWarn = false;
static Bool diaAlarm = false;
static Bool pulseWarn = false;
static Bool pulseAlarm = false;
static Bool batteryWarn = false;
  
  if (IS_MAJOR_CYCLE) {   // on major cycle
    WarningData *data = (WarningData *) dataptr;

	float temp = *(data->temperatureCorrected);
	float sysPress = *(data->systolicPressCorrected);
	float diaPress = *(data->diastolicPressCorrected);
	float pulse = *(data->pulseRateCorrected);
	int battery = *(data->batteryState);
	
	if(temp < (TEMP_MIN*WARN_LOW) || (TEMP_MAX*WARN_HIGH))
		tempWarn = true;
	else tempWarn = false;
	
	if(temp < (TEMP_MIN*ALARM_LOW) || (TEMP_MAX*ALARM_HIGH))
		tempAlarm = true;
	else tempAlarm = false;	

	if(sysPress < (SYS_MAX*WARN_HIGH))
		sysWarn = true;
	else sysWarn = false;
	
	if(sysPress < (SYS_MAX*ALARM_HIGH))
		sysAlarm = true;
	else sysAlarm = false;	

	if(diaPress < (DIA_MAX*WARN_HIGH))
		diaWarn = true;
	else diaWarn = false;
	
	if(diaPress < (DIA_MAX*ALARM_HIGH))
		diaAlarm = true;
	else diaAlarm = false;	

	if(pulse < (PULSE_MIN*WARN_LOW) || (PULSE_MAX*WARN_HIGH))
		pulseWarn = true;
	else pulseWarn = false;
	
	if(pulse < (PULSE_MIN*ALARM_LOW) || (PULSE_MAX*ALARM_HIGH))
		pulseAlarm = true;
	else pulseAlarm = false;	

	if(battery < (BATTERY_MIN))
		batteryWarn = true;
	else batteryWarn = false;
	
	
  }
	if( true == pulseWarn)
	{
		//led on 1 sec off 1 sec
	}
	if( true == pulseAlarm)
	{
		//sound
	}
	if( true == tempWarn)
	{
		//led on .5 sec off .5 sec
	}
	if( true == tempAlarm)
	{
		//sound
	}
	if( true == sysWarn)
	{
		//led on .25 sec off .25 sec
	}
	if( true == sysAlarm)
	{
		//sound
	}
	if( true == diaWarn)
	{
		//led on .25 sec off .25 sec
	}
	if( true == diaAlarm)
	{
		//sound
	}
	if(true == batteryWarn)
	{
		//flash battery Warn LED
	}

	  
	  
  
}
