/*
 * startup.h
 * author: Patrick ma
 * Date: 2/13/2014
 *
 * initializes the system and system variables
 */

extern tBoolean runSchedule;

// Interrupt handler for hw timer
void SysTickIntHandler();

// configure the hardware timer
void initializeHWCounter();

// initialize system variables and hardware timer
void startup();

void DisplayIPAddress();

void lwIPHostTimerHandler();

