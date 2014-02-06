/* 
 * globals.c
 * Author(s): Jonathan Ellington, Patrick Ma
 * 1/28/2014
 *
 * Defines global data for tasks to access
 */
#include "globals.h"

GlobalData global;

void initializeGlobalData() {
  global.temperatureRaw.head = 0;
  global.temperatureRaw.buf[global.temperatureRaw.head] = 75;

  global.bloodPressRaw.sys_head = 0;
  global.bloodPressRaw.dia_head = 8;
  global.bloodPressRaw.buf[global.bloodPressRaw.sys_head] = 80; // sys
  global.bloodPressRaw.buf[global.bloodPressRaw.dia_head] = 80; // dia

  global.pulseRateRaw.head = 0;
  global.pulseRateRaw.buf[global.pulseRateRaw.head] = 50;

  global.batteryState = 200;
  global.mode = 0;
  global.measurementSelection = 0;
  global.alarmAcknowledge = false;
  global.scroll = 0;
}
