/* 
 * globals.h
 * Author(s): Jonathan Ellington, Patrick Ma
 * 1/28/2014
 *
 * Defines global data for tasks to access
 * MUST be initialized before using
 */

#include "inc/hw_types.h"
#include "CircularBuffer.h"
#include "stddef.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#define DEBUG 0

#define TEMP_RAW_INIT 80        // initial 80
#define SYS_RAW_INIT 80        // initial 50
#define DIA_RAW_INIT 50         // initial 50
#define PULSE_RAW_INIT 30       // initial 30

#define TEMP_CORR_INIT 0.0
#define SYS_CORR_INIT 0.0
#define DIA_CORR_INIT 0.0
#define PULSE_CORR_INIT 0.0

#define BATT_INIT 200

typedef struct global_data {
  CircularBuffer temperatureRaw;
  CircularBuffer systolicPressRaw;
  CircularBuffer diastolicPressRaw;
  CircularBuffer pulseRateRaw;

  CircularBuffer temperatureCorrected;
  CircularBuffer systolicPressCorrected;
  CircularBuffer diastolicPressCorrected;
  CircularBuffer pulseRateCorrected;

  unsigned short batteryState;
  unsigned short mode;
  unsigned short measurementSelection;
  tBoolean alarmAcknowledge;
  tBoolean select;
  unsigned short scroll;
} GlobalData;

extern GlobalData global;

// initializes the global variables for use by system
void initializeGlobalData();

// allows use of pin47 for debug, toggles the pin hi or lo alternatively
void debugPin47();