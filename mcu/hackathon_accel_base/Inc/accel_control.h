/*
 * accel_control.h
 *
 *  Created on: 2018. aug. 24.
 *      Author: gyooo
 */

#ifndef ACCEL_CONTROL_H_
#define ACCEL_CONTROL_H_

#include "ADXL345.h"
#include "stdint.h"
#include "consts.h"


int16_t accel[NUM_OF_ACCEL_SAMPLES+1];
uint32_t accelCurrentPos;
//temporaries
uint8_t reg;

void readAccelData(uint8_t n);

//initialize and configure accelerometer
void initAccel();

//Place part into measurement mode: POWER_CTL register
void startAccel();

//Place part into standby mode and clear FIFO
void stopAccel();



#endif /* ACCEL_CONTROL_H_ */
