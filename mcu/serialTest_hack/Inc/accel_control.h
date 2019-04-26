/*
 * accel_control.h
 *
 *  Created on: 2019. Apr. 26.
 *      Author: gyooo, mmaroti
 */

#ifndef ACCEL_CONTROL_H_
#define ACCEL_CONTROL_H

#include <inttypes.h>

// Set from the accelerometer interrupt handler
extern uint8_t accel_ready;

// initialize and configure accelerometer
void accel_init();

// Place part into measurement mode: POWER_CTL register
void accel_start();

// Place part into standby mode and clear FIFO
void accel_stop();

// Called from the main loop
void handle_accel();

//detect accident
uint8_t detect_accident(int16_t *data);

#endif /* ACCEL_CONTROL_H */
