/* 
 * measure.h
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Implements measure.c
 */

#include "task.h"
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
  int *tRawBuf;
  int *tRawHead;

  int *bpRawBuf;
  int *bpRawSysHead;
  int *bpRawDiaHead;

  int *prRawBuf;
  int *prRawHead;
} MeasureData;

static MeasureData data;  // internal data
TCB measureTask = {&measureRunFunction, &data};  // task interface
  
void initializeMeasureTask() {
#if DEBUG
  RIT128x96x4Init(1000000);
#endif
  // Load data memory
  data.tRawBuf = &(global.tRawBuf[0]);
  data.tRawHead = &(global.tRawHead);

  data.bpRawBuf = &(global.bpRawBuf[0]);
  data.bpRawSysHead = &(global.bpRawSysHead);
  data.bpRawDiaHead = &(global.bpRawDiaHead);

  data.prRawBuf = &(global.prRawBuf[0]);
  data.prRawHead = &(global.prRawHead);
}

void setTemp(int *temp) {
    static unsigned int i = 0;
    static tBoolean goingUp = true;

    if (*temp > 50)
      goingUp = false;
    else if (*temp < 15)
      goingUp = true;

    if (goingUp) {
        if (i%2==0) (*temp)+=2;
        else (*temp)--;
    }
    else {
        if (i%2==0) (*temp)-=2;
        else (*temp)++;
    }
    
    i++;
}

void setSysPress(int *syspress) {
    // This is written to lab spec, with a flag to indicate "complete".
    // Right now, it does nothing, but I imagine it should probably be a global
    // variable to indicate to the compute task that the pressure measurement 
    // is ready, since this measurement takes a nontrivial amount of time
   
    static unsigned int i = 0;

    tBoolean complete = false;    
    
    if (*syspress > 100) {
      complete = true;
      *syspress = SYS_RAW_INIT;
    }

    if (i%2==0) (*syspress)+=3;
    else (*syspress)--;
    
    i++;
}

void setDiaPress(int *diapress) {
    static unsigned int i = 0;

    tBoolean complete = false;
    if (*diapress < 40) {
      complete = true;
      *diapress = DIA_RAW_INIT;
    }
    
    if (i%2==0) (*diapress)-=2;
    else (*diapress)++;
    
    i++;
}

void setPulse(int *pulse) {
    static unsigned int i = 0;

    static tBoolean goingUp = true;

    if (*pulse < 15)
      goingUp = true;
    else if (*pulse > 40)
      goingUp = false;

    if (goingUp) {
      if (i%2 == 0) (*pulse)--;
      else (*pulse)+=3;
    }
    else {
      if (i%2 == 0) (*pulse)++;
      else (*pulse)-=3;
    }
    
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

    setTemp(data);
    setSysPress(data);
    setDiaPress(data);
    setPulse(data);
    
#if DEBUG
    char num[30];
    sprintf(num, "Raw temp: %d", *(data->temperatureRaw));
    RIT128x96x4StringDraw(num, 0, 0, 15);
    
    sprintf(num, "Raw Syst: %d", *(data->systolicPressRaw));
    RIT128x96x4StringDraw(num, 0, 10, 15);
    
    sprintf(num, "Raw Dia: %d", *(data->diastolicPressRaw));
    RIT128x96x4StringDraw(num, 0, 20, 15);
    
    sprintf(num, "Raw Pulse: %d", *(data->pulseRateRaw));
    RIT128x96x4StringDraw(num, 0, 30, 15);
#endif
  }
}
