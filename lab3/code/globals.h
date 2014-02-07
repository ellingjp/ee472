/* 
 * globals.h
 * Author(s): Jonathan Ellington, Patrick Ma
 * 1/28/2014
 *
 * Defines global data for tasks to access
 * MUST be initialized before using
 */

#include "inc/hw_types.h"
#include "intCircArray.h"

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
  int tRawBuf[8];
  int tRawHead;

  int bpRawBuf[16];
  int bpRawSysHead;
  int bpRawDiaHead;

  int prRawBuf[8];
  int prRawHead;

  float tCorrBuf[8];
  int tCorrHead;

  float bpCorrBuf[16];
  int bpCorrSysHead;
  int bpCorrDiaHead;

  float prCorrBuf[8];
  int prCorrHead;

  unsigned short batteryState;
  unsigned short mode;
  //unsigned short measurementSelection; // removed from lab
  tBoolean alarmAcknowledge;
  unsigned short scroll;
} GlobalData;

extern GlobalData global;

void initializeGlobalData();
