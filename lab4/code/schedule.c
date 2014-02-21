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
#include "keyPad.h"

// Used for debug display
#include "drivers/rit128x96x4.h"
#include "utils/ustdlib.h"


 
unsigned int minor_cycle_ctr = 0;   // minor cycle counter

static TCB *currentTaskPtr;	// taskQueue pointers
static TCB *listHeadPtr;
static TCB *listTailPtr;

// flags to track task states
tBoolean computeActive;	
tBoolean serialActive;
static tBoolean computeInQueue;
static tBoolean serialInQueue;

tBoolean runSchedule = false;


// Private functions
void delay_in_ms(int ms);
void insertNextNode(TCB *newNode);
void deleteCurrent();
void updateQueue();
void debug();

// Must initialize before running this function!
void runTasks() {
  if(runSchedule) {
  runSchedule = false;
  
  updateQueue();

  while (NULL != currentTaskPtr) {
    currentTaskPtr->runTaskFunction(currentTaskPtr->taskDataPtr);
    currentTaskPtr = currentTaskPtr->nextTCB; // go to next task
  }
  }
}


// Initialize the taskQueue with each task
void initializeQueue() {
	// listhead > measure > keyPad > display > warn > status > null
	listHeadPtr = &measureTask; // from measure.h
	measureTask.nextTCB = &keyPadTask;
	keyPadTask.nextTCB = &displayTask;
	displayTask.nextTCB = &warningTask;
	warningTask.nextTCB = &statusTask;
	statusTask.nextTCB = NULL;

	// backwards pointers listTailPtr > status > ...
	listTailPtr = &statusTask;
	statusTask.prevTCB = &warningTask;
	warningTask.prevTCB = &displayTask;
	displayTask.prevTCB = &keyPadTask;
	keyPadTask.prevTCB = &measureTask;
	measureTask.prevTCB = NULL;

	currentTaskPtr = listHeadPtr;	// and set up to start at the top
}

/* Traverse the taskQueue and insert/delete tasks based on set flags.
 * currentTaskPtr pointer is reset to the head of the list */
void updateQueue() {
  // update computeTask
  if (computeActive && !computeInQueue) {
    currentTaskPtr = &measureTask;
    insertNextNode(&computeTask);
    computeInQueue = true;
  }
  if (!computeActive && computeInQueue) {
    currentTaskPtr = &computeTask;
    deleteCurrent();
    computeInQueue = false;
  }
  // update serialTask
  if (serialActive && !serialInQueue) {
    currentTaskPtr = &warningTask;
    insertNextNode(&serialTask);
    serialInQueue = true;
  }
  if (!serialActive && serialInQueue) {
    currentTaskPtr = &serialTask;
    deleteCurrent();
    serialInQueue = false;
  }
  currentTaskPtr = listHeadPtr;
}

// inserts the given node into the list as the next node. currentTaskPtr pointer
// moves to point at the newly inserted node.
// Source: code derived in part from JD Olsen, Innovative Softwear and TA, Ltd
void insertNextNode(TCB *newNode) {
  if(NULL == listHeadPtr) {	// empty list
    listHeadPtr = newNode;
    listTailPtr = newNode;
  }
  else {	// insert the newNode between two nodes
    newNode -> nextTCB = currentTaskPtr -> nextTCB;
    currentTaskPtr -> nextTCB = newNode;
    newNode -> prevTCB = currentTaskPtr;
    if (NULL == (newNode -> nextTCB)) {	// newest node is at list end
      listTailPtr = newNode;
    } else {	// in the middle somewhere
      newNode -> nextTCB -> prevTCB = newNode;
    }
  }
  currentTaskPtr = newNode;
}

/* Removes the currentTaskPtr node from the taskQueue. 
 * Moves currentTaskPtr pointer to the nextTCB task in the list. */
void deleteCurrent() {
  if (NULL == currentTaskPtr -> nextTCB) {	// edge case: at last task
    listTailPtr = currentTaskPtr -> prevTCB;
    listTailPtr -> nextTCB = NULL;
  } else {	// reassign pointers
    currentTaskPtr -> nextTCB -> prevTCB = currentTaskPtr ->prevTCB;	
    currentTaskPtr -> prevTCB -> nextTCB = currentTaskPtr -> nextTCB;
  }
  currentTaskPtr = currentTaskPtr -> nextTCB;	// update currentTaskPtr pointer
}

// Software delay
void delay_in_ms(int ms) {
  for (volatile int i = 0; i < ms; i++)
    for (volatile int j = 0; j < 800; j++);
}

// for debug (obviously)
void debug() {
  char num[30];
    static int test = 0;
    usnprintf(num, 30, "Test number: %d  ", test);
    RIT128x96x4StringDraw(num, 0, 90, 15);
    test++;
}

