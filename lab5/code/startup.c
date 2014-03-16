/*
 * startup.c
 * author: Patrick ma
 * Date: 2/13/2014
 *
 * initializes the system and system variables
 */

#include "timebase.h"
#include "globals.h"
#include "driverlib/systick.h" // for systick interrupt (minor cycle)
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "startup.h"
#include "schedule.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "driverlib/ethernet.h"
#include "driverlib/flash.h"
#include "driverlib/gpio.h"
#include "utils/locator.h"
#include "utils/lwiplib.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "httpserver_raw/httpd.h"
#include "drivers/rit128x96x4.h"


//*****************************************************************************
//
// Defines for setting up the system clock.
//
//*****************************************************************************
#define SYSTICKHZ               100
#define SYSTICKMS               (1000 / SYSTICKHZ)
#define SYSTICKUS               (1000000 / SYSTICKHZ)
#define SYSTICKNS               (1000000000 / SYSTICKHZ)

//*****************************************************************************
//
// A set of flags.  The flag bits are defined as follows:
//
//     0 -> An indicator that a SysTick interrupt has occurred.
//
//*****************************************************************************
#define FLAG_SYSTICK            0
static volatile unsigned long g_ulFlags;

//*****************************************************************************
//
// External Application references.
//
//*****************************************************************************
extern void httpd_init(void);


//*****************************************************************************
//
// Timeout for DHCP address request (in seconds).
//
//*****************************************************************************
#ifndef DHCP_EXPIRE_TIMER_SECS
#define DHCP_EXPIRE_TIMER_SECS  45
#endif



/*
 * Minor Cycle Handler
 */
void SysTickIntHandler (void) {
  minor_cycle_ctr = minor_cycle_ctr + 1;
  
    //
    // Indicate that a SysTick interrupt has occurred.
    //
    HWREGBITW(&g_ulFlags, FLAG_SYSTICK) = 1;

    //
    // Call the lwIP timer handler.
    //
    lwIPTimer(SYSTICKMS);
  
  
//  runSchedule = true;
}


/*
 * Initializes hw counter ans system state variables
 */
void startup() {
  IntMasterEnable();
  initializeTimebase();
  
  // Initialize global data
  initializeGlobalData();   // from globals.h
  
  
  
  
  	
    FlashUserGet(&ulUser0, &ulUser1);
    if((ulUser0 == 0xffffffff) || (ulUser1 == 0xffffffff))
    {
        //
        // We should never get here.  This is an error if the MAC address
        // has not been programmed into the device.  Exit the program.
        //
        RIT128x96x4StringDraw("MAC Address", 0, 16, 15);
        RIT128x96x4StringDraw("Not Programmed!", 0, 24, 15);
        while(1);
    }
	
	pucMACArray[0] = ((ulUser0 >>  0) & 0xff);
    pucMACArray[1] = ((ulUser0 >>  8) & 0xff);
    pucMACArray[2] = ((ulUser0 >> 16) & 0xff);
    pucMACArray[3] = ((ulUser1 >>  0) & 0xff);
    pucMACArray[4] = ((ulUser1 >>  8) & 0xff);
    pucMACArray[5] = ((ulUser1 >> 16) & 0xff);

	
	    //
    // Initialze the lwIP library, using DHCP.
    //
    lwIPInit(pucMACArray, 0, 0, 0, IPADDR_USE_DHCP);

    //
    // Setup the device locator service.
    //
    LocatorInit();
    LocatorMACAddrSet(pucMACArray);
    LocatorAppTitleSet("EK-LM3S8962 enet_io");

	
	
	   //
    // Initialize a sample httpd server.
    //
    httpd_init();

    //
    // Pass our tag information to the HTTP server.
    //

    // Initialize IO controls
    //
    io_init();
	
	

  //computeActive = false;	// neither serial or compute task runs at start up
  //serialActive = false;
  //ekgProcessActive = false;

//  initializeQueue(); // start up task queue with basic tasks
}








//*****************************************************************************
//
// Display an lwIP type IP Address.
//
//*****************************************************************************
void DisplayIPAddress(unsigned long ipaddr, unsigned long ulCol,
                 unsigned long ulRow)
{
    char pucBuf[16];
    unsigned char *pucTemp = (unsigned char *)&ipaddr;

    //
    // Convert the IP Address into a string.
    //
    usprintf(pucBuf, "%d.%d.%d.%d", pucTemp[0], pucTemp[1], pucTemp[2],
             pucTemp[3]);

    //
    // Display the string.
    //
    RIT128x96x4StringDraw(pucBuf, ulCol, ulRow, 15);
}

//*****************************************************************************
//
// Required by lwIP library to support any host-related timer functions.
//
//*****************************************************************************
void lwIPHostTimerHandler(void)
{
    static unsigned long ulLastIPAddress = 0;
    unsigned long ulIPAddress;

    ulIPAddress = lwIPLocalIPAddrGet();

    //
    // If IP Address has not yet been assigned, update the display accordingly
    //
    if(ulIPAddress == 0)
    {
        static int iColumn = 6;

        //
        // Update status bar on the display.
        //
        RIT128x96x4Enable(1000000);
        if(iColumn < 12)
        {
            RIT128x96x4StringDraw(" >", 114, 24, 15);
            RIT128x96x4StringDraw("< ", 0, 24, 15);
            RIT128x96x4StringDraw("*",iColumn, 24, 7);
        }
        else
        {
            RIT128x96x4StringDraw(" *",iColumn - 6, 24, 7);
        }

        iColumn += 4;
        if(iColumn > 114)
        {
            iColumn = 6;
            RIT128x96x4StringDraw(" >", 114, 24, 15);
        }
        RIT128x96x4Disable();
    }

    //
    // Check if IP address has changed, and display if it has.
    //
    else if(ulLastIPAddress != ulIPAddress)
    {
        ulLastIPAddress = ulIPAddress;
        RIT128x96x4Enable(1000000);
        RIT128x96x4StringDraw("                       ", 0, 16, 15);
        RIT128x96x4StringDraw("                       ", 0, 24, 15);
        RIT128x96x4StringDraw("IP:   ", 0, 16, 15);
        RIT128x96x4StringDraw("MASK: ", 0, 24, 15);
        RIT128x96x4StringDraw("GW:   ", 0, 32, 15);
        DisplayIPAddress(ulIPAddress, 36, 16);
        ulIPAddress = lwIPLocalNetMaskGet();
        DisplayIPAddress(ulIPAddress, 36, 24);
        ulIPAddress = lwIPLocalGWAddrGet();
        DisplayIPAddress(ulIPAddress, 36, 32);
        RIT128x96x4Disable();
    }
}

