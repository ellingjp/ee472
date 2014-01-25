\\ This file contains basic definitions for the Dr.at your fingers program

\\Patrick Ma
\\ January 25, 2014


\*
* Generic declaration of the Task Control Block (TCB) struct. Contains a
* function pointer and a data pointer.
* The TCB is the basic form all tasks called by the scheduler stem from.
*\
typedef struct {
  void (*runTaskFunction) (void*);
  void *taskDataPtr;
} TCB;


\*
* Boolean type emulation for pre-2000 compilers
*\
enum _fakeBool {FALSE = 0, TRUE = 1};

typedef enum _fakeBool Bool;
