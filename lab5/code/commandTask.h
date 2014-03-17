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
 * These placeholders are explained below. If an invalid command sequence is
 * given, commandTask responds with an 'E' character. Otherwise, the immediate
 * response is 'A', followed by any specified return value(s).
 *

 * <X> : Primary Command affecting major system functions
 *
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
 *
 * <Y> : Modifier specifying the sublevel of behavior
 * 		
 * 		A - Instruct the system to measure All sensors
 *
 * 		T - Instruct system to measure Temperature. Will return the temperature
 * 		in degrees Celsius
 *
 * 		B - Instruct system to measure the patient Blood pressure. Will return
 * 		the systolic and diastolic blood pressure in mmHg
 *
 * 		P - Instruct system to measure patient Pulse rate. Will return the heart
 * 		rate in beats per minute
 *
 * 		E - Instruct the system to perform an EKG measurement. Will return the
 * 		primary EKG frequency
 *
 * 		S - Query the system battery Status. Returns the percentage left.
 *
 * 		D - DEBUG mode only. Returns the local Display status
 *
 * 		M - DEBUG mode only. Returns system Measurement status. Returns boolean
 * 		true if measurement is enabled, false otherwise
 *
 * 		T - DEBUG mode only. Returns the Type of the last measurement performed.
 *
 *
 * <Z> : Modifier specifying the number of measurements to perform. The
 * 			 measurements are sent to a buffer for usage one at at time.
 * 			 
 * 		<no value> - Scan continuously
 *
 * 		<integer value> - perform the specified number of measurements.
 */

#include "tcb.h"

/* points to the TCB for commandTask */
extern TCB commandTask;
