#ifndef _GPIO_ADC_
#define _GPIO_ADC_

#include "stm32f4xx.h"

#define OUT1_RCC		RCC_AHB1Periph_GPIOD
#define OUT1_PORT		GPIOD
#define OUT1_PIN		GPIO_Pin_11

#define OUT2_RCC		RCC_AHB1Periph_GPIOA
#define OUT2_PORT		GPIOA
#define OUT2_PIN		GPIO_Pin_15

#define IN1_RCC			RCC_AHB1Periph_GPIOA
#define IN1_PORT		GPIOA
#define IN1_PIN			GPIO_Pin_5


void ADC_Configuration(void);
void ADC_Handler(char *pcInsert);

void LED5_off(void);
void LED5_on(void);
uint8_t LED5_status(void);

#endif
