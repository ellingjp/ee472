/* 
 * globals.h
 * Author(s): Jonathan Ellington, Patrick Ma
 * 1/28/2014
 *
 * Defines global data for tasks to access
 * MUST be initialized before using
 */

#define DEBUG 0

#define TEMP_RAW_INIT 80        // initial 80
#define SYS_RAW_INIT 50         // initial 50
#define DIA_RAW_INIT 50         // initial 50
#define PULSE_RAW_INIT 30       // initial 30

#define TEMP_CORR_INIT 0.0
#define SYS_CORR_INIT 0.0
#define DIA_CORR_INIT 0.0
#define PULSE_CORR_INIT 0.0

#define BATT_INIT 200

<<<<<<< HEAD
typedef struct global_data {
=======
typedef struct correctedData {
>>>>>>> a6bd9b48cfc3011aaf42afba151673c7e5cc9c86
  int temperatureRaw;
  int systolicPressRaw;
  int diastolicPressRaw;
  int pulseRateRaw;
  float temperatureCorrected;
  float systolicPressCorrected;
  float diastolicPressCorrected;
  float pulseRateCorrected;
  int batteryState;
} GlobalData;

extern GlobalData globalDataMem;

void initializeGlobalData();
