/* 
 * warning.h
 * Author(s): Jarrett Gaddy
 * 1/28/2014
 *
 * Defines the interface for the warning task  
 * initializeWarningData() should be called before running warningTask()
 */
 
 
#define WARN_LOW 0.95 //warn at 5% below min range value
#define WARN_HIGH 1.05 //warn at 5% above max range value
#define ALARM_LOW 0.90 //alarm at 10% below min range value
#define ALARM_HIGH 1.10 //alarm at 10% above max range value

#define TEMP_MIN 36.1
#define TEMP_MAX 37.8
#define SYS_MAX 120
#define DIA_MAX 80
#define PULSE_MIN 60
#define PULSE_MAX 100
#define BATTERY_MIN 40

 
 

/* Points to the data used by warning */
extern void *warningData;

/* Initialize displayData, must be done before running warningTask() */
void initializeWarningTask(void *displayData);

/* Perform the warning task */
void warningTask(void *dataptr);