/*
 * commandTask.h
 * Author(s): Patrick Ma
 *
 * 3/10/2014
 *
 * Header for the command task
 *
 * Command supported by the commandTask are of the form:
 * <X> <Y> <Z>
 *
 * These place holders are explained below.
 *
 * <X> : Primary Command affecting major system functions
 * 		S - Indicates START mode. Causes the hardware to initiate the
 * 		previously assigned measurement task. 
 *
 * 		P - Indicates STOP mode. Causes the hardware to suspend any current
 * 		measurements.
 *
 * 		D - Toggles the local OLED display on and off
 *
 * 		M - Initiate a MEASURE function. Takes additional commands to specify the
 * 		type and number of measurements
 *
 * 		G - Causes the commandTask to enter DEBUG mode. Additional modifiers
 * 		specify which state to debug.
 *
 * <Y> : Modifier specifying the sublevel of behavior
 * 		T - Instruct system to measure Temperature. Will return the temperature in degrees Celsius
 *
 * 		B - Instruct system to measure the patient blood pressure. Will return the systolic and diastolic blood pressure in mmHg
 *
 * 		P - Instruct system to measure patient pulse rate. Will return the heart rate in beats per minute
 *
 * 		E - Instruct the system to perform an EKG measurement. Will return the primary EKG frequency
 *
 * 		D - DEBUG mode only. Returns the local display status
 *
 * 		M - DEBUG mode only. Returns system measurement status. Returns boolean true if measurement is enabled, false otherwise
 *
 * <Z> : Modifier specifying the number of measurements to perform
 * 		<no value> - Scan continuously
 *
 * 		<integer value> - perform the specified number of measurements
 */

#include "tcb.h"

/* points to the TCB for commandTask */
extern TCB commandTask;
