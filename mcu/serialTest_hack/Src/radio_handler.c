/*
 * radio_handler.c
 *
 *  Created on: 2019. �pr. 26.
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
	RADIO_LEN = 128,
};
char radio[RADIO_LEN];
int radio_pos = 0;

void process_report() {
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

	if (0) {
		for (int i = 0; i < radio_pos; i++) {
			if (radio[i] < 32 || radio[i] > 127)
				radio[i] = '?';
		}
	}

	write_console("radio: ");
	radio[radio_pos] = 0;
	write_console(radio);
	write_console("\n");
}

void handle_radio() {
	if (is_radio_rx_ready()) {
		uint8_t data;
		if (HAL_UART_Receive(&huart1, &data, 1, 1) == HAL_OK) {
			if (data == '\n' || data == '\r') {
				data = 0;
			}
			if (radio_pos < RADIO_LEN) {
				radio[radio_pos++] = data;
				if (data == 0 && radio_pos > 1)
					process_report(radio);
			}
			if (data == 0)
				radio_pos = 0;
		}
	}
}

void send_radio(const char *data) {
	HAL_UART_Transmit(&huart1, (uint8_t*) data, strlen(data), 10);
}
