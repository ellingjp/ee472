/* 
 * serial.c
 * Author(s): Jonathan Ellington
 * 2/05/2014
 *
 * Implements serial.c
 */

#include "globals.h"
#include "timebase.h"
#include "serial.h"
#include "schedule.h"
#include "CircularBuffer.h"
#include "inc/hw_types.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "utils/ustdlib.h"
#include <string.h>


// Internal data structure
typedef struct serialData {
  CircularBuffer *temperatureCorrected;
  CircularBuffer *systolicPressCorrected;
  CircularBuffer *diastolicPressCorrected;
  CircularBuffer *pulseRateCorrected;
  CircularBuffer *ekgFrequencyResult;
  unsigned short *batteryState;
} SerialData;

// Prototype
void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount);
void serialRunFunction(void *dataptr);

static SerialData data;  // internal data
TCB serialTask = {&serialRunFunction, &data};          // task interface

void initializeSerialTask() {
  // Initialize Data
  data.temperatureCorrected = &(global.temperatureCorrected);
  data.systolicPressCorrected = &(global.systolicPressCorrected);
  data.diastolicPressCorrected = &(global.diastolicPressCorrected);
  data.pulseRateCorrected = &(global.pulseRateCorrected);
  data.batteryState = &(global.batteryState);
  data.ekgFrequencyResult = &(global.ekgFrequencyResult);
  // UART Stuff
  // Enable the peripherals used by this example.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

  // Set GPIO A0 (UART RX)
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_1);

  // Configure the UART for 115,200, 8-N-1 operation.
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
       UART_CONFIG_PAR_NONE));

  UARTEnable(UART0_BASE);
}

void serialRunFunction(void *dataptr) {
  static tBoolean onFirstRun = true;
  SerialData *sData = (SerialData *) dataptr;
  if (onFirstRun) {
    initializeSerialTask();
    onFirstRun = false;
  }
  
  int temp = (int) *((float *)cbGet(sData->temperatureCorrected));
  int sys = (int) *((float *)cbGet(sData->systolicPressCorrected));
  int dia = (int) *((float *)cbGet(sData->diastolicPressCorrected));
  int pulse = (int) (*(float *)cbGet(sData->pulseRateCorrected));
  int EKG = (int) *((float *)cbGet(sData->ekgFrequencyResult));
  int batt = *(data.batteryState);
  
  char buf[40];
  usnprintf(buf, 40, "\f1. Temperature:\t\t%d C\n\n\r", temp);
  UARTSend( (unsigned char *) buf, strlen(buf));

  usnprintf(buf, 40, "2. Systolic pressure:\t%d mm Hg\n\n\r", sys);
  UARTSend( (unsigned char *) buf, strlen(buf));

  usnprintf(buf, 40, "3. Diastolic pressure:\t%d mm Hg\n\n\r", dia);
  UARTSend( (unsigned char *) buf, strlen(buf));

  usnprintf(buf, 40, "4. Pulse rate:\t\t%d BPM\n\n\r", pulse);
  UARTSend( (unsigned char *) buf, strlen(buf));
  
  usnprintf(buf, 40, "5. EKG:\t\t\t%d Hz\n\n\r", EKG);
  UARTSend( (unsigned char *) buf, strlen(buf));
  
  usnprintf(buf, 40, "6. Battery:\t\t%d%%\n\n\r", batt/2);
  UARTSend( (unsigned char *) buf, strlen(buf));

  vTaskSuspend(NULL);   // suspend self
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
