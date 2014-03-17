/*
    FreeRTOS V7.0.1 - Copyright (C) 2011 Real Time Engineers Ltd.
	

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution and has been modified to 
    demonstrate three simple tasks running.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/


/*
 * Creates all the application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the standard demo application tasks.
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "OLED" task - the OLED task is a 'gatekeeper' task.  It is the only task that
 * is permitted to access the display directly.  Other tasks wishing to write a
 * message to the OLED send the message on a queue to the OLED task instead of
 * accessing the OLED themselves.  The OLED task just blocks on the queue waiting
 * for messages - waking and displaying the messages as they arrive.
 *
 * "Check" hook -  This only executes every five seconds from the tick hook.
 * Its main function is to check that all the standard demo tasks are still
 * operational.  Should any unexpected behaviour within a demo task be discovered
 * the tick hook will write an error to the OLED (via the OLED task).  If all the
 * demo tasks are executing with their expected behaviour then the check task
 * writes PASS to the OLED (again via the OLED task), as described above.
 *
 * "uIP" task -  This is the task that handles the uIP stack.  All TCP/IP
 * processing is performed in this task.
 */




/*************************************************************************
 * Please ensure to read http://www.freertos.org/portlm3sx965.html
 * which provides information on configuring and running this demo for the
 * various Luminary Micro EKs.
 *************************************************************************/

/* Set the following option to 1 to include the WEB server in the build.  By
default the WEB server is excluded to keep the compiled code size under the 32K
limit imposed by the KickStart version of the IAR compiler.  The graphics
libraries take up a lot of ROM space, hence including the graphics libraries
and the TCP/IP stack together cannot be accommodated with the 32K size limit. */

//  set this value to non 0 to include the web server

#define mainINCLUDE_WEB_SERVER		0


/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Hardware library includes. */
#include "hw_memmap.h"
#include "hw_types.h"
#include "hw_sysctl.h"
#include "sysctl.h"
#include "gpio.h"
// #include "grlib.h"
#include "rit128x96x4.h"

/* Demo app includes. */
// #include "lcd_message.h"
// #include "bitmap.h"


#include "timebase.h"
#include "schedule.h"

/* Tasks */
#include "tcb.h"
#include "startup.h"
#include "measure.h"
#include "compute.h"
#include "display.h"
#include "warning.h"
#include "keypad.h"
#include "serial.h"
#include "status.h"
#include "keyPad.h"
#include "ekgCapture.h"

/*-----------------------------------------------------------*/

/* 
  The time between cycles of the 'check' functionality (defined within the
  tick hook. 
*/
#define mainCHECK_DELAY	( ( portTickType ) 5000 / portTICK_RATE_MS )

// Size of the stack allocated to the uIP task.
#define mainBASIC_WEB_STACK_SIZE            ( configMINIMAL_STACK_SIZE * 3 )

// The OLED task uses the sprintf function so requires a little more stack too.
#define mainOLED_TASK_STACK_SIZE	    ( configMINIMAL_STACK_SIZE + 50 )

//  Task priorities.
#define mainQUEUE_POLL_PRIORITY		    ( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY		    ( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY		    ( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY		    ( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3 )
#define mainINTEGER_TASK_PRIORITY           ( tskIDLE_PRIORITY )
#define mainGEN_QUEUE_TASK_PRIORITY	    ( tskIDLE_PRIORITY )


//  The maximum number of messages that can be waiting for display at any one time.
  #define mainOLED_QUEUE_SIZE					( 3 )

// Dimensions the buffer into which the jitter time is written. 
  #define mainMAX_MSG_LEN						25

/* 
  The period of the system clock in nano seconds.  This is used to calculate
  the jitter time in nano seconds. 
*/

#define mainNS_PER_CLOCK ( ( unsigned portLONG ) ( ( 1.0 / ( double ) configCPU_CLOCK_HZ ) * 1000000000.0 ) )


// Constants used when writing strings to the display.

#define mainCHARACTER_HEIGHT		    ( 9 )
#define mainMAX_ROWS_128		    ( mainCHARACTER_HEIGHT * 14 )
#define mainMAX_ROWS_96			    ( mainCHARACTER_HEIGHT * 10 )
#define mainMAX_ROWS_64			    ( mainCHARACTER_HEIGHT * 7 )
#define mainFULL_SCALE			    ( 15 )
#define ulSSI_FREQUENCY			    ( 3500000UL )

/*-----------------------------------------------------------*/

/*
 * Configure the hardware .
 */
static void prvSetupHardware( void );

/*
 * Hook functions that can get called by the kernel.
 */
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName );
void vApplicationTickHook( void );

/* The tasks */
void measure(void *vParameters);
void compute(void *vParameters);
void display(void *vParameters);
void keyPad(void *vParameters);
void warning(void *vParameters);
void serial(void *vParameters);
void status(void *vParameters);
void ekgCapture(void *vParameters);

/*-----------------------------------------------------------*/

/* 
  The queue used to send messages to the OLED task. 
*/
xQueueHandle xOLEDQueue;

/*-----------------------------------------------------------*/

xTaskHandle computeHandle;
xTaskHandle serialHandle;
xTaskHandle measureHandle;
xTaskHandle displayHandle;
xTaskHandle ekgCaptureHandle;
xTaskHandle ekgProcessHandle;

/*************************************************************************
 * Please ensure to read http://www.freertos.org/portlm3sx965.html
 * which provides information on configuring and running this demo for the
 * various Luminary Micro EKs.
 *************************************************************************/

int main( void )
{
    prvSetupHardware();

    /* Startup task */
    startup();
    
    /* Start the tasks */
    RIT128x96x4Init(10000000);
    xTaskCreate(measure, "measure task", 100,NULL, 2, &measureHandle);
    xTaskCreate(compute, "compute task", 100,NULL, 3, &computeHandle);
    xTaskCreate(display, "display task", 100,NULL, 5, &displayHandle);
    xTaskCreate(keyPad, "keyPad task", 100,NULL, 4,NULL);
    xTaskCreate(warning, "warning task", 100,NULL, 5,NULL);
    xTaskCreate(serial, "serial task", 100,NULL, 5,&serialHandle);
    xTaskCreate(status, "status task", 100,NULL, 1,NULL);
 //   xTaskCreate(ekgCapture, "ekgCapture task", 100, NULL, 3, &ekgCaptureHandle);

    vTaskSuspend(computeHandle);
    vTaskSuspend(serialHandle);
    /* Start the scheduler. */
    vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle task. 
       (Created by vTaskStartScheduler())*/
    return 0;
}

/*
  three dummy tasks
*/

void measure(void *vParameters)
{
  while(1)
  {
      measureTask.runTaskFunction(measureTask.taskDataPtr);
      vTaskDelay(MINOR_CYCLE * MAJOR_CYCLE);
  }
}

void compute(void *vParameters)
{
  while(1)
  {
      computeTask.runTaskFunction(computeTask.taskDataPtr);
      vTaskDelay(MINOR_CYCLE);
  }
}

void keyPad(void *vParameters)
{
  while(1)
  {
      keyPadTask.runTaskFunction(keyPadTask.taskDataPtr);
      vTaskDelay(MINOR_CYCLE);
  }
}

void display(void *vParameters)
{
  while(1)
  {
      displayTask.runTaskFunction(displayTask.taskDataPtr);
      vTaskDelay(MINOR_CYCLE);
  }
}

void warning(void *vParameters)
{
  while(1)
  {
      warningTask.runTaskFunction(warningTask.taskDataPtr);
      vTaskDelay(MINOR_CYCLE);
  }
}

void serial(void *vParameters)
{
  while(1)
  {
      serialTask.runTaskFunction(serialTask.taskDataPtr);
      vTaskDelay(MINOR_CYCLE);
  }
}

void status(void *vParameters)
{
  while(1)
  {
      statusTask.runTaskFunction(statusTask.taskDataPtr);
      vTaskDelay(MINOR_CYCLE);
  }
}

void ekgCapture(void *vParameters)
{
  while(1)
  {
      ekgCaptureTask.runTaskFunction(ekgCaptureTask.taskDataPtr);
      vTaskDelay(MINOR_CYCLE);
  }
}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
    ( void ) pxTask;
    ( void ) pcTaskName;
  
    while( 1 );
}

/*-----------------------------------------------------------*/

void prvSetupHardware( void )
{
    /* 
      If running on Rev A2 silicon, turn the LDO voltage up to 2.75V.  This is
      a workaround to allow the PLL to operate reliably. 
    */
  
    if( DEVICE_IS_REVA2 )
    {
        SysCtlLDOSet( SYSCTL_LDO_2_75V );
    }
	
    // Set the clocking to run from the PLL at 50 MHz
    
    SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ );
    
    /* 	
      Enable Port F for Ethernet LEDs
            LED0        Bit 3   Output
            LED1        Bit 2   Output 
    */
    
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
    GPIODirModeSet( GPIO_PORTF_BASE, (GPIO_PIN_2 | GPIO_PIN_3), GPIO_DIR_MODE_HW );
    GPIOPadConfigSet( GPIO_PORTF_BASE, (GPIO_PIN_2 | GPIO_PIN_3 ), GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD );	
	
}


/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
    // static xOLEDMessage xMessage = { "PASS" };
    static unsigned portLONG ulTicksSinceLastDisplay = 0;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    /* 
      Called from every tick interrupt.  Have enough ticks passed to make it
      time to perform our health status check again? 
    */
    
    ulTicksSinceLastDisplay++;
    if( ulTicksSinceLastDisplay >= mainCHECK_DELAY )
    {
       ulTicksSinceLastDisplay = 0;
            
    }
}
