/*
 * serial.h
 * Author(s): Jonathan Ellington
 * 2/05/2014
 *
 * Defines the serial communications task.  This sends various data over
 * RS-232.  
 * 
 * This function should only run if there is a warning (in other words, the
 * scheduler should add it to the task queue in the event of a warning), and
 * should subsequently delete itself from the task queue.  
 */

#include "tcb.h"  // for TCBs

/* The status Task */
extern TCB serialTask;
