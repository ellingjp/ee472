/* 
 * timebase.h
 * Author(s): Jonathan Ellington
 * 1/28/2014
 *
 * Defines the major and minor cycles the system runs on
 */

#define MINOR_CYCLE 250      // minor cycle, in milliseconds
#define MAJOR_CYCLE 4      // major cycle, in number of minor cycles
#define PULSE_CYCLE 2       // pulse rate measurement time, in number of 
                            // major cycles.
#define IS_PULSE_CYCLE (minor_cycle_ctr % (MAJOR_CYCLE * PULSE_CYCLE) == 0)
#define IS_MAJOR_CYCLE (minor_cycle_ctr % MAJOR_CYCLE == 0)

extern unsigned int minor_cycle_ctr;    // counts number of minor cycles

void initializeTimebase(void);
void TimerAIntHandler(void);
