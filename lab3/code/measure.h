/* 
 * measure.h
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Defines the interface for the measureTask.  
 * initializeMeasureData() should be called before running measureTask()
 */

#include "task.h"

/* Initialize MeasureData, must be done before running measureTask() */
void initializeMeasureTask();

/* Points to the TCB for measure */
extern TCB measureTask;
