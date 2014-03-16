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
#include "measure.h"
#endif 

#define TOKEN_DELIM	" \t"	// token delimiter values
#define TEMP_BUFFER_LEN 40

// compiler prototypes
void commandRunFunction(void *commandDataPtr);

// internal command data structure
typedef struct commandData
{
	char *commandStr;
	char *responseStr;
	unsigned short *measureSelect;
	tBoolean *measureComplete;
} CommandData;

static CommandData data; // version of data exposed to outside
TCB commandTask = {&commandRunFunction, &data}; // set up task interface

static tBoolean initialized = false;
static tBoolean measureOn;
static tBoolean displayOn = true;
static char temporaryBuffer[TEMP_BUFFER_LEN];	// for formatting single responses
static char *cmd;
static char *sensor;
static char parseArr[COMMAND_LENGTH];


/*
 * initializes task variables
 */
void initializeCommandTask(){
	data.commandStr = (global.commandStr);
	data.responseStr = (global.responseStr);
	data.measureSelect = &(global.measurementSelection);
	data.measureComplete = &(global.measurementComplete);
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
 * Adds an acknowledge or not acknowledge to the response buffer
 */
void ackNack(CommandData *cData, tBoolean stat) {
	if (stat) 
		strncat(cData->responseStr, "<p>A</p>", RESPONSE_LENGTH - 1);
	else
		strncat(cData->responseStr, "<p>E</p>", RESPONSE_LENGTH - 1);
}

/*
 * Formats given string with appropriate html tags. Returns pointer to
 * formmated string. If statusOK is false, <blink> or </blink> flags are added.
 *
 * CAUTION! addTags does not guard against buffer overflow. Make sure your
 * string is not too long. The tags add up to 22 characters.
 */
char* addTags(char* string, tBoolean statusOK) {
	strncpy(temporaryBuffer, "<p>", 3); // first tag

	if (!statusOK) {
		strcat(temporaryBuffer, "<blink>");	// include warning maybe
	}	
	strcat(temporaryBuffer, string);
	if (!statusOK) {
		strcat(temporaryBuffer, "</blink>");
	}
	strcat(temporaryBuffer, "</p>");

	return temporaryBuffer;
}

/*
 * measures the data from a sensor
 */
void measureFromSensor(CommandData* cData) {
	tBoolean meas = true;
	switch (*sensor) {
		case 'A' :
#if DEBUG_COMMAND
			RIT128x96x4StringDraw("take A", 2, 30, 15);
#endif
			*(cData->measureSelect) = 0;
			break;
		case 'T':
#if DEBUG_COMMAND
			RIT128x96x4StringDraw("take T", 2, 30, 15);
#endif
			*(cData->measureSelect) = 1;
			break;
		case 'B':
#if DEBUG_COMMAND
			RIT128x96x4StringDraw("take B", 2, 30, 15);
#endif
			*(cData->measureSelect) = 2;
			break;
		case 'P' :
#if DEBUG_COMMAND
			RIT128x96x4StringDraw("take P", 2, 30, 15);
#endif
			*(cData->measureSelect) = 3;
			break;
		case 'E' :
#if DEBUG_COMMAND
			RIT128x96x4StringDraw("take E", 2, 30, 15);
#endif
			*(cData->measureSelect) = 4;
			break;
		case 'S' :
#if DEBUG_COMMAND
			RIT128x96x4StringDraw("unsupported", 0, 30, 15);
#endif
			*(cData->measureSelect) = '%';
			break;
		default :
#if DEBUG_COMMAND
			RIT128x96x4StringDraw("invalid command", 0, 30, 15);
#endif
			meas = false;
	}

	if (meas) {
		ackNack(cData, true);
//		vTaskResume(measureHandle);
#if DEBUG_COMMAND
		measureTask.runTaskFunction(measureTask.taskDataPtr); 
		RIT128x96x4StringDraw("MeasureTask go!", 0, 40, 15);
#endif
	} else {
#if DEBUG_COMMAND
		RIT128x96x4StringDraw("no Measure", 0, 40, 15);
#endif
		ackNack(cData, false);
	}
/* See Peckol notes for structure revision
	while (!(cData->measureComplete)) {	// TODO better to suspend self?
	}
*/
	
}

/*
 * runs the command task
 *
 * NB: the response string needs html formatting:
 * ex: <p> Temperature: 50 C </p> <p> <blink> Blood Pressure: 120 </blink> </p>
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

	memset(cData->responseStr, '\0', RESPONSE_LENGTH);
	switch(*cmd) {
		case 'D' : // toggle display on/off
			if (displayOn) {
				//				vTaskSuspend(displayHandle);
				RIT128x96x4Clear();
			} else {
				//				vTaskResume(displayHandle);
			}
			displayOn = !displayOn;
			ackNack(cData, true);
			
#if DEBUG_COMMAND
			usnprintf(num, 30, "%d %s", displayOn, cData->responseStr);
			RIT128x96x4StringDraw(num, 0, 30, 15);
#endif
			break;
		case 'S' :	// start measurements
			//			vTaskResume(measureHandle);
			//			vTaskResume(computeHandle);
			//			vTaskResume(ekgCaptureHandle);
			//			vTaskResume(ekgProcessHandle);

			// enable the interrupts used for measurement
			//			IntEnable(INT_GPIOA);	// for pulse
			//			IntEnable(INT_ADC0SS0);	// for ekg
			//			IntEnable(INT_ADC0SS1);	// for temperature
			measureOn = true;
			ackNack(cData, true);
			break;
		case 'P' :	// stop 
			//			vTaskSuspend(measureHandle);
			//			vTaskSuspend(computeHandle);
			//			vTaskSuspend(ekgCaptureHandle);
			//			vTaskSuspend(ekgProcessHandle);

			// disable the interrupts used for measurement
			//			IntDisable(INT_GPIOA);	// for pulse
			//			IntDisable(INT_ADC0SS0);	// for ekg
			//			IntDisable(INT_ADC0SS1);	// for temperature
			measureOn = false;
			ackNack(cData, true);
			break;
		case 'M' : // measure a sensor
			measureFromSensor(cData);
			break;
		case 'G' :	// Commands for DEBUG mode
			switch (*sensor) { 
				case 'D' :
					ackNack(cData, true);
					if (displayOn)
						strncat(cData->responseStr, "<p>On</p>", RESPONSE_LENGTH - 1);
					else
						strncat(cData->responseStr, "<p>Off</p>", RESPONSE_LENGTH - 1);

#if DEBUG_COMMAND
			usnprintf(num, 30, "%d %s", displayOn, cData->responseStr);
			RIT128x96x4StringDraw(num, 0, 30, 15);
#endif
					break;
				case 'M' :
					ackNack(cData, true);
					addTags((measureOn ? "ON" : "OFF"), false); //TODO fix the false vlaue
					strncat(cData->responseStr, temporaryBuffer, RESPONSE_LENGTH - 1);

#if DEBUG_COMMAND
					usnprintf(num, 30, "%d %s", measureOn, cData->responseStr);
					RIT128x96x4StringDraw(num, 0, 30, 15);
#endif
					break;
			}
			ackNack(cData, false);
			break;
		default :	// send error to remoteStr
			ackNack(cData, false);
#if DEBUG_COMMAND
			usnprintf(num, 30, "invalid command");
			RIT128x96x4StringDraw(num, 0, 30, 15);
#endif
	}

	//	vTaskSuspend(controlHandle);
}
