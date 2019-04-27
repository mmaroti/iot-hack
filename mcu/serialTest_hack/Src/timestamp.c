/*
 * timestamp.c
 *
 *  Created on: 2019. ápr. 27.
 *      Author: gyooo
 */

#include "timestamp.h"
#include "console_handler.h"
#include "stdint.h"
#include "stm32l4xx_hal.h"

uint32_t get_timestamp_cnt() {
	return HAL_GetTick();
}


void write_timestamp_console() {
	write_console_int((int)get_timestamp_cnt());
}
