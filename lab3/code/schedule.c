/* 
 * schedule.c
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Implements schedule.h
 */

#include "task.h"
#include "schedule.h"
#include "timebase.h"
#include "globals.h"

// Each task include
#include "measure.h"
#include "compute.h"
#include "display.h"
#include "warning.h"
#include "status.h"
#include "serial.h"

#define NUM_TASKS 4

static TCB taskQueue[NUM_TASKS];    // The taskQueue holding TCB for each task
unsigned int minor_cycle_ctr = 0;   // minor cycle counter

// Private functions
TCB *getNextTask();
void initializeQueue();
void delay_in_ms(int ms);

// Must initialize before running this function!
void runTasks() {
  for (int i = 0; i < NUM_TASKS; i++) {
    TCB *task = &taskQueue[i];
    task->runTaskFunction(task->taskDataPtr);
  }
  delay_in_ms(MINOR_CYCLE);
  minor_cycle_ctr = minor_cycle_ctr+1;
}

// Initialize datastructures
void initialize() {
  // Initialize global data
  initializeGlobalData();   // from globals.h
  
  // schedule each task
  initializeQueue();
}

// Initialize the taskQueue with each task
void initializeQueue() {
  // Load tasks
  taskQueue[0] = measureTask; // from measure.h
  taskQueue[1] = computeTask; // from compute.h
  taskQueue[2] = serialTask;
  taskQueue[3] = statusTask;
  //taskQueue[2] = displayTask; // from display.h
  //taskQueue[3] = warningTask; // from warning.h
  //taskQueue[4] = statusTask;  // from status.h
}

// Software delay
void delay_in_ms(int ms) {
  for (volatile int i = 0; i < ms; i++)
    for (volatile int j = 0; j < 800; j++);
}
