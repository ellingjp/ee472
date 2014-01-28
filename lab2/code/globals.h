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
