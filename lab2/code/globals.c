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
  globalDataMem.temperatureRaw = TEMP_RAW_INIT;
  globalDataMem.systolicPressRaw = SYS_RAW_INIT;
  globalDataMem.diastolicPressRaw = DIA_RAW_INIT;
  globalDataMem.pulseRateRaw = PULSE_RAW_INIT;

  globalDataMem.temperatureCorrected = TEMP_CORR_INIT;
  globalDataMem.systolicPressCorrected = SYS_CORR_INIT;
  globalDataMem.diastolicPressCorrected = DIA_CORR_INIT;
  globalDataMem.pulseRateCorrected = PULSE_RAW_INIT;

  globalDataMem.batteryState = BATT_INIT;
}
