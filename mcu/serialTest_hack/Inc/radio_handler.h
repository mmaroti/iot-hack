/*
 * radio_handler.h
 *
 *  Created on: 2019. �pr. 26.
 *      Author: gyooo
 */

#ifndef RADIO_HANDLER_H_
#define RADIO_HANDLER_H_

void handle_radio();
void write_radio_raw(const char *text); // raw commands
void write_radio_int(int value);
void send_radio_packet(const char *text);

#endif /* RADIO_HANDLER_H_ */
