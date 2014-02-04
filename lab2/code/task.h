/* 
 * task.h
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Defines the interface for a task
 */

typedef struct tcb_struct {
  void (*runTaskFunction) (void*);
  void *taskDataPtr;
} TCB;
