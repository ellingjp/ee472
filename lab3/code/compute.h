/*
 * compute.h
 * Author(s): PatrickMa
 * 1/28/2014
 *
 * Defines the public interface for computeTask
 * initializeComputeData() should be called before running computeTask()
 */

#include "task.h"

/*
 * Initializes the compute function. Should only be called once at the
 * beginning (i.e. at startup).
 */
void initializeComputeTask();

/* Points to the TCB for compute */
extern TCB computeTask;