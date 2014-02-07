/* 
 * globals.c
 * Author(s): Jonathan Ellington, Patrick Ma
 * 1/28/2014
 *
 * Defines global data for tasks to access
 */
#include "globals.h"

GlobalData global;

void initializeGlobalData() {
  global.tRawHead = 0;
  global.tRawBuf[global.tRawHead] = 75;

  global.bpRawSysHead = 0;
  global.pbRawDiaHead = 8;
  global.bpRawBuf[global.bpRawSysHead] = 80;
  global.bpRawBuf[global.bpRawDiaHead] = 80;

  global.prRawHead = 0;
  global.prRawBuf[global.prRawHead] = 50;

  global.batteryState = 200;
  global.mode = 0;
  //global.measurementSelection = 0; removed from lab
  global.alarmAcknowledge = false;
  global.scroll = 0;
}
