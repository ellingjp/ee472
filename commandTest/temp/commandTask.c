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
#include <string.h>

// Used for debug display
#if DEBUG_COMMAND
	#include "drivers/rit128x96x4.h"
	#include "utils/ustdlib.h"
	char num[30];
#endif 

#define TOKEN_DELIM	" \t"	// token delimiter values

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
static char *cmd;
static char *sensor;
static char parseArr[COMMAND_LENGTH];


/*
 * initializes task variables
 */
void initializeCommandTask(){
	data.commandStr = (global.commandStr);
	data.responseStr = (global.responseStr);
	data.displayOn = &(global.displayOn);
}


/*
 *  parses the command string for command parameters, sriting those values to
 *  variables
 */
void parse() {
	char delim[2] = " ";

	strncpy(parseArr, data.commandStr, COMMAND_LENGTH - 1);

	cmd = strtok(parseArr, delim);
	sensor = strtok(NULL, delim);
	
#if DEBUG_COMMAND
	usnprintf(num, 30, strcat(cmd, sensor));
	RIT128x96x4StringDraw(num, 0, 20, 15);
#endif
}

/*
 * runs the command task
 */
void commandRunFunction(void *commandDataPtr) {
	if (!initialized) {
		initialized = true;
		initializeCommandTask();
	}

#if DEBUG_COMMAND
	usnprintf(num, 30, "Initialize cmd function");
	RIT128x96x4StringDraw(num, 0, 10, 15);
#endif

	parse();

#if DEBUG_COMMAND
	usnprintf(num, 30, strcat(cmd, sensor));
	RIT128x96x4StringDraw(num, 0, 30, 15);
#endif

	switch(*cmd) {
		case 'D' : // toggle display on/off
			*(data.displayOn) = !*(data.displayOn);
#if DEBUG_COMMAND
			usnprintf(num, 30, "%d", *(data.displayOn));
			RIT128x96x4StringDraw(num, 0, 30, 15);
#endif
			break;
		default :	// send error to remoteStr
#if DEBUG_COMMAND
			usnprintf(num, 30, "invalid command");
			RIT128x96x4StringDraw(num, 0, 30, 15);
#endif
	}
}
