// This file contains basic definitions for the Dr.at your fingers program

// Patrick Ma, Jonathan Ellington
// January 25, 2014

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

GlobalData globalDataMem;

void initializeGlobals() {
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
