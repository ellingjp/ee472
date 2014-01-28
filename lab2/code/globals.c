/* 
 * globals.c
 * Author(s): Jonathan Ellington, Patrick Ma
 * 1/28/2014
 *
 * Defines global data for tasks to access
 */
#include "globals.h"

GlobalData globalDataMem;

void initializeGlobalData() {
  globalDataMem.temperatureRaw = 75;
  globalDataMem.systolicPressRaw = 80;
  globalDataMem.diastolicPressRaw = 80;
  globalDataMem.pulseRateRaw = 50;

  globalDataMem.temperatureCorrected = 0;
  globalDataMem.systolicPressCorrected = 0;
  globalDataMem.diastolicPressCorrected = 0;
  globalDataMem.pulseRateCorrected = 0;

  globalDataMem.batteryState = 200;
}
