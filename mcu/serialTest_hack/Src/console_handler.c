/*
 * uart_handler.c
 *
 *  Created on: 2019. �pr. 26.
 *      Author: gyooo
 */

#include <console_handler.h>
#include "usart.h"
#include <string.h>

int is_rx_ready() {
	return (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) ? SET : RESET) == SET;
}

enum {
	CONSOLE_LEN = 100,
};
uint8_t console[CONSOLE_LEN];
int console_pos = 0;

void process_console() {
	if (memcmp(&console, "toggle", 6) == 0) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	}
}

void handle_console() {
	//usart2
	if (is_rx_ready()) {
		uint8_t data;
		HAL_UART_Receive(&huart2, &data, 1, 1);
		if (data == '\n' || data == '\r') {
			process_console();
			console_pos = 0;
		} else if (console_pos < CONSOLE_LEN) {
			console[console_pos++] = data;
		}
	}
}
