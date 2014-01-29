/* 
 * measure.h
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Defines the interface for the measureTask.  
 * initializeMeasureData() should be called before running measureTask()
 */

/* Points to the data used by measure */
extern void *measureData;

/* Initialize MeasureData, must be done before running measureTask() */
void initializeMeasureTask(void *measureData);

/* Perform the measure task */
void measureTask(void *dataptr);