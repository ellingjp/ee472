#include "globals.h"
#include "measure.h"

#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"

#define NUM_TASKS 5

#ifdef DEBUG
 void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif


/*
* Generic declaration of the Task Control Block (TCB) struct. Contains a
* function pointer and a data pointer.
* The TCB is the basic form all tasks called by the scheduler stem from.
*/
typedef struct tcb_struct {
  void (*runTaskFunction) (void*);
  void *taskDataPtr;
} TCB;

// Static to make sure no other files can access
static MeasureData measureData;
//static ComputeData computeData;
//static DisplayData displayData;
//static WarningAlarmData warningAlarmData;
//static Status status;

static TCB taskQueue[NUM_TASKS];

void scheduleTasks();
void initializeTaskData();
void dispatchTask(TCB *task);
TCB *getNextTask();

int main(void) {
    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);
    initializeGlobals();
    initializeTaskData();
    
    // Initialize the OLED display.
    RIT128x96x4Init(1000000);
    
    scheduleTasks();
    while (1) {
       dispatchTask(getNextTask());
       delay();
    }
}

void delay() {
  for (int i = 0; i < 100; i++)
    for (int j = 0; j < 1000; j++);
}

void dispatchTask(TCB *task) {
  task->runTaskFunction(task->taskDataPtr);
}

TCB *getNextTask() {
  return &taskQueue[0];
}

void scheduleTasks() {
  taskQueue[0].runTaskFunction = &measureTask;  // measureTask() in measure.c
  taskQueue[0].taskDataPtr = &measureData;      // measureData found here
}

void initializeTaskData() {
  initializeMeasureData(&measureData);
}