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

#include "tcb.h"  // for TCBs

/* Initialize StatusData, must be done before running functions */
void initializeStatusTask();

/* The status Task */
extern TCB statusTask;
