/* 
 * display.h
 * Author(s): Jarrett Gaddy, Jonathan Ellington
 * 1/28/2014, updated 2/3/2014
 *
 * Defines the interface for the measureTask.  
 * initializeMeasureData() should be called before running measureTask()
 */

#include "task.h"

/* Initialize MeasureData, must be done before running measureTask() */
void initializeDisplayTask();

/* Points to the TCB for measure */
extern TCB displayTask;
