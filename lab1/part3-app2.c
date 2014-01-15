//*****************************************************************************
//
// Built from hello.c - Simple hello world example.
//
// Copyright (c) 2006-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 7243 of the EK-LM3S8962 Firmware Package.
//
//*****************************************************************************

#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"



//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

#define TRUE 1
#define FALSE 0
#define OLEDCLK 1000000
#define CHARWIDTH 6

void delay(unsigned long aValue);

//*****************************************************************************
//
// Display "ABCD", then remove each letter sequentially.
//
//*****************************************************************************
int
main(void)
{
    unsigned long flash_delay = 8000;   // Delay between flashes (approx 1 sec)
    
    char *characters = "ABCD";          // Characters to flash (if changed,
                                        // must change len)
    
    unsigned int len = 4;               // length of characters (excluding 
                                        // null term)
    
    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    // Initialize display
    RIT128x96x4Init(OLEDCLK);
    
    // Display ABCD, then remove A then B then C then D
    unsigned int xVal = 15;
    unsigned int yVal = 15;
    while(TRUE)
    {
      RIT128x96x4StringDraw(characters, xVal, yVal, 15);
      delay(flash_delay);
      
      for (volatile unsigned int i = 0; i < CHARWIDTH*len; i+=CHARWIDTH) {
        RIT128x96x4StringDraw(" ", xVal+i, yVal, 15);
        delay(flash_delay);
      }
    }
}

// Software delay
void delay(unsigned long aValue)
{
    for (volatile unsigned int i = 0; i < aValue; i++)
      for (volatile unsigned int j = 0; j < 100; j++);
}
