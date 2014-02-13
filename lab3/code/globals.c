/* 
 * globals.c
 * Author(s): Jonathan Ellington, Patrick Ma
 * 1/28/2014
 *
 * Defines global data for tasks to access
 */
#include "globals.h"

GlobalData global;

// The arrays to be wrapped in a 
// circular buffer
static int temperatureRawArr[8];
static int systolicPressRawArr[8];
static int diastolicPressRawArr[8];
static int pulseRateRawArr[8];

static float temperatureCorrectedArr[8];
static float systolicPressCorrectedArr[8];
static float diastolicPressCorrectedArr[8];
static float pulseRateCorrectedArr[8];

void initializeGlobalData() {
  // Wrap the arrays
  global.temperatureRaw = cbWrap(temperatureRawArr, sizeof(int), 8);
  global.systolicPressRaw = cbWrap(systolicPressRawArr, sizeof(int), 8);
  global.diastolicPressRaw = cbWrap(diastolicPressRawArr, sizeof(int), 8);
  global.pulseRateRaw = cbWrap(pulseRateRawArr, sizeof(int), 8);

  global.temperatureCorrected = cbWrap(temperatureCorrectedArr, sizeof(float), 8);
  global.systolicPressCorrected = cbWrap(systolicPressCorrectedArr, sizeof(float), 8);
  global.diastolicPressCorrected = cbWrap(diastolicPressCorrectedArr, sizeof(float), 8);
  global.pulseRateCorrected = cbWrap(pulseRateCorrectedArr, sizeof(float), 8);

  int tr = TEMP_RAW_INIT;
  int sr = SYS_RAW_INIT;
  int dr = DIA_RAW_INIT;
  int pr = PULSE_RAW_INIT;

  float tc = TEMP_CORR_INIT;
  float sc = SYS_CORR_INIT;
  float dc = DIA_CORR_INIT;
  float pc = PULSE_CORR_INIT;

  // Add initial values
  cbAdd(&(global.temperatureRaw), &tr);
  cbAdd(&(global.systolicPressRaw), &sr);
  cbAdd(&(global.diastolicPressRaw), &dr);
  cbAdd(&(global.pulseRateRaw), &pr);

  cbAdd(&(global.temperatureCorrected), &tc);
  cbAdd(&(global.systolicPressCorrected), &sc);
  cbAdd(&(global.diastolicPressCorrected), &dc);
  cbAdd(&(global.pulseRateCorrected), &pc);

  // Set normal variables
  global.batteryState = 200;
  global.mode = 0;
  global.measurementSelection = 0;
  global.alarmAcknowledge = false;
  global.select = false;
  global.scroll = 0;
}
