/*
 * status.h
 * Author(s): PatrickMa
 * 1/28/2014
 *
 * Defines the public interface for the status task
 * 
 * initializeStatusTask() should be called once before performing status
 * functions
 */

/* Points to the data used by Status */
extern void *statusData;

/* Initialize StatusData, must be done before running functions */
void initializeStatusData (void *statusData);

/* Perform the status tasks */
void statusTask(void *dataPtr);
