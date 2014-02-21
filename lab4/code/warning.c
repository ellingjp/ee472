/* 
 * warning.c
 * Author(s): jarrett Gaddy, PatrickMa
 * 1/28/2014
 *
 * Implements warning.h
 */

#include "globals.h"
#include "task.h"
#include "timebase.h"
#include "warning.h"
#include "schedule.h"
#include "inc/hw_types.h"
#include "drivers/rit128x96x4.h"
#include <stdlib.h>
#include <stdio.h>

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"

#if DEBUG
#include "drivers/rit128x96x4.h"
#include "utils/ustdlib.h"
#endif

// alarm cycle period (on/off) in millisecond
#define ALARM_PERIOD 2000
#define ALARM_CYCLE_RATE    (ALARM_PERIOD / MINOR_CYCLE / 2)

// duration to sleep in terms of minor cycles
#define ALARM_SLEEP_PERIOD   (5 * MAJOR_CYCLE)

#define LED_GREEN GPIO_PIN_6
#define LED_RED   GPIO_PIN_5
#define LED_YELLOW GPIO_PIN_7

typedef enum {OFF, ON, ASLEEP} alarmState;
typedef enum {NONE, WARN_PRESS, WARN_TEMP, WARN_PULSE} warningState;
typedef enum {NORMAL, LOW} batteryState;

//pin E0 for input on switch 3
//pin C5 C6 and C7 for led out

// compiler prototypes
void warningRunFunction(void *dataptr);  // prototype for compiler

// Internal data structure
typedef struct WarningData {
  CircularBuffer *temperatureCorrected;
  CircularBuffer *systolicPressCorrected;
  CircularBuffer *diastolicPressCorrected;
  CircularBuffer *pulseRateCorrected;
  unsigned short *batteryState;
} WarningData;

extern tBoolean serialActive;
static unsigned long ulPeriod; // sets the alarm tone period
static WarningData data;        // internal data

TCB warningTask = {&warningRunFunction, &data};  // task interface


/* 
 * initializes task variables
 */
void initializeWarningTask() {
  //
  // Enable the peripherals used by this code. I.e enable the use of pin banks, etc.
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);        // bank C
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);        // bank E
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);        // bank F
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);        // bank G


  // configure the pin C5 for 4mA output
  GPIOPadConfigSet(GPIO_PORTC_BASE,LED_RED, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);
  GPIODirModeSet(GPIO_PORTC_BASE, LED_RED, GPIO_DIR_MODE_OUT);    

  // configure the pin C6 for 4mA output
  GPIOPadConfigSet(GPIO_PORTC_BASE,LED_GREEN, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);
  GPIODirModeSet(GPIO_PORTC_BASE, LED_GREEN, GPIO_DIR_MODE_OUT);

  // configure the pin C7 for 4mA output
  GPIOPadConfigSet(GPIO_PORTC_BASE,LED_YELLOW, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);
  GPIODirModeSet(GPIO_PORTC_BASE, LED_YELLOW, GPIO_DIR_MODE_OUT);

  // configure the pin E0 for input (sw3). NB: requires pull-up to operate
  GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA,
      GPIO_PIN_TYPE_STD_WPU);
  GPIODirModeSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_DIR_MODE_IN);


  /*  This function call does the same result of the above pair of calls,
   *  but still requires that the bank of peripheral pins is enabled via
   *  SysCtrlPeripheralEnable()
   */
  //  GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, LED_RED);

  ///////////////////////////////////////////
  //  This section defines the PWM speaker characteristics
  ///////////////////////////////////////////

  //
  // Set the clocking to run directly from the crystal.
  //
  SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

  //
  // Set GPIO F0 and G1 as PWM pins.  They are used to output the PWM0 and
  // PWM1 signals.
  //
  GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0);
  GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1);

  //
  // Compute the PWM period based on the system clock.
  //
  ulPeriod = SysCtlClockGet() / 65;

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

  // initialize the warning data pointers
  data.temperatureCorrected = &(global.temperatureCorrected);
  data.systolicPressCorrected = &(global.systolicPressCorrected);
  data.diastolicPressCorrected = &(global.diastolicPressCorrected);
  data.pulseRateCorrected = &(global.pulseRateCorrected);
  data.batteryState = &(global.batteryState);

}

////////////////////////////////////////////////////////////

/* 
 * Warning task function
 */
void warningRunFunction(void *dataptr) {
  
  static alarmState aState = OFF;
  static warningState wState = NONE;
  static batteryState bState = NORMAL;

  static warningState prevState;
  prevState = wState;

  static int wakeUpAlarmAt = 0;

  static tBoolean onFirstRun = true;

  if (onFirstRun){
    initializeWarningTask();
    onFirstRun = false;
  }


  // Get measurement data
  WarningData *wData = (WarningData *) dataptr;
  float temp = *( (float*) cbGet(wData->temperatureCorrected));
  float sysPress = *( (float*) cbGet(wData->systolicPressCorrected));
  float diaPress = *( (float*) cbGet(wData->diastolicPressCorrected));
  float pulse = *( (float*) cbGet(wData->pulseRateCorrected));
  unsigned short battery = *(wData->batteryState);

  // Alarm condition
  if ( (sysPress > SYS_MAX*ALARM_HIGH) ) { //|| // Commented lines = lab2
     // (temp < TEMP_MIN*ALARM_LOW || temp > (TEMP_MAX*ALARM_HIGH)) ||
     // (diaPress > DIA_MAX*ALARM_HIGH) ||
     // (pulse < PULSE_MIN*ALARM_LOW || pulse > PULSE_MAX*ALARM_HIGH) ) {

    // Should only turn alarm ON if it was previously OFF.  If it is
    // ASLEEP, shouldn't do anything.
    if (aState == OFF) aState = ON;
  }
  else
    aState = OFF;

  // Warning Condition
  if ( sysPress > SYS_MAX*ALARM_HIGH || diaPress > DIA_MAX*ALARM_HIGH )
    wState = WARN_PRESS;
  else if ( temp < TEMP_MIN*WARN_LOW || temp > TEMP_MAX*WARN_HIGH )
    wState = WARN_TEMP;
  else if ( pulse < PULSE_MIN*WARN_LOW || pulse > PULSE_MAX*WARN_HIGH )
    wState = WARN_PULSE;
  else
    wState = NONE; 

  // Battery Condition
  if (battery < BATTERY_MIN)
    bState = LOW;

  // Handle speaker, based on alarm state
  static tBoolean pwmEnable = false;
  switch (aState) {
    case ON:
      
      if (0 == (minor_cycle_ctr % ALARM_CYCLE_RATE)) { // toggle between on/off
        if (pwmEnable) 
          PWMGenEnable(PWM0_BASE, PWM_GEN_0);
        else
          PWMGenDisable(PWM0_BASE, PWM_GEN_0);
        pwmEnable = !pwmEnable;
      }
      break;
    case ASLEEP:
      PWMGenDisable(PWM0_BASE, PWM_GEN_0);
      break;
    default:  // OFF
      PWMGenDisable(PWM0_BASE, PWM_GEN_0);
      
      break;
  }

  // Handle warning cases
  static int toggletime;
  switch (wState) {
    case WARN_PRESS:
      GPIOPinWrite(GPIO_PORTC_BASE, LED_GREEN, 0X00);

      if (wState != prevState) {
        GPIOPinWrite(GPIO_PORTC_BASE, LED_RED, 0XFF); 
        toggletime = WARN_RATE_PRESS;
      }
      else if (0 == minor_cycle_ctr%toggletime) {
        if (GPIOPinRead(GPIO_PORTC_BASE, LED_RED) == 0)
          GPIOPinWrite(GPIO_PORTC_BASE, LED_RED, 0XFF); 
        else
          GPIOPinWrite(GPIO_PORTC_BASE, LED_RED, 0X00); 
      }

      break;
    case WARN_TEMP:
      GPIOPinWrite(GPIO_PORTC_BASE, LED_GREEN, 0X00);

      if (wState != prevState) {
        GPIOPinWrite(GPIO_PORTC_BASE, LED_RED, 0XFF); 
        toggletime = WARN_RATE_TEMP;
      }
      else if (0 == minor_cycle_ctr%toggletime) {
        if (GPIOPinRead(GPIO_PORTC_BASE, LED_RED) == 0)
          GPIOPinWrite(GPIO_PORTC_BASE, LED_RED, 0XFF); 
        else
          GPIOPinWrite(GPIO_PORTC_BASE, LED_RED, 0X00); 
      }
      break;
    case WARN_PULSE:
      GPIOPinWrite(GPIO_PORTC_BASE, LED_GREEN, 0X00);

      if (wState != prevState) {
        GPIOPinWrite(GPIO_PORTC_BASE, LED_RED, 0XFF); 
        toggletime = WARN_RATE_PULSE;
      }
      else if (0==minor_cycle_ctr%toggletime) {
        if (GPIOPinRead(GPIO_PORTC_BASE, LED_RED) == 0)
          GPIOPinWrite(GPIO_PORTC_BASE, LED_RED, 0XFF); 
        else
          GPIOPinWrite(GPIO_PORTC_BASE, LED_RED, 0X00); 
      }
      break;
    default:  // NORMAL
      GPIOPinWrite(GPIO_PORTC_BASE, LED_GREEN, 0xFF);
      GPIOPinWrite(GPIO_PORTC_BASE, LED_RED, 0x00);
      break;
  }

  // activate the remote terminal task if in ANY warn or alarm state
  if (NONE == wState && OFF == aState)
    serialActive = false;
  else
    serialActive = true;

  // battery state indicator
  if (bState == LOW) {
    GPIOPinWrite(GPIO_PORTC_BASE, LED_YELLOW, 0xFF);
    GPIOPinWrite(GPIO_PORTC_BASE, LED_GREEN, 0x00);
  }
  else
    GPIOPinWrite(GPIO_PORTC_BASE, LED_YELLOW, 0x00);

  /* This is the alarm override
   * Upon override, the alarm is silenced for some time.
   * silence length is defined by ALARM_SLEEP_PERIOD
   * 
   * If the button is pushed, the value returned is 0
   * If the button is NOT pushed, the value is non-zero
   */
  if ( ( 0 == global.alarmAcknowledge) && (aState == ON) )
  {
    //GPIOPinWrite(GPIO_PORTC_BASE, LED_YELLOW, 0XFF);  // for debug, lights led
    aState = ASLEEP;
    wakeUpAlarmAt = minor_cycle_ctr + ALARM_SLEEP_PERIOD;
  }

  // Check whether to resound alarm
  if (minor_cycle_ctr == wakeUpAlarmAt && aState == ASLEEP) {
    aState = ON;
    //GPIOPinWrite(GPIO_PORTC_BASE, LED_YELLOW, 0X00);  // for debug, kills led
  }
  
#if DEBUG
    char num[30];


    usnprintf(num, 30, "Cor temp: %d  ", (int) temp);
    RIT128x96x4StringDraw(num, 0, 0, 15);

    usnprintf(num, 30, "Cor Syst: %d  ", (int) sysPress);
    RIT128x96x4StringDraw(num, 0, 10, 15);

    usnprintf(num, 30, "Cor Dia: %d  ", (int) diaPress);
    RIT128x96x4StringDraw(num, 0, 20, 15);

    usnprintf(num, 30, "Cor Pulse: %d  ", (int) pulse);
    RIT128x96x4StringDraw(num, 0, 30, 15);
    
    usnprintf(num, 30, "Cor Batt: %d  ", (unsigned short) battery);
    RIT128x96x4StringDraw(num, 0, 40, 15);

    
    usnprintf(num, 30, "aState: %d  ", aState);
    RIT128x96x4StringDraw(num, 0, 50, 15);

    usnprintf(num, 30, "alarmAck: %d  ", global.alarmAcknowledge);
    RIT128x96x4StringDraw(num, 0, 60, 15);
    
    usnprintf(num, 30, "pwmEn: %d  ", pwmEnable);
    RIT128x96x4StringDraw(num, 0, 70, 15);
#endif
}
