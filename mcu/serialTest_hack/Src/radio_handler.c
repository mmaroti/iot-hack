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

enum {
	CONFIG_NONE = 0,
	CONFIG_ECHO = 1,
	CONFIG_CLOSE = 2,
	CONFIG_OPEN = 3,
	CONFIG_DONE = 4,
};
uint8_t config_step = 0;
uint16_t config_wait = 0;
uint8_t valid_ip = 0;

void send_config(const char *cmd) {
	write_console("config: ");
	write_console(cmd);
	write_console("\n");
	send_radio(cmd);
	send_radio("\r");
}

void process_report() {
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

	write_console("radio: ");
	write_console(radio);
	write_console("\n");

	if (strcmp(radio, "OK") == 0 && config_step < CONFIG_DONE) {
		if (config_step == CONFIG_NONE) {
			send_config("ATE1");
		} else if (config_step == CONFIG_ECHO) {
//			send_config("AT+QICLOSE=1");
//		} else if (config_step == CONFIG_CLOSE) {
			send_config("AT+QIOPEN=1,0,\"UDP\",\"52.4.126.47\",1973,0,1");
		} else if (config_step == CONFIG_OPEN) {
			write_console("config: done\n");
		}
		config_step += 1;
	}
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

	if (config_step == CONFIG_NONE && ++config_wait == 0) {
		send_config("AT+QSCLK=0");
	}
}

void send_radio(const char *data) {
	for (int i = 0; i < strlen(data); i++)
		HAL_UART_Transmit(&huart1, (uint8_t*) data + i, 1, 10);
}
