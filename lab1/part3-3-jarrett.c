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


void delay(int aValue);

//*****************************************************************************
//
// Print A and C to the OLED board and flash in 1 second interval.
// Print B and D to the OLED board and flash in 2 second interval.
//
//*****************************************************************************
int
main(void)
{
//
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    //
    // Initialize the OLED display.
    //
    RIT128x96x4Init(1000000);
    
    //  define some local variables
	int delayVal = 1000;
	char A[1];
	A[0] = 'A';
	A[1] = '/0';
    char B[1];
	B[0] = 'B';
	B[1] = '/0';      
	char C[1];
	C[0] = 'C';
	C[1] = '/0';      
	char D[1];
	D[0] = 'D';
	D[1] = '/0';
	char space[1];
	space[0] = ' ';
	space[1] = '/0';
    
    //
    // The value if i is:
    //
    //RIT128x96x4StringDraw("The value of i is: \n", 10, 24, 15);
    
    //
    //  print A, B, C, and D and flash them.
    while(TRUE)
    {

	  RIT128x96x4StringDraw(A, 10, 24, 15);
	  RIT128x96x4StringDraw(B, 20, 24, 15);
	  RIT128x96x4StringDraw(C, 30, 24, 15);
	  RIT128x96x4StringDraw(D, 40, 24, 15);
	  delay(delayVal);
	  RIT128x96x4StringDraw(space, 10, 24, 15);
	  RIT128x96x4StringDraw(space, 20, 24, 15);
	  delay(delayVal);
	  RIT128x96x4StringDraw(A, 10, 24, 15);
	  RIT128x96x4StringDraw(B, 20, 24, 15);
	  RIT128x96x4StringDraw(space, 30, 24, 15);
	  RIT128x96x4StringDraw(space, 40, 24, 15);
	  delay(delayVal);
	  RIT128x96x4StringDraw(space, 10, 24, 15);
	  RIT128x96x4StringDraw(space, 20, 24, 15);
	  delay(delayVal);
	  
	  
	  
	  
      




}




//  software delay
void delay(int aValue)
{
    volatile int i = 0;

    volatile unsigned int j = 0;
    
    for (i = aValue; i > 0; i--)
    {
        for (j = 0; j < 100; j++);
    }

    return;
}