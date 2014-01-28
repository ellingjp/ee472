/* 
 * globals.h
 * Author(s): Jonathan Ellington, Patrick Ma
 * 1/28/2014
 *
 * Defines global data for tasks to access
 * MUST be initialized before using
 */

typedef struct correctedData {
  int temperatureRaw;
  int systolicPressRaw;
  int diastolicPressRaw;
  int pulseRateRaw;
  int temperatureCorrected;
  int systolicPressCorrected;
  int diastolicPressCorrected;
  int pulseRateCorrected;
  int batteryState;
} GlobalData;

extern GlobalData globalDataMem;

void initializeGlobals();
