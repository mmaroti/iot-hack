/*
 * uart_handler.c
 *
 *  Created on: 2019. �pr. 26.
 *      Author: gyooo
 */

#include <console_handler.h>
#include "usart.h"
#include <string.h>
#include "radio_handler.h"

int is_rx_ready() {
	return (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) ? SET : RESET) == SET;
}

enum {
	CONSOLE_LEN = 100,
};
char console[CONSOLE_LEN];
int console_pos = 0;

void process_console() {
	if(console[0]=='\n' || console[0]=='\r') {
		return;
	}
	if (strcmp(console, "toggle") == 0) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	} else if(memcmp(console, "AT", 2) == 0 && console_pos<CONSOLE_LEN-3) {
		console[console_pos-1] = '\r';
		console[console_pos++] = '\n';
		console[console_pos++] = 0;
		send_radio(console);
	} else {
		write_console("Unknown: ");
		write_console(console);
		write_console("\n");
	}
}

void handle_console() {
	//usart2
	if (is_rx_ready()) {
		uint8_t data;
		HAL_UART_Receive(&huart2, &data, 1, 1);
		if (data == '\n' || data == '\r') {
			data = 0;
		}
		if (console_pos < CONSOLE_LEN) {
			console[console_pos++] = data;
			if (data == 0)
				process_console();
		}
		if (data == 0)
			console_pos = 0;
	}
}

void write_console(const char *text) {
	int len = strlen(text);
	HAL_UART_Transmit(&huart2, (uint8_t*) text, len, 10);
}
