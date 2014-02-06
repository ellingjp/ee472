/* 
 * globals.h
 * Author(s): Jonathan Ellington, Patrick Ma
 * 1/28/2014
 *
 * Defines global data for tasks to access
 * MUST be initialized before using
 */

#include "hw/types.h"

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

typedef struct global_data {
  typedef struct {
    int buf[8];
    int head;
  } temperatureRaw; 

  typedef struct {
    int buf[16];
    int head;
  } bloodPressRaw;

  typedef struct {
    int buf[8];
    int head;
  } pulseRateRaw;

  typedef struct {
    float buf[8];
    int   head;
  } temperatureCorrected;

  typedef struct {
    float buf[16];
    int   head;
  } bloodPressCorrected;

  typedef struct {
    float buf[8];
    int   head;
  } prCorrected;

  typedef struct {
    int buf[8];
    int head;
  } batteryState;

  unsigned short mode;
  unsigned short measurementSelection;
  tBoolean alarmAcknowledge;
  unsigned short scroll;
} GlobalData;

extern GlobalData globalDataMem;

void initializeGlobalData();
