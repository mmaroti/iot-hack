/*
 * accel_control.c
 *
 *  Created on: 2019. Apr. 26.
 *      Author: gyooo, mamroti
 */

#include "i2c.h"
#include "ADXL345.h"
#include "accel_control.h"
#include "console_handler.h"

uint8_t accel_ready;

enum {
	ACCEL_BATCH = 25, // water mark for hardware fifo
	ACCEL_LEN = 3 * 25, // must be divisible by 3*25
};

int16_t accel[ACCEL_LEN];
uint32_t accel_pos;

void handle_accel() {
	uint8_t ready;
	__disable_irq();
	ready = accel_ready;
	accel_ready = 0;
	__enable_irq();

	if (ready && accel_pos + 3 * ACCEL_BATCH <= ACCEL_LEN) {
		for (int n = 0; n < ACCEL_BATCH; n++) {
			ADXL345_GetXyz(accel + accel_pos, accel + accel_pos + 1,
					accel + accel_pos + 2);
			accel_pos += 3;
		}

		if (accel_pos >= ACCEL_LEN) {
			write_console("accel:");
			// for (int n = 0; n < accel_pos; n++) {
			//	write_console(" ");
			//	write_console_int(accel[n]);
			//}
			write_console("\n");
			accel_pos = 0;
		}
	}
}

//initialize and configure accelerometer
void accel_init() {
	uint8_t reg = 0u;
	HAL_I2C_MspInit(&hi2c3);
	ADXL345_Init(ADXL345_I2C_COMM);
	ADXL345_SetPowerMode(0);
	//Set data parameters such as data rate, measurement range, data format, and offset adjustment.
	reg = ADXL345_GetRegisterValue(ADXL345_BW_RATE);
	//reg = (reg & 0xF0) | 0x07; //0111 - 12,5Hz
	reg = (reg & 0xF0) | 0x0A; //1010 - 12,5Hz
	ADXL345_SetRegisterValue(ADXL345_BW_RATE, reg);
	//Configure interrupts (do not enable): thresholds and timing values, and map interrupts to pins.
	ADXL345_SetRegisterValue(ADXL345_INT_MAP, 0x00); //all interrupts connected to INT1
	//reg = ADXL345_GetRegisterValue(ADXL345_DATA_FORMAT);
	//reg = (reg & 0xFC) | 0x03; //11 - +-16g
	//ADXL345_SetRegisterValue(ADXL345_DATA_FORMAT, reg);

	//How to reset FIFO -> put to bypass mode

	//Configure FIFO (if in use): mode, trigger interrupt if using trigger mode, and samples bits.
	ADXL345_SetRegisterValue(ADXL345_FIFO_CTL, 0x00); //clears FIFO
	ADXL345_SetRegisterValue(ADXL345_FIFO_CTL, 0x59); //01- FIFO mode, 0 - trigger bit, 11001 - 25 samples (2 seconds) = 0101 1001 -> 0x59
	//Enable interrupts: INT_ENABLE register.
	ADXL345_SetRegisterValue(ADXL345_INT_ENABLE, 0x00);
	ADXL345_SetRegisterValue(ADXL345_INT_ENABLE, ADXL345_WATERMARK); //enable Watermark
	//ADXL345_SetRegisterValue(ADXL345_INT_ENABLE, ADXL345_DATA_READY); //enable data ready

	//init position pointer
	accel_pos = 0u;
}

//Place part into measurement mode: POWER_CTL register
void accel_start() {
	//measurement mode
	ADXL345_SetPowerMode(1);
}

//Place part into standby mode and clear FIFO
void accel_stop() {
	//disable interrupts
	ADXL345_SetRegisterValue(ADXL345_INT_ENABLE, 0x00);
	//put to standby mode
	ADXL345_SetPowerMode(0);
	//reconfigure for fast wake up time
	ADXL345_SetRegisterValue(ADXL345_FIFO_CTL, 0x00); //clears FIFO, bypass mode
	ADXL345_SetRegisterValue(ADXL345_FIFO_CTL, 0x59); //01- FIFO mode, 0 - trigger bit, 11001 - 25 samples (2 seconds) = 0101 1001 -> 0x59
	ADXL345_SetRegisterValue(ADXL345_INT_ENABLE, ADXL345_WATERMARK); //enable Watermark
}
