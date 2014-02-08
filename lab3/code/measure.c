/* 
 * measure.h
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Implements measure.c
 */

#include "task.h"
#include "CircularBuffer.h"
#include "globals.h"
#include "timebase.h"
#include "measure.h"
#include "inc/hw_types.h"
#include "drivers/rit128x96x4.h"

// Used for debug display
#if DEBUG
#include "drivers/rit128x96x4.h"
#include <stdlib.h>
#include <stdio.h>
#endif 

// prototype for compiler
void measureRunFunction(void *dataptr);  

// Internal data structure
typedef struct measureData {
  CircularBuffer *temperatureRaw;
  CircularBuffer *systolicPressRaw;
  CircularBuffer *diastolicPressRaw;
  CircularBuffer *pulseRateRaw;
} MeasureData;

static MeasureData data;  // internal data
TCB measureTask = {&measureRunFunction, &data};  // task interface
  
void initializeMeasureTask() {
#if DEBUG
  RIT128x96x4Init(1000000);
#endif
  // Load data memory
  data.temperatureRaw = &(global.temperatureRaw);
  data.systolicPressRaw = &(global.systolicPressRaw);
  data.diastolicPressRaw = &(global.diastolicPressRaw);
  data.pulseRateRaw = &(global.pulseRateRaw);
}

void setTemp(CircularBuffer *tbuf) {
    static unsigned int i = 0;
    static tBoolean goingUp = true;

    int temp = *(int *)cbGet(tbuf);

    if (temp > 50)
      goingUp = false;
    else if (temp < 15)
      goingUp = true;

    if (goingUp) {
        if (i%2==0) temp+=2;
        else temp--;
    }
    else {
        if (i%2==0) temp-=2;
        else temp++;
    }

    cbAdd(tbuf, &temp);
    
    i++;
}

void setSysPress(CircularBuffer *spbuf) {
    // This is written to lab spec, with a flag to indicate "complete".
    // Right now, it does nothing, but I imagine it should probably be a global
    // variable to indicate to the compute task that the pressure measurement 
    // is ready, since this measurement takes a nontrivial amount of time
   
    static unsigned int i = 0;

    tBoolean complete = false;    
    int syspress = *(int *)cbGet(spbuf);
    
    if (syspress > 100) {
      complete = true;
      syspress = SYS_RAW_INIT;
    }

    if (i%2==0) syspress+=3;
    else syspress--;

    cbAdd(spbuf, &syspress);
    
    i++;
}

void setDiaPress(CircularBuffer *dpbuf) {
    static unsigned int i = 0;

    tBoolean complete = false;
    int diapress = *(int *)cbGet(dpbuf);

    if (diapress < 40) {
      complete = true;
      diapress = DIA_RAW_INIT;
    }
    
    if (i%2==0) diapress-=2;
    else diapress++;

    cbAdd(dpbuf, &diapress);
    
    i++;
}

void setPulse(CircularBuffer *pbuf) {
    static unsigned int i = 0;

    static tBoolean goingUp = true;

    int pulse = *(int *)cbGet(pbuf);

    if (pulse < 15)
      goingUp = true;
    else if (pulse > 40)
      goingUp = false;

    if (goingUp) {
      if (i%2 == 0) pulse--;
      else pulse+=3;
    }
    else {
      if (i%2 == 0) pulse++;
      else pulse-=3;
    }
    
    cbAdd(pbuf, &pulse);

    i++;
}

void measureRunFunction(void *dataptr) {
  static tBoolean onFirstRun = true;

  if (onFirstRun) {
    initializeMeasureTask();
    onFirstRun = false;
  }

  // only run on major cycle
  if (IS_MAJOR_CYCLE) {   // on major cycle
    MeasureData *data = (MeasureData *) dataptr;

    setTemp(data->temperatureRaw);
    setSysPress(data->systolicPressRaw);
    setDiaPress(data->diastolicPressRaw);
    setPulse(data->pulseRateRaw);
    
#if DEBUG
    char num[30];
    sprintf(num, "Raw temp: %d", *(int *)cbGet(data->temperatureRaw));
    RIT128x96x4StringDraw(num, 0, 0, 15);
    
    sprintf(num, "Raw Syst: %d", *(int *)cbGet(data->systolicPressRaw));
    RIT128x96x4StringDraw(num, 0, 10, 15);
    
    sprintf(num, "Raw Dia: %d", *(int *)cbGet(data->diastolicPressRaw));
    RIT128x96x4StringDraw(num, 0, 20, 15);
    
    sprintf(num, "Raw Pulse: %d", *(int *)cbGet(data->pulseRateRaw));
    RIT128x96x4StringDraw(num, 0, 30, 15);
#endif
  }
}
