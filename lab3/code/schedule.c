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

 
unsigned int minor_cycle_ctr = 0;   // minor cycle counter
static TCB *currentTask;	// taskQueue pointers
static TCB *listHead;
static TCB *listTail;

// flags to track task states
static tBoolean computeInQueue;
extern tBoolean computeActive;	
extern tBoolean remoteActive;
static tBoolean remoteInQueue;

// Private functions
void initializeQueue();
void delay_in_ms(int ms);
void insertNode(TCB *newNode);
void deleteNode();
void updateQueue();

// Must initialize before running this function!
void runTasks() {
  while (NULL != currentTask) {
    currentTask->runTaskFunction(currentTask->taskDataPtr);
    currentTask = currentTask->nextTCB; // go to next task
  }
  updateQueue();

  // TODO figure this part out with hw delay!
  delay_in_ms(MINOR_CYCLE);
  minor_cycle_ctr = minor_cycle_ctr+1;
}

// Initialize datastructures
void initialize() {
  // Initialize global data
  initializeGlobalData();   // from globals.h
  
  computeActive = false;	// neither remote or compute task runs at start up
  remoteActive = false;

  initializeQueue(); // start up task queue with basic tasks
}

// Initialize the taskQueue with each task
void initializeQueue() {
	// listhead > measure > keyPad > display > warn > status
	listHead = measureTask; // from measure.h
	measureTask.nextTCB = keyPadTask;
	keyPadTask.nextTCB = displayTask;
	displayTask.nextTCB = &warningTask;
	warningTask.nextTCB = statusTask;
	statusTask.nextTCB = NULL;

	// backwards pointers listTail > status > ...
	listTail = &statusTask;
	statusTask.prevTCB = warningTask;
	warningTask.prevTCB = displayTask;
	displayTask.prevTCB = keyPadTask;
	keyPadTask.prevTCB = measureTask;
	measureTask.prevTCB = NULL;

	currentTask = listHead;	// and set up to start at the top
}

/* Traverse the taskQueue and insert/delete tasks based on set flags.
 * currentTask pointer is reset to the head of the list */
void updateQueue() {
  // update computeTask
  if (computeActive && !computeInQueue) {
    currentTask = measureTask;
    insertNode(computeTask);
    computeInQueue = true;
  }
  if (!computeActive && computeInQueue) {
    currentTask = computeTask;
    deleteNode();
    computeInQueue = false;
  }
  // update serialTask
  if (remoteActive && !remoteInQueue) {
    currentTask = warningTask;
    insertNode(serialTask);
    remoteInQueue = true;
  }
  if (!remoteActive && remoteInQueue) {
    currentTask = serialTask;
    deleteNode();
    remoteInQueue = false;
  }
  currentTask = listHead;
}

// inserts the given node into the list as the next node. currentTask pointer
// moves to point at the newly inserted node.
// Source: code derived in part from JD Olsen, Innovative Softwear and TA, Ltd
void insertNode(TCB *newNode) {
  if(NULL == listHead) {	// empty list
    listHead = newNode;
    listTail = newNode;
  }
  else {	// insert the newNode between two nodes
    newNode -> nextTCB = currentTask -> nextTCB;
    currentTask -> nextTCB = newNode;
    newNode -> prevTCB = currentTask;
    if (NULL == (newNode -> nextTCB)) {	// newest node is at list end
      listTail = newNode;
    } else {	// in the middle somewhere
      newNode -> nextTCB -> prevTCB = newNode;
    }
  }
  currentTask = newNode;
}

/* Removes the currentTask node from the taskQueue. Moves currentTaskTask
 * pointer to the nextTCB task in the list. */
void deleteNode() {
  if (NULL == (*currentTask).nextTCB)	// edge case: at last task
    listTail = (*currentTask).prevTCB;
  else {	// reassign pointers
    (*(*currentTask).nextTCB).prevTCB = (*currentTask).prevTCB;	
    (*(*currentTask).prevTCB).nextTCB = (*currentTask).nextTCB;
  }
  currentTask = (*currentTask).nextTCB;	// update currentTask pointer
}

// Software delay
void delay_in_ms(int ms) {
  for (volatile int i = 0; i < ms; i++)
    for (volatile int j = 0; j < 800; j++);
}
