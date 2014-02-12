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
void initializeQueue();
void delay_in_ms(int ms);
TCB *currentTask;
TCB *listHead;
TCB *listTail;
tBoolean computeActive;
tBoolean RemoteActive;

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
  
  // neither remote or compute task runs at start up
  computeActive = false;
  remoteActive = false;

  // schedule each task
  initializeQueue();
}

// Initialize the taskQueue with each task
void initializeQueue() {
	// listhead > measure > keyMonitor > display > warn > status
	listHead = &measureTask; // from measure.h
	measureTask.next = &keyMonitorTask;
	keyMonitorTask.next = &displayTask;
	displayTask.next = &warningTask;
	warningTask.next = &statusTask;
	statusTask.next = NULL;

	// set up backwards pointers listTail > status > ...
	listTail = &statusTask;
	statusTask.prevTCB = &warning;
	warningTask.prevTCB = &displayTask;
	displayTask.prevTCB = &keyMonitorTask;
	keyMonitorTask.prevTCB = &measureTask;
	monitorTask.prevTCB = NULL;

	currentTask = *listHead;	// and set up to start at the top
}

// inserts the given node into the list as the next node
void insertNode(tcb_struct* newNode) {
  if(NULL == head) {	// empty list
    head = newNode;
    tail = newNode;
    size++;
  }
  else {	// insert the newNode between two nodes
    newNode -> next = current -> next;
    current -> next = newNode;
    newNode -> prev = current;
    if (NULL == (newNode -> next)) {	// just added to end of list
      tail = newNode;
    } else {	// in the middle somewhere
      newNode -> next -> prev = newNode;
    }
  }
  size++;
}
// Software delay
void delay_in_ms(int ms) {
  for (volatile int i = 0; i < ms; i++)
    for (volatile int j = 0; j < 800; j++);
}
