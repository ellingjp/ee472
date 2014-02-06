/* 
 * serial.c
 * Author(s): Jonathan Ellington
 * 2/05/2014
 *
 * Implements serial.c
 */

#include "task.h"
#include "globals.h"
#include "timebase.h"
#include "serial.h"
#include "inc/hw_types.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include <stdio.h>
#include <string.h>

// Internal data structure
typedef struct serialData {
  float *temperatureCorrected;
  float *systolicPressCorrected;
  float *diastolicPressCorrected;
  float *pulseRateCorrected;
  int *batteryState;
} SerialData;

static SerialData data;  // internal data
TCB serialTask;          // task interface

// Prototype
void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount);
void serialRunFunction(void *dataptr);

void initializeSerialTask() {
  // Initialize Data
  data.temperatureCorrected = &(globalDataMem.temperatureCorrected);
  data.systolicPressCorrected = &(globalDataMem.systolicPressCorrected);
  data.diastolicPressCorrected = &(globalDataMem.diastolicPressCorrected);
  data.pulseRateCorrected = &(globalDataMem.pulseRateCorrected);
  data.batteryState = &(globalDataMem.batteryState);

  // TCB
  serialTask.runTaskFunction = &serialRunFunction;
  serialTask.taskDataPtr = &data;

  // UART Stuff
  // Enable the peripherals used by this example.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

  // Set GPIO A0 (UART RX)
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0);

  // Configure the UART for 115,200, 8-N-1 operation.
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
       UART_CONFIG_PAR_NONE));

  // Might need UARTEnable(UART0_BASE);
}

void serialRunFunction(void *dataptr) {
  char buf[512];
  sprintf(buf,
          "1. Temperature\t%f C\n"
          "2. Systolic pressure:\t%f mm Hg\n"
          "3. Diastolic pressure:\t%f mm Hg\n"
          "4. Pulse rate:\t%f BPM\n"
          "5. Battery:\t%d\n",
          *(data.temperatureCorrected), 
          *(data.systolicPressCorrected),
          *(data.diastolicPressCorrected), 
          *(data.pulseRateCorrected),
          *(data.batteryState));
  
  // The cast removes a warning.  It is safe as long as buf contains
  // plain ASCII (non extended)
  UARTSend( (unsigned char *) buf, strlen(buf));
}

void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount) {
  // Loop while there are more characters to send.
  while(ulCount--)
  {
    // Write the next character to the UART.
    // This blocks while the FIFO queue is full
    UARTCharPut(UART0_BASE, *pucBuffer++);
  }
}
