/*
 * uart_handler.c
 *
 *  Created on: 2019. �pr. 26.
 *      Author: gyooo
 */

#include "console_handler.h"
#include "usart.h"
#include "radio_handler.h"
#include <string.h>
#include <stdlib.h>

enum {
	CONSOLE_LEN = 128, // input command buffer
	REPORT_LEN = 1024, // output report buffer
};
char console[CONSOLE_LEN];
int console_pos = 0;
int booted = 0;

char report[REPORT_LEN];
int report_pos = 0;
int report_len = 0;

int is_console_rx_ready() {
	return (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) ? SET : RESET) == SET;
}

int is_console_tx_ready() {
	return (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) ? SET : RESET) == SET;
}

void handle_console() {
	if (!booted) {
		booted = 1;
		write_console("booted\n");
	}

	if (is_console_rx_ready()) {
		uint8_t data;
		if (HAL_UART_Receive(&huart2, &data, 1, 1) == HAL_OK) {
			if (data == '\n' || data == '\r') {
				data = 0;
			}
			if (console_pos < CONSOLE_LEN) {
				console[console_pos++] = data;
				if (data == 0 && console_pos > 1)
					process_command(console);
			}
			if (data == 0)
				console_pos = 0;
		}
	}

	if (report_len > 0 && is_console_tx_ready()) {
		HAL_UART_Transmit(&huart2, (uint8_t*) report + report_pos, 1, 1);
		report_pos += 1;
		if (report_pos >= REPORT_LEN)
			report_pos = 0;
		report_len -= 1;
	}
}

void write_console(const char *text) {
	while (*text != 0 && report_len < REPORT_LEN) {
		int pos = report_pos + report_len;
		if (pos >= REPORT_LEN)
			pos -= REPORT_LEN;
		report[pos] = *(text++);
		report_len += 1;
	}
}

void write_console_int(int value) {
	char buffer[20];
	itoa(value, buffer, 10);
	write_console(buffer);
}

void process_command(const char *text) {
	if (strcmp(text, "toggle") == 0) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	} else if (strncmp(text, "AT", 2) == 0) {
		write_console("at cmd: ");
		write_console(text);
		write_console("\n");
		write_radio_raw(text);
		write_radio_raw("\r");
	} else if (strncmp(text, "send ", 5) == 0) {
		write_console("sending: ");
		write_console(text + 5);
		write_console("\n");
		send_radio_packet(text + 5);
	} else {
		write_console("unknown: ");
		write_console(text);
		write_console("\n");
	}
}
