/* 
 * measure.h
 * Author(s): Jarrett Gaddy
 * 1/28/2014
 *
 * Defines the interface for the oledDisplay task  
 * initializeDisplayData() should be called before running oleddisplayTask()
 */

/* Points to the data used by oledDisplay */
extern void *displayData;

/* Initialize displayData, must be done before running oledDisplayTask() */
void initializeDisplayData(void *displayData);

/* Perform the oledDisplay task */
void oledDisplayTask(void *dataptr);