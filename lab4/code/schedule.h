/* 
 * schedule.h
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Defines the scheduler interface.  The scheduler
 * is responsible for running tasks on a specified
 * schedule.
 */
#include "FreeRTOS.h"
#include "task.h"

#include "inc\hw_types.h"

// globally setable variables
extern xTaskHandle computeHandle;	
extern xTaskHandle serialHandle;
extern xTaskHandle ekgCaptureHandle;
extern xTaskHandle ekgProcessHandle;


/*
 * Must be called before runTasks()
 * Initializes schedule required data structures
 */
//void initializeQueue();
