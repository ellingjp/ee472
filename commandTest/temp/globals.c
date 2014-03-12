/* 
 * globals.c
 * Author(s): Jonathan Ellington, Patrick Ma
 * 1/28/2014
 *
 * Defines global data for tasks to access
 */
#include "globals.h"

GlobalData global;

// The arrays to be wrapped in a 
// circular buffer
static int temperatureRawArr[8];
static int systolicPressRawArr[8];
static int diastolicPressRawArr[8];
static int pulseRateRawArr[8];

static float temperatureCorrectedArr[8];
static float systolicPressCorrectedArr[8];
static float diastolicPressCorrectedArr[8];
static float pulseRateCorrectedArr[8];
static int ekgFrequencyResultArr[16];

static signed int ekgRaw[NUM_EKG_SAMPLES];	// initialize all the elements to 0
static signed int ekgTemp[NUM_EKG_SAMPLES];

void initializeGlobalData() {
	// Wrap the arrays
	global.temperatureRaw = cbWrap(temperatureRawArr, sizeof(int), 8);
	global.systolicPressRaw = cbWrap(systolicPressRawArr, sizeof(int), 8);
	global.diastolicPressRaw = cbWrap(diastolicPressRawArr, sizeof(int), 8);
	global.pulseRateRaw = cbWrap(pulseRateRawArr, sizeof(int), 8);


	global.temperatureCorrected = cbWrap(temperatureCorrectedArr, sizeof(float), 8);
	global.systolicPressCorrected = cbWrap(systolicPressCorrectedArr, sizeof(float), 8);
	global.diastolicPressCorrected = cbWrap(diastolicPressCorrectedArr, sizeof(float), 8);
	global.pulseRateCorrected = cbWrap(pulseRateCorrectedArr, sizeof(float), 8);
	global.ekgFrequencyResult = cbWrap(ekgFrequencyResultArr, sizeof(int), 16);

	int tr = TEMP_RAW_INIT;
	int sr = SYS_RAW_INIT;
	int dr = DIA_RAW_INIT;
	int pr = PULSE_RAW_INIT;

	float tc = TEMP_CORR_INIT;
	float sc = SYS_CORR_INIT;
	float dc = DIA_CORR_INIT;
	float pc = PULSE_CORR_INIT;
	int fr = EKG_FREQ_RLT;

	// Add initial values
	cbAdd(&(global.temperatureRaw), &tr);
	cbAdd(&(global.systolicPressRaw), &sr);
	cbAdd(&(global.diastolicPressRaw), &dr);
	cbAdd(&(global.pulseRateRaw), &pr);

	cbAdd(&(global.temperatureCorrected), &tc);
	cbAdd(&(global.systolicPressCorrected), &sc);
	cbAdd(&(global.diastolicPressCorrected), &dc);
	cbAdd(&(global.pulseRateCorrected), &pc);
	cbAdd(&(global.ekgFrequencyResult), &fr);

	// Set normal variables
	global.batteryState = 200;
	global.mode = 0;
	global.measurementSelection = 0;
	global.alarmAcknowledge = false;
	global.select = false;
	global.scroll = 0;
	memset(&(global.commandStr), NULL, sizeof(char) * COMMAND_LENGTH);
	memset(&(global.responseStr), NULL, sizeof(char) * COMMAND_LENGTH);
	global.displayOn = true;
}

// debug tool
void debugPin47() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);        // debug
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2);
	long a = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_2);
	if(0 == a) 
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0xFF);
	else 
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0x00);
}
