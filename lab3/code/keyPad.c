/*
 * status.c
 * Author(s): PatrickMa
 * 1/28/2014
 *
 * implements status.h
 */

#include "status.h"
#include "globals.h"
#include "timebase.h"
#include "Boolean.h"

// StatusData structure internal to compute task
typedef struct {
  unsigned short *mode;
  unsigned short *measurementSelection;
  unsigned short *scroll;
  Boolean alarmAcknowledge;
  Boolean select;
} KeyPadData;

void keyPadRunFunction(void *data);  // prototype for compiler

static KeyPadData kdata;  // the internal data
TCB keyPadTask = {&keyPadRunFunction, &data}; // task interface

/* Initialize the StatusData task values */
void initializeKeyPadTask() {

	 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); 

	 GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA,
     GPIO_PIN_TYPE_STD_WPU);
	 GPIODirModeSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_DIR_MODE_IN);



  // Load data
  kdata.mode = &(global.mode);
  kdata.measurementSelection = &(global.measurementSelection);  
  kdata.scroll = &(global.scroll);
  kdata.alarmAcknowledge = &(global.alarmAcknowledge);
  kdata.select = &(global.select);

  // Load TCB
  keyPadTask.runTaskFunction = &keyPadRunFunction;
  keyPadTask.taskDataPtr = &data;
}

/* Perform status tasks */
void keyPadRunFunction(void *data){
 
  if (IS_MAJOR_CYCLE) {//change this to every 2 seconds
	if( 0 == *(kdata->mode))
	{
		if(LEFT_SW)
		{
			*(kdata->mode) = 1;
		}
		if(false == *(kdata->select)
		{
			if(UP_SW)
				*(kdata->scroll) = *(kdata->scroll) +1;
			else if(DOWN_SW)
				*(kdata->scroll) = *(kdata->scroll) -1;
			else if(RIGHT_SW)
				*(kdata->select) = true;
			
		}
		else
		{
			*(kdata->select) = false;
		}
	}
	else
	{
		if(RIGHT_SW)
		{
			*(kdata->mode) = 0;
		}
	}
	if( 1 == ACK_SW)
	{
		*(kdata->alarmAcknowledge) = true;
	}
	else
		*(kdata->alarmAcknowledge) = false;
	
	
	
  }
}