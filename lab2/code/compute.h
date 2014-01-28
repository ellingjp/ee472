/*
 * compute.h
 * Author(s): PatrickMa
 * 1/28/2014
 *
 * Defines the public interface for computeTask
 * initializeComputeData() should be called before running computeTask()
 */

/* Points to data used by compute */
extern void *computeData;

/*
 * Initializes the compute function. Should only be called once at the
 * beginning (i.e. at startup).
 */
void initializeComputeTask(void *computeData);

/* Carry out the compute task */
void computeTask(void *computeDataPtr);
