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
static int systolicPressureRawArr[8];
static int diastolicPressureRawArr[8];
static int pulseRateRawArr[8];

static float temperatureCorrectedArr[8];
static float systolicPressureCorrectedArr[8];
static float diastolicPressureCorrectedArr[8];
static float pulseRateCorrectedArr[8];

// For ease of use, globals.h defines pointers to
// CircularBuffers.  Since there's no dynamic memory,
// they must be defined here
static CircularBuffer temperatureRaw;
static CircularBuffer systolicPressureRaw;
static CircularBuffer diastolicPressureRaw;
static CircularBuffer pulseRateRaw;

static CircularBuffer temperatureCorrected;
static CircularBuffer systolicPressureCorrected;
static CircularBuffer diastolicPressureCorrected;
static CircularBuffer pulseRateCorrected;

void initializeGlobalData() {
  global.temperatureRaw = cbWrap(temperatureRawArr, sizeof(int), 8);
  global.systolicPressureRaw = cbWrap(systolicPressureRawArr, sizeof(int), 8);
  global.diastolicPressureRaw = cbWrap(diastolicPressureRawArr, sizeof(int), 8);
  global.pulseRateRaw = cbWrap(pulseRateRawArr, sizeof(int), 8);

  global.temperatureCorrected = cbWrap(temperatureCorrectedArr, sizeof(float), 8);
  global.systolicPressureCorrected = cbWrap(systolicPressureCorrectedArr, sizeof(float), 8);
  global.diastolicPressureCorrected = cbWrap(diastolicPressureCorrectedArr, sizeof(float), 8);
  global.pulseRateCorrected = cbWrap(pulseRateCorrectedArr, sizeof(float), 8);

  global.temperatureRaw = &temperatureRaw;
  global.systolicPressureRaw = &systolicPressureRaw;
  global.diastolicPressureRaw = &diastolicPressureRaw;
  global.pulseRateRaw = &pulseRateRaw;

  global.temperatureCorrected = &temperatureCorrected;
  global.systolicPressureCorrected = &systolicPressureCorrected;
  global.diastolicPressureCorrected = &diastolicPressureCorrected;
  global.pulseRateCorrected = &pulseRateCorrected;

  int tr = TEMP_RAW_INIT;
  int sr = SYS_RAW_INIT;
  int dr = DIA_RAW_INIT;
  int pr = PULSE_RAW_INIT;

  int tc = TEMP_CORR_INIT;
  int sc = SYS_CORR_INIT;
  int dc = DIA_CORR_INIT;
  int pc = PULSE_CORR_INIT;

  cbAdd(global.temperatureRaw, &tr);
  cbAdd(global.systolicPressureRaw, &sr);
  cbAdd(global.diastolicPressureRaw, &dr);
  cbAdd(global.pulseRateRaw, &pr);

  cbAdd(global.temperatureCorrected, &tc);
  cbAdd(global.systolicPressureCorrected, &sc);
  cbAdd(global.diastolicPressureCorrected, &dc);
  cbAdd(global.pulseRateCorrected, &pc);

  global.batteryState = 200;
  global.mode = 0;
  global.measurementSelection = 0;
  global.alarmAcknowledge = false;
  global.scroll = 0;
}
