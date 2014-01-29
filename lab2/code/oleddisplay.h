/* 
 * measure.h
 * Author(s): Jarrett Gaddy
 * 1/28/2014
 *
 * Defines the interface for the oledDisplay task  
 * initializeDisplayData() should be called before running oleddisplayTask()
 */

/* Points to the data used by oledDisplay */
extern void *oledDisplayData;

/* Initialize displayData, must be done before running oledDisplayTask() */
void initializeDisplayTask(void *displayData);

/* Perform the oledDisplay task */
void oledDisplayTask(void *dataptr);