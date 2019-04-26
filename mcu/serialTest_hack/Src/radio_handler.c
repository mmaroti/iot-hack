/*
 * radio_handler.c
 *
 *  Created on: 2019. ápr. 26.
 *      Author: gyooo
 */

#include "radio_handler.h"
#include "usart.h"
#include <string.h>
#include "console_handler.h"

int is_radio_rx_ready() {
	return (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) ? SET : RESET) == SET;
}

enum {
	RADIO_LEN = 100,
};
uint8_t radio[RADIO_LEN];
int radio_pos = 0;

void process_radio() {
	if(radio[0]=='\n' || radio[0]=='\r') {
		return;
	}
	if (memcmp(&radio, "OK", 6) == 0) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	}
	if(radio_pos<RADIO_LEN-3) {
		radio[radio_pos-1] = '\r';
		radio[radio_pos++] = '\n';
		radio[radio_pos++] = 0;
		write_console(radio);
	}
}

void handle_radio() {
	//usart2
	if (is_radio_rx_ready()) {
		uint8_t data;
		HAL_UART_Receive(&huart1, &data, 1, 1);
		if (data == '\n' || data == '\r') {
			data = 0;
		}
		if (radio_pos < RADIO_LEN) {
			radio[radio_pos++] = data;
			if (data == 0)
				process_radio();
		}
		if (data == 0)
			radio_pos = 0;
	}
}

void send_radio(char *data) {
	HAL_UART_Transmit(&huart1, data, strlen(data), 10);
}
