/* 
 * schedule.h
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Defines the scheduler interface.  The scheduler
 * is responsible for running tasks on a specified
 * schedule.
 */

/*
 * Must be called before dispatchNextTask()
 * Initializes schedule required data structures
 */
void initialize();


/* Dispatch the next task in the queue */
void dispatchNextTask();
