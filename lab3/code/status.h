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

<<<<<<< HEAD
#include "task.h"  // for TCBs

/* Initialize StatusData, must be done before running functions */
void initializeStatusTask();

/* The status Task */
extern TCB statusTask;
=======
/* Points to the data used by Status */
extern void *statusData;

/* Initialize StatusData, must be done before running functions */
void initializeStatusTask (void *statusData);

/* Perform the status tasks */
void statusTask(void *dataPtr);
>>>>>>> a6bd9b48cfc3011aaf42afba151673c7e5cc9c86
