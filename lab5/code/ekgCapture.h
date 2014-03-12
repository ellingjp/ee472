/*
 * EkgCapture.h
 * Author: Patrick Ma
 * 2/22/2014
 *
 * Defines the public interface for the EKG data capture task.
 *
 */

#include "tcb.h"

#define EKG_TIMER_BASE TIMER1_BASE	// the timeer base used for sample collection
#define EKG_TIMER TIMER_A	// the timer used for EKG sample collection
#define EKG_SEQ 0	// The sequence number assigned to the ekg sensor
#define EKG_CH ADC_CTL_CH0	// EKG analog input channel (ch0: pinE7, others: pinE4-6?)
#define EKG_PRIORITY 0	// the ekg sequence capture priority

/* Points to the TCB for the task */
extern TCB ekgCaptureTask;
