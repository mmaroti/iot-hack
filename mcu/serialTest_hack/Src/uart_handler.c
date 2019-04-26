/*
 * uart_handler.c
 *
 *  Created on: 2019. �pr. 26.
 *      Author: gyooo
 */

#include "uart_handler.h"
#include "usart.h"
#include <string.h>

int isRxReady(UART_HandleTypeDef *huart) {
	return (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE) ? SET : RESET) == SET;
}

enum {
	CONSOLE_LEN = 100,
};
uint8_t console[CONSOLE_LEN];
int console_pos = 0;

void processConsole() {
	if (memcmp(console, "toggle", 6) == 0) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	}
}

void handleConsole() {
	//usart2
	if (isRxReady(&huart2)) {
		uint8_t data;
		HAL_UART_Receive(&huart2, &data, 1, 1);
		if (data == '\n') {
			processConsole();
			console_pos = 0;
		} else if (console_pos < CONSOLE_LEN) {
			console[console_pos++] = data;
		}
	}
}

void handleRadio() {
	//usart1
}
