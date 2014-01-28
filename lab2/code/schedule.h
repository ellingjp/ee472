/*
 * Must be called before dispatchNextTask()
 * Initializes schedule required data structures
 */
void initialize();

/*
 * Dispatch the next task in the queue
 */
void dispatchNextTask();
