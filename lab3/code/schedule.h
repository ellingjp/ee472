/* 
 * schedule.h
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Defines the scheduler interface.  The scheduler
 * is responsible for running tasks on a specified
 * schedule.
 */

#include "inc\hw_types.h"

// globally setable variables
extern tBoolean computeActive;	
extern tBoolean remoteActive;

/*
 * Must be called before runTasks()
 * Initializes schedule required data structures
 */
void initialize();


/* Run all the tasks in the queue, delay minor cycle */
void runTasks();
