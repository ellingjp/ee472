/* 
 * schedule.c
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Implements schedule.h
 */

#include "schedule.h"
#include "timebase.h"
#include "globals.h"

// Each task include
#include "measure.h"
#include "compute.h"
#include "oleddisplay.h"
//#include "warning.h"
//#include "status.h"

#define NUM_TASKS 3

// TCB
typedef struct tcb_struct {
  void (*runTaskFunction) (void*);
  void *taskDataPtr;
} TCB;

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
  minor_cycle_ctr = (minor_cycle_ctr+1) % MAJOR_CYCLE;
}

// Initialize datastructures
void initialize() {
  initializeGlobalData();   // from globals.h

  // Initialize each task data
  initializeMeasureTask(measureData);  // from measure.h
  initializeComputeTask(computeData);  // from compute.h
  initializeDisplayTask(oledDisplayData);   // from oleddisplay.h
  // schedule each task
  initializeQueue();
}

// Initialize the taskQueue with each task
void initializeQueue() {
  // Measure Task
  taskQueue[0].runTaskFunction = measureTask; // from measure.h
  taskQueue[0].taskDataPtr = measureData;     // from measure.h

  // Compute Task (not yet implemented)
  taskQueue[1].runTaskFunction = computeTask; // from compute.h
  taskQueue[1].taskDataPtr = computeData;     // from compute.h
  
   // Compute Task (not yet implemented)
  taskQueue[2].runTaskFunction = oledDisplayTask; // from compute.h
  taskQueue[2].taskDataPtr = oledDisplayData;     // from compute.h
}

// Software delay
void delay_in_ms(int ms) {
  for (volatile int i = 0; i < ms; i++)
    for (volatile int j = 0; j < 800; j++);
}