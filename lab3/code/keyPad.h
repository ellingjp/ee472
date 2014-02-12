/*
 * keyPad.h
 * Author(s): Jarrett Gaddy
 * 1/28/2014
 *
 * Defines the public interface for the keyPad task
 * 
 * initializeStatusTask() should be called once before performing runkeyPad
 * functions
 */

#include "task.h"  // for TCBs

/* Initialize StatusData, must be done before running functions */
void initializeKeyPadTask();

/* The keyPad Task */
extern TCB keyPadTask;
