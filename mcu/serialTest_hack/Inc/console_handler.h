/*
 * uart_handler.h
 *
 *  Created on: 2019. �pr. 26.
 *      Author: gyooo
 */

#ifndef CONSOLE_HANDLER_H_
#define CONSOLE_HANDLER_H_

void handle_console();
void process_command(const char *text);
void write_console(const char* text);
void write_console_int(int value);

#endif /* CONSOLE_HANDLER_H_ */
