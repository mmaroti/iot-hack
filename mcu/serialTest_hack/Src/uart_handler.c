/*
 * uart_handler.c
 *
 *  Created on: 2019. ápr. 26.
 *      Author: gyooo
 */

#include "uart_handler.h"
#include "usart.h"

int isRxReady(UART_HandleTypeDef *huart) {
	return __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
}


void handleConsole() {
	//usart2
	if(1 || isRxReady(&huart2)) {
		char temp;
		HAL_UART_Receive(&huart2, &temp, 1, 0);
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	}
}


void handleRadio() {
	//usart1

}
