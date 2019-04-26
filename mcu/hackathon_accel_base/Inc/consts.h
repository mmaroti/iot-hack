#ifndef CONSTS_H
#define CONSTS_H

#define REV_E

#define DEBUG_ENABLE

//sampling parameters
#define NUM_OF_SAMPLES 8*1024
#define NUM_OF_FIRST_PHASE_SAMPLES 200

//if the max signal peak is smaller in the 1st phase, cancel sampling
//good value ~2100-2200
#define MIC_THRESH 2300

//SD write after NUM_OF_ACCEL_SAMPLES_PER_CH samples
#define NUM_OF_ACCEL_SAMPLES_PER_CH 100u
//~45KB -> SD write in every ~20 minutes if accel freq is 12,5Hz
#define NUM_OF_ACCEL_SAMPLES 3u*NUM_OF_ACCEL_SAMPLES_PER_CH


#endif
