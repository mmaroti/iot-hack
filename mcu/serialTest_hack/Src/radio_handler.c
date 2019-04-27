/*
 * radio_handler.c
 *
 *  Created on: 2019. �pr. 26.
 *      Author: gyooo
 */

#include "console_handler.h"
#include "radio_handler.h"
#include "usart.h"
#include <string.h>
#include <stdlib.h>

enum {
	RADIO_LEN = 128, // radio to mcu buffer
	COMMAND_LEN = 512, // mcu to radio buffer
};

char radio[RADIO_LEN];
int radio_pos = 0;

char command[COMMAND_LEN];
int command_pos = 0;
int command_len = 0;

enum {
	CONFIG_NONE = 0,
	CONFIG_ECHO_SEND = 1,
	CONFIG_ECHO_SENT = 2,
	CONFIG_CLOSE_SEND = 3,
	CONFIG_CLOSE_SENT = 4,
	CONFIG_OPEN_SEND = 5,
	CONFIG_OPEN_SENT = 6,
	CONFIG_DONE = 7,
};

uint8_t config_step = 0;
uint16_t config_wait = 0;

void write_radio_raw(const char *text) {
	while (*text != 0 && command_len < COMMAND_LEN) {
		int pos = command_pos + command_len;
		if (pos >= COMMAND_LEN)
			pos -= COMMAND_LEN;
		command[pos] = *(text++);
		command_len += 1;
	}
}

void write_radio_int(int value) {
	char buffer[20];
	itoa(value, buffer, 10);
	write_radio_raw(buffer);
}

void send_config(const char *cmd) {
	write_console("config: ");
	write_console(cmd);
	write_console("\n");
	write_radio_raw(cmd);
	write_radio_raw("\r");
}

void process_report() {
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

	write_console("radio: ");
	write_console(radio);
	write_console("\n");

	if (config_step < CONFIG_DONE && strcmp(radio, "OK") == 0) {
		if (config_step == CONFIG_NONE) {
			config_wait = 0;
			config_step = CONFIG_ECHO_SEND;
		} else if (config_step == CONFIG_ECHO_SENT) {
			config_wait = 0;
			config_step = CONFIG_CLOSE_SEND;
		} else if (config_step == CONFIG_CLOSE_SENT) {
			config_wait = 0;
			config_step = CONFIG_OPEN_SEND;
		} else if (config_step == CONFIG_OPEN_SENT) {
			write_console("config: done\n");
			config_step = CONFIG_DONE;
		}
	}

	if (config_step < CONFIG_DONE && strcmp(radio, "ERROR") == 0) {
		if (config_step == CONFIG_OPEN_SENT) {
			config_wait = 0;
			config_step = CONFIG_OPEN_SEND;
		}
	}
}

int is_radio_rx_ready() {
	return (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) ? SET : RESET) == SET;
}

int is_radio_tx_ready() {
	return (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE) ? SET : RESET) == SET;
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

	if (command_len > 0 && is_radio_tx_ready()) {
		HAL_UART_Transmit(&huart1, (uint8_t*) command + command_pos, 1, 1);
		command_pos += 1;
		if (command_pos >= COMMAND_LEN)
			command_pos = 0;
		command_len -= 1;
	}

	if (config_step < CONFIG_DONE && ++config_wait == 0) {
		if (config_step == CONFIG_NONE) {
			send_config("AT+QSCLK=0");
		} else if (config_step == CONFIG_ECHO_SEND) {
			send_config("ATE1");
			config_step = CONFIG_ECHO_SENT;
		} else if (config_step == CONFIG_CLOSE_SEND) {
			send_config("AT+QICLOSE=0");
			config_step = CONFIG_CLOSE_SENT;
		} else if (config_step == CONFIG_OPEN_SEND) {
			send_config("AT+QIOPEN=1,0,\"UDP\",\"52.4.126.47\",1973,0,1");
			config_step = CONFIG_OPEN_SENT;
		}
	}
}

void send_radio_packet(const char *text) {
	write_radio_raw("AT+QISEND=0,");
	write_radio_int(strlen(text));
	write_radio_raw(",");
	write_radio_raw(text);
	write_radio_raw("\r");
}
