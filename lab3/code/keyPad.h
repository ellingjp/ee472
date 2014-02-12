/*
 * keyPad.h
 * Author(s): Jarrett Gaddy
 * 2/10/2014
 *
 * Defines the public interface for the keyPad task
 * 
 * initializeKeyPadTask() should be called once before performing runkeyPad
 * functions
 */

#include "task.h"  // for TCBs

/* Initialize KeyPadData, must be done before running functions */
void initializeKeyPadTask();

/* The keyPad Task */
extern TCB keyPadTask;
