/*
 * compute.h
 * Author(s): PatrickMa
 * 1/28/2014
 *
 * Defines the public interface for computeTask
 * initializeComputeData() should be called before running computeTask()
 */

<<<<<<< HEAD
#include "task.h"
=======
/* Points to data used by compute */
extern void *computeData;
>>>>>>> a6bd9b48cfc3011aaf42afba151673c7e5cc9c86

/*
 * Initializes the compute function. Should only be called once at the
 * beginning (i.e. at startup).
 */
<<<<<<< HEAD
void initializeComputeTask();

/* Points to the TCB for compute */
extern TCB computeTask;
=======
void initializeComputeTask(void *computeData);

/* Carry out the compute task */
void computeTask(void *computeDataPtr);
>>>>>>> a6bd9b48cfc3011aaf42afba151673c7e5cc9c86
