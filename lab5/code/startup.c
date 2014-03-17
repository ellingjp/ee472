/*
 * startup.c
 * author: Patrick ma
 * Date: 2/13/2014
 *
 * initializes the system and system variables
 */

#include <String.h>
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
#include "driverlib/timer.h"



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

void timer1IntHandler (void) {

    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    //
    // Indicate that a SysTick interrupt has occurred.
    //
    HWREGBITW(&g_ulFlags, FLAG_SYSTICK) = 1;

    //
    // Call the lwIP timer handler.
    //
    lwIPTimer(SYSTICKMS);
}


/*
 * Initializes hw counter ans system state variables
 */
void startup() {
  
      SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
      SYSCTL_XTAL_8MHZ);
  
  
    RIT128x96x4Init(1000000);
    
  // Initialize global data
  initializeGlobalData();   // from globals.h
    initializeTimebase();
    
    
    
////////////////////////////////////////////
/////  Web server initialization stuff /////
////////////////////////////////////////////
  unsigned long ulUser0, ulUser1;
  unsigned char pucMACArray[8];
  
    // Enable and Reset the Ethernet Controller.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);
    SysCtlPeripheralReset(SYSCTL_PERIPH_ETH);

    // Enable Port F for Ethernet LEDs.
    //  LED0        Bit 3   Output
    //  LED1        Bit 2   Output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);

    // Configure timer for a periodic interrupt.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);        
	TimerDisable(TIMER1_BASE, TIMER_BOTH);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER );
	TimerLoadSet(TIMER1_BASE, TIMER_A, (SysCtlClockGet() / SYSTICKHZ));
        TimerIntRegister(TIMER1_BASE,TIMER_A, timer1IntHandler);
        TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
        TimerEnable(TIMER1_BASE, TIMER_A);

  IntMasterEnable();

    FlashUserGet(&ulUser0, &ulUser1);
    if((ulUser0 == 0xffffffff) || (ulUser1 == 0xffffffff))
    {
        // We should never get here.  This is an error if the MAC address
        // has not been programmed into the device.  Exit the program.
        RIT128x96x4StringDraw("MAC Address", 0, 76, 15);
        RIT128x96x4StringDraw("Not Programmed!", 0, 86, 15);
        while(1);
    }
	
    pucMACArray[0] = ((ulUser0 >>  0) & 0xff);
    pucMACArray[1] = ((ulUser0 >>  8) & 0xff);
    pucMACArray[2] = ((ulUser0 >> 16) & 0xff);
    pucMACArray[3] = ((ulUser1 >>  0) & 0xff);
    pucMACArray[4] = ((ulUser1 >>  8) & 0xff);
    pucMACArray[5] = ((ulUser1 >> 16) & 0xff);
	
    // Initialze the lwIP library, using DHCP.
    lwIPInit(pucMACArray, 0, 0, 0, IPADDR_USE_DHCP);

    // Setup the device locator service.
    LocatorInit();
    LocatorMACAddrSet(pucMACArray);
    LocatorAppTitleSet("EK-LM3S8962 enet_io");

    // Initialize a sample httpd server.
    httpd_init();
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
 //       RIT128x96x4Enable(1000000);
        if(iColumn < 12)
        {
            RIT128x96x4StringDraw(" >", 114, 86, 15);
            RIT128x96x4StringDraw("< ", 0, 86, 15);
            RIT128x96x4StringDraw("*",iColumn, 86, 7);
        }
        else
        {
            RIT128x96x4StringDraw(" *",iColumn - 6, 86, 7);
        }

        iColumn += 4;
        if(iColumn > 114)
        {
            iColumn = 6;
            RIT128x96x4StringDraw(" >", 114, 86, 15);
        }
        //RIT128x96x4Disable();
    }

    //
    // Check if IP address has changed, and display if it has.
    //
    else if(ulLastIPAddress != ulIPAddress)
    {
        ulLastIPAddress = ulIPAddress;
//        RIT128x96x4Enable(1000000);
//        RIT128x96x4StringDraw("                       ", 0, 16, 15);
        RIT128x96x4StringDraw("                       ", 0, 86, 15);
        RIT128x96x4StringDraw("IP:   ", 0, 86, 15);
//        RIT128x96x4StringDraw("MASK: ", 0, 24, 15);
//        RIT128x96x4StringDraw("GW:   ", 0, 32, 15);
        DisplayIPAddress(ulIPAddress, 36, 86);
//        ulIPAddress = lwIPLocalNetMaskGet();
//        DisplayIPAddress(ulIPAddress, 36, 24);
//        ulIPAddress = lwIPLocalGWAddrGet();
//        DisplayIPAddress(ulIPAddress, 36, 32);
//        RIT128x96x4Disable();
    }
}

