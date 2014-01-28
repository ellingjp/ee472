/* 
 * schedule.c
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Implements schedule.h
 */

#include "schedule.h"
#include "globals.h"

// Each task include
#include "measure.h"
//#include "compute.h"
//#include "display.h"
//#include "warning.h"
//#include "status.h"

#define NUM_TASKS 5

// TCB
typedef struct tcb_struct {
  void (*runTaskFunction) (void*);
  void *taskDataPtr;
} TCB;

// The taskQueue holding TCB for each task
static TCB taskQueue[NUM_TASKS];

// Initialize datastructures
void initialize() {
  initializeGlobalData();   // from globals.h

  // Initialize each task data
  initializeMeasureData(&measureData);  // from measure.h
  //initializeComputeData()  // not implemented yet

  // schedule each task
  initializeQueue();
}

// Dispatch the next task, as defined
// by getNextTask()
// SHOULD INCLUDE A DELAY!
void dispatchNextTask() {
  TCB *task = getNextTask;
  task->runTaskFunction(task->taskDataPtr);
}

// Get the next task in the queue.
// Implemented as a circular queue
TCB *getNextTask() {
  delay();

  static unsigned int i = 0;   // static to keep track across calls

  // take modulo, then increment i
  return &taskQueue[i++ % NUM_TASKS];
}

// Initialize the taskQueue with each task
void initializeQueue() {
  // Measure Task
  taskQueue[0].runTaskFunction = measureTask; // from measure.h
  taskQueue[0].taskDataPtr = measureData;     // from measure.h

  // Compute Task (not yet implemented)
  // taskQueue[1].runTaskFunction = computeTask; // from compute.h
  // taskQueue[1].taskDataPtr = computeData;     // from compute.h
}

// Software delay
void delay() {
  for (int i = 0; i < 100; i++)
    for (int j = 0; j < 1000; j++);
}
