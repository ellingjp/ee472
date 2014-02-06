/* 
 * measure.h
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Defines the interface for the measureTask.  
 * initializeMeasureData() should be called before running measureTask()
 */

<<<<<<< HEAD
#include "task.h"

/* Initialize MeasureData, must be done before running measureTask() */
void initializeMeasureTask();

/* Points to the TCB for measure */
extern TCB measureTask;
=======
/* Points to the data used by measure */
extern void *measureData;

/* Initialize MeasureData, must be done before running measureTask() */
void initializeMeasureTask(void *measureData);

/* Perform the measure task */
void measureTask(void *dataptr);
>>>>>>> a6bd9b48cfc3011aaf42afba151673c7e5cc9c86
