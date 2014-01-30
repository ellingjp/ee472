/* 
 * warning.c
 * Author(s): jarrett Gaddy
 * 1/28/2014
 *
 * Implements warning.h
 */

#include "globals.h"
#include "timebase.h"
#include "warning.h"
#include "bool.h"
#include "drivers/rit128x96x4.h"
#include <stdlib.h>
#include <stdio.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"


//pin E0 for input on switch 3
//pin C5 C6 and C7 for led out



// Internal data structure
typedef struct WarningData {
  float *temperatureCorrected;
  float *systolicPressCorrected;
  float *diastolicPressCorrected;
  float *pulseRateCorrected;
  int *batteryState;
} WarningData;

static WarningData data;                   // internal data
static unsigned long ulPeriod;


void *warningData = (void *)&data;  // external pointer to internal data

void initializeWarningTask(void *data) {

	//GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_DIR_MODE_OUT); //green
	//GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_DIR_MODE_OUT); //red
	//GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_7, GPIO_DIR_MODE_OUT); //yellow

    //
    // Set the clocking to run directly from the crystal.
    //
    //SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);


    //
    // Enable the peripherals used by this code.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);

    //
    // Set GPIO F0 and G1 as PWM pins.  They are used to output the PWM0 and
    // PWM1 signals.
    //
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1);

    //
    // Compute the PWM period based on the system clock.
    //
    ulPeriod = SysCtlClockGet() / 65.4064;

    //
    // Set the PWM period to 440 (A) Hz.
    //
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ulPeriod);

    //
    // Set PWM0 to a duty cycle of 25% and PWM1 to a duty cycle of 75%.
    //
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, ulPeriod / 4);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ulPeriod * 3 / 4);

    //
    // Enable the PWM0 and PWM1 output signals.
    //
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, true);



  
  WarningData *mdata = (WarningData *)data;
  mdata->temperatureCorrected = &(globalDataMem.temperatureCorrected);
  mdata->systolicPressCorrected = &(globalDataMem.systolicPressCorrected);
  mdata->diastolicPressCorrected = &(globalDataMem.diastolicPressCorrected);
  mdata->pulseRateCorrected = &(globalDataMem.pulseRateCorrected);
  mdata->batteryState = &(globalDataMem.batteryState);
}


void warningTask(void *dataptr) {
  // only run on major cycle
  
static Bool tempWarn = false;
static Bool tempAlarm = false;
static Bool sysWarn = false;
static Bool sysAlarm = false;
static Bool diaWarn = false;
static Bool diaAlarm = false;
static Bool pulseWarn = false;
static Bool pulseAlarm = false;
static Bool batteryWarn = false;
static Bool ledGreen = false;
static Bool ledYellow = false;
static Bool ledRed = false;

  
  if (IS_MAJOR_CYCLE) {   // on major cycle
    WarningData *data = (WarningData *) dataptr;

	float temp = *(data->temperatureCorrected);
	float sysPress = *(data->systolicPressCorrected);
	float diaPress = *(data->diastolicPressCorrected);
	float pulse = *(data->pulseRateCorrected);
	int battery = *(data->batteryState);
	
	if(temp < (TEMP_MIN*WARN_LOW) || temp >(TEMP_MAX*WARN_HIGH))
		tempWarn = true;
	else tempWarn = false;
	
	if(temp < (TEMP_MIN*ALARM_LOW) || temp >(TEMP_MAX*ALARM_HIGH))
		tempAlarm = true;
	else tempAlarm = false;	

	if(sysPress < (SYS_MAX*WARN_HIGH))
		sysWarn = true;
	else sysWarn = false;
	
	if(sysPress < (SYS_MAX*ALARM_HIGH))
		sysAlarm = true;
	else sysAlarm = false;	

	if(diaPress < (DIA_MAX*WARN_HIGH))
		diaWarn = true;
	else diaWarn = false;
	
	if(diaPress < (DIA_MAX*ALARM_HIGH))
		diaAlarm = true;
	else diaAlarm = false;	

	if(pulse < (PULSE_MIN*WARN_LOW) || pulse >(PULSE_MAX*WARN_HIGH))
		pulseWarn = true;
	else pulseWarn = false;
	
	if(pulse < (PULSE_MIN*ALARM_LOW) || pulse >(PULSE_MAX*ALARM_HIGH))
		pulseAlarm = true;
	else pulseAlarm = false;	

	if(battery < (BATTERY_MIN))
		batteryWarn = true;
	else batteryWarn = false;
	
	
  }
	Bool normal = true;
	Bool sound = false;
	if( true == pulseWarn)
	{
		//led on 1 sec off 1 sec
		normal = false;
		//GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0XFF)
	}
	if( true == pulseAlarm)
	{
		//sound
		sound = true;
		normal = false;
	}
	if( true == tempWarn)
	{
		//led on .5 sec off .5 sec
		normal = false;
		//GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0XFF)
	}
	if( true == tempAlarm)
	{
		//sound
		sound = true;
		normal = false;
	}
	if( true == sysWarn)
	{
		//led on .25 sec off .25 sec
		normal = false;
		//GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0XFF)
	}
	if( true == sysAlarm)
	{
		//sound
		sound = true;
		normal = false;
	}
	if( true == diaWarn)
	{
		//led on .25 sec off .25 sec
		normal = false;
		//GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0XFF)
	}
	if( true == diaAlarm)
	{
		//sound
		sound = true;
		normal = false;
	}
	if(true == batteryWarn)
	{
		//GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0XFF)
		
	}
	if(normal == true)
	{
		/*if(false == batteryWarn)
			GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0XFF)//green led on solid
		GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0X00)//yellow led off
		GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0X00)//red led off*/
	}
	else
	{
		//GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0X00)//green led off
	}
	if(true == sound)
	{
		PWMGenEnable(PWM0_BASE, PWM_GEN_0);
	
	}
	else
	{
		PWMGenDisable(PWM0_BASE, PWM_GEN_0);
	}
	//playSound()
		
	  
  
}
