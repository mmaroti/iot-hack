/*
 * accel_control.c
 *
 *  Created on: 2018. aug. 24.
 *      Author: gyooo
 */

#include "accel_control.h"
#include "i2c.h"

int16_t dummy_x, dummy_y, dummy_z;

void readAccelData(uint8_t n) {
	for (; n > 0; n--) {
		ADXL345_GetXyz(&accel[accelCurrentPos], &accel[accelCurrentPos+1], &accel[accelCurrentPos+2]);
		accelCurrentPos += 3;
		if (accelCurrentPos >= NUM_OF_ACCEL_SAMPLES) {
			//read the remaining samples, otherwise the next interrupt comes too early
			n--;
			for (; n > 0; n--) {
				ADXL345_GetXyz(&dummy_x, &dummy_y, &dummy_z);
			}
			break;
		}
	}
}

//initialize and configure accelerometer
void initAccel() {
	reg = 0u;
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
	accelCurrentPos = 0u;
}

//Place part into measurement mode: POWER_CTL register
void startAccel() {
	//measurement mode
	ADXL345_SetPowerMode(1);
}

//Place part into standby mode and clear FIFO
void stopAccel() {
	//disable interrupts
	ADXL345_SetRegisterValue(ADXL345_INT_ENABLE, 0x00);
	//put to standby mode
	ADXL345_SetPowerMode(0);
	//reconfigure for fast wake up time
	ADXL345_SetRegisterValue(ADXL345_FIFO_CTL, 0x00); //clears FIFO, bypass mode
	ADXL345_SetRegisterValue(ADXL345_FIFO_CTL, 0x59); //01- FIFO mode, 0 - trigger bit, 11001 - 25 samples (2 seconds) = 0101 1001 -> 0x59
	ADXL345_SetRegisterValue(ADXL345_INT_ENABLE, ADXL345_WATERMARK); //enable Watermark
}
