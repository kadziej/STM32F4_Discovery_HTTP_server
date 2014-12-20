/**
 *	Onewire library for STM32F4 devices
 */
#ifndef TM_ONEWIRE_H
#define TM_ONEWIRE_H 100

/**
 * Includes
 */
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"


#define SCRATCHPAD_SIZE 9
#define CRC8INIT	0x00
#define CRC8POLY	0x18

#define TM_ONEWIRE_DELAY(x)		delayuS(x)

#define TM1_ONEWIRE_RCC			RCC_AHB1Periph_GPIOD
#define TM1_ONEWIRE_PORT			GPIOD
#define TM1_ONEWIRE_PIN			GPIO_Pin_1

#define TM2_ONEWIRE_RCC			RCC_AHB1Periph_GPIOD
#define TM2_ONEWIRE_PORT			GPIOD
#define TM2_ONEWIRE_PIN			GPIO_Pin_0

#define TM3_ONEWIRE_RCC			RCC_AHB1Periph_GPIOB
#define TM3_ONEWIRE_PORT			GPIOB
#define TM3_ONEWIRE_PIN			GPIO_Pin_9

#define TM4_ONEWIRE_RCC			RCC_AHB1Periph_GPIOB
#define TM4_ONEWIRE_PORT			GPIOB
#define TM4_ONEWIRE_PIN			GPIO_Pin_8



#define TM1_ONEWIRE_LOW			TM1_ONEWIRE_PORT->BSRRH = TM1_ONEWIRE_PIN;
#define TM1_ONEWIRE_HIGH		TM1_ONEWIRE_PORT->BSRRL = TM1_ONEWIRE_PIN;
#define TM1_ONEWIRE_INPUT		TM1_OneWire_GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN; GPIO_Init(TM1_ONEWIRE_PORT, &TM1_OneWire_GPIO_InitDef);
#define TM1_ONEWIRE_OUTPUT		TM1_OneWire_GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT; GPIO_Init(TM1_ONEWIRE_PORT, &TM1_OneWire_GPIO_InitDef);

#define TM2_ONEWIRE_LOW			TM2_ONEWIRE_PORT->BSRRH = TM2_ONEWIRE_PIN;
#define TM2_ONEWIRE_HIGH		TM2_ONEWIRE_PORT->BSRRL = TM2_ONEWIRE_PIN;
#define TM2_ONEWIRE_INPUT		TM2_OneWire_GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN; GPIO_Init(TM2_ONEWIRE_PORT, &TM2_OneWire_GPIO_InitDef);
#define TM2_ONEWIRE_OUTPUT		TM2_OneWire_GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT; GPIO_Init(TM2_ONEWIRE_PORT, &TM2_OneWire_GPIO_InitDef);

#define TM3_ONEWIRE_LOW			TM3_ONEWIRE_PORT->BSRRH = TM3_ONEWIRE_PIN;
#define TM3_ONEWIRE_HIGH		TM3_ONEWIRE_PORT->BSRRL = TM3_ONEWIRE_PIN;
#define TM3_ONEWIRE_INPUT		TM3_OneWire_GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN; GPIO_Init(TM3_ONEWIRE_PORT, &TM3_OneWire_GPIO_InitDef);
#define TM3_ONEWIRE_OUTPUT		TM3_OneWire_GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT; GPIO_Init(TM3_ONEWIRE_PORT, &TM3_OneWire_GPIO_InitDef);

#define TM4_ONEWIRE_LOW			TM4_ONEWIRE_PORT->BSRRH = TM4_ONEWIRE_PIN;
#define TM4_ONEWIRE_HIGH		TM4_ONEWIRE_PORT->BSRRL = TM4_ONEWIRE_PIN;
#define TM4_ONEWIRE_INPUT		TM4_OneWire_GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN; GPIO_Init(TM4_ONEWIRE_PORT, &TM4_OneWire_GPIO_InitDef);
#define TM4_ONEWIRE_OUTPUT		TM4_OneWire_GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT; GPIO_Init(TM4_ONEWIRE_PORT, &TM4_OneWire_GPIO_InitDef);



//OneWire commands
#define TM_ONEWIRE_CMD_RSCRATCHPAD			0xBE //Read Scratchpad
#define TM_ONEWIRE_CMD_WSCRATCHPAD			0x4E //Write Scratchpad
#define TM_ONEWIRE_CMD_CPYSCRATCHPAD		0x48
#define TM_ONEWIRE_CMD_RECEEPROM			0xB8
#define TM_ONEWIRE_CMD_RPWRSUPPLY			0xB4
#define TM_ONEWIRE_CMD_SEARCHROM			0xF0
#define TM_ONEWIRE_CMD_READROM				0x33
#define TM_ONEWIRE_CMD_MATCHROM				0x55
#define TM_ONEWIRE_CMD_SKIPROM				0xCC
#define TM_ONEWIRE_CMD_CONVERT_T			0x44

/**
 * Initialize OneWire bus
 *
 */

typedef struct
{
	uint8_t present;
	uint8_t crc_err;
	uint8_t minus;
	uint8_t calkowita_temp;
	uint8_t ulamkowa_temp;
	char temp_string[8];
}TM_temp_struct;


void TM_OneWire_Init(void);
uint8_t TM_OneWire_Reset(uint8_t nr);
uint8_t TM_OneWire_Reset_All(void);
void TM_OneWire_ReadBit(uint8_t *mask, uint8_t *bits);
void TM_OneWire_ReadByte(uint8_t *mask, uint8_t *bytes);
void TM_OneWire_WriteBit(uint8_t *maska, uint8_t bit);
void TM_OneWire_WriteByte(uint8_t *mask, uint8_t byte);
void TM_read_temp(uint8_t *mask, TM_temp_struct *sensors);
unsigned char crc8 ( uint8_t *data_in, uint16_t number_of_bytes_to_read );

void delayuS(uint16_t time);
void startTim(void);

uint32_t stopTim(void);

#endif

