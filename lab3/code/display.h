/* 
 * display.h
 * Author(s): Jarrett Gaddy
 * 1/28/2014, updated 2/10/2014
 *
 * Defines the interface for the displayTask.  
 * initializeDisplayData() should be called before running displayTask()
 */

#include "task.h"

/* Initialize DisplayData, must be done before running displayTask() */
void initializeDisplayTask();

/* Points to the TCB for display */
extern TCB displayTask;
