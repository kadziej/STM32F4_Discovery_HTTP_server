/*
 * usart.h
 *
 *  Created on: Apr 25, 2011
 *      Author: kuba
 */

#ifndef USART_H_
#define USART_H_

#include "stm32f10x.h"
#include "stm32f10x_usart.h"

int GetKey (void);
void usart_printf(char * tablica);
void usartSetup (void);
void SendChar (signed char ch);
void get_line (char *buff, uint8_t len);
u8 USART_Scanf(u32 value);
void usart_const_printf(const char *tablica);
#endif /* USART_H_ */
