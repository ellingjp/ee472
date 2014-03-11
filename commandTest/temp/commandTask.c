/*
 * commandTask.c
 * Author(s); Patrick Ma
 *
 * 3/10/2014
 *
 * Interprets the text commands from the remote connection or system, performs
 * the actions requested, and sends a reply signal.
 */

#define DEBUG_COMMAND 1

#include "globals.h"
#include "CircularBuffer.h"
#include "commandTask.h"

// Used for debug display
#if DEBUG_COMMAND
	#include "drivers/rit128x96x4.h"
	#include "utils/ustdlib.h"
#endif 

// compiler prototypes
void commandRunFunction(void *commandDataPtr);

// internal command data structure
typedef struct commandData
{
	char *commandStr;
	char *responseStr;
	tBoolean *displayOn;
} CommandData;

static CommandData data; // version of data exposed to outside
TCB commandTask = {&commandRunFunction, &data}; // set up task interface

static tBoolean initialized = false;

// initializes task variables
void initializeCommandTask(){
	data.commandStr = (global.commandStr);
	data.responseStr = (global.responseStr);
	data.displayOn = &(global.displayOn);
}

// runs the command task
void commandRunFunction(void *commandDataPtr) {
	if (!initialized) {
		initialized = true;
		initializeCommandTask();
	}

#if DEBUG_COMMAND
	char num[30];
	usnprintf(num, 30, "Initialize command function");
	RIT128x96x4StringDraw(num, 0, 10, 15);
#endif

}
