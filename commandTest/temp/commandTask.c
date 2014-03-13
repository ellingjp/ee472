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
 *  parses the command string for command parameters, writing those values to
 *  variables
 */
void parse(CommandData *cData) {
	char delim[2] = TOKEN_DELIM;

	strncpy(parseArr, cData->commandStr, COMMAND_LENGTH - 1);

	cmd = strtok(parseArr, delim);
	sensor = strtok(NULL, delim);

}

/*
 * writes an acknowledge or not acknowledge to the response buffer
 */
void ackNack(CommandData *cData, tBoolean stat) {
	if (stat) 
		strncpy(cData->responseStr, "A\0", 5);
	else
		strncpy(cData->responseStr, "E\0", 5);
}

/*
 * runs the command task
 */
void commandRunFunction(void *commandDataPtr) {
	if (!initialized) {
		initialized = true;
		initializeCommandTask();
	}
	CommandData *cData = (CommandData *) commandDataPtr;

#if DEBUG_COMMAND
	usnprintf(num, 30, "Initialize cmd function");
	RIT128x96x4StringDraw(num, 0, 10, 15);
#endif

	parse(cData);

#if DEBUG_COMMAND
	usnprintf(num, 30, "%s %s", cmd, sensor);
	RIT128x96x4StringDraw(num, 0, 20, 15);
#endif

	switch(*cmd) {
		case 'D' : // toggle display on/off
			if (*(cData->displayOn)) {
				vTaskSuspend(displayHandle);
				RIT128x96x4Clear();
			} else {
				vTaskResume(displayHandle);
			}
			*(cData->displayOn) = !*(cData->displayOn);
			ackNack(cData, true);
#if DEBUG_COMMAND
			usnprintf(num, 30, "%d %s", *(cData->displayOn), cData->responseStr);
			RIT128x96x4StringDraw(num, 0, 30, 15);
#endif
			break;
//		case 'S' :	// start measurements
//			vTaskResume(measureHandle);
//			vTaskResume(computeHandle);
//			vTaskResume(ekgCaptureHandle);
//			vTaskResume(ekgProcessHandle);
//
//			// enable the interrupts used for measurement
//			IntEnable(INT_GPIOA);	// for pulse
//			IntEnable(INT_ADC0SS0);	// for ekg
//			IntEnable(INT_ADC0SS1);	// for temperature
//			ackNack(cData, true);
//			break;
//		case 'P' :	// stop 
//			vTaskSuspend(measureHandle);
//			vTaskSuspend(computeHandle);
//			vTaskSuspend(ekgCaptureHandle);
//			vTaskSuspend(ekgProcessHandle);
//
//			// disable the interrupts used for measurement
//			IntDisable(INT_GPIOA);	// for pulse
//			IntDisable(INT_ADC0SS0);	// for ekg
//			IntDisable(INT_ADC0SS1);	// for temperature
//			ackNack(cData, true);
//			break;
		default :	// send error to remoteStr
			ackNack(cData, false);
#if DEBUG_COMMAND
			usnprintf(num, 30, "invalid command");
			RIT128x96x4StringDraw(num, 0, 30, 15);
#endif
	}

	vTaskSuspend(controlHandle);
}
