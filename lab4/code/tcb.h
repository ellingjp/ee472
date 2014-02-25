/*
 * task.h
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Defines the interface for a task
 */

#ifndef _TASK_H
#define _TASK_H

typedef struct tcb_struct {
  void (*runTaskFunction) (void*);
  void *taskDataPtr;
} TCB;

#endif // _TASK_H
