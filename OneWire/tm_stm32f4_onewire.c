#include "tm_stm32f4_onewire.h"

GPIO_InitTypeDef TM1_OneWire_GPIO_InitDef;
GPIO_InitTypeDef TM2_OneWire_GPIO_InitDef;
GPIO_InitTypeDef TM3_OneWire_GPIO_InitDef;
GPIO_InitTypeDef TM4_OneWire_GPIO_InitDef;


void TM_OneWire_Init(void) {
	// Timer 2, potrzebny funkcji DelayuS
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //w³¹czenie zegara
	TIM_TimeBaseInitTypeDef TIM_InitStruct; //struktura inicjalizacyjna
	TIM_InitStruct.TIM_ClockDivision = 0; //dzielnik 0
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up; // licznik w gore
	TIM_InitStruct.TIM_Period = 65535; // okres licznika
	TIM_InitStruct.TIM_Prescaler = 72; // preskaler 72
	TIM_TimeBaseInit(TIM2, &TIM_InitStruct); // inicjalizacja TIM2
	TIM_Cmd(TIM2, ENABLE);

	
	RCC_AHB1PeriphClockCmd(TM1_ONEWIRE_RCC, ENABLE);
	TM1_OneWire_GPIO_InitDef.GPIO_Pin = TM1_ONEWIRE_PIN;
	TM1_OneWire_GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	TM1_OneWire_GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	TM1_OneWire_GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_UP;
	TM1_OneWire_GPIO_InitDef.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(TM1_ONEWIRE_PORT, &TM1_OneWire_GPIO_InitDef);
	
	RCC_AHB1PeriphClockCmd(TM2_ONEWIRE_RCC, ENABLE);
	TM2_OneWire_GPIO_InitDef.GPIO_Pin = TM2_ONEWIRE_PIN;
	TM2_OneWire_GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	TM2_OneWire_GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	TM2_OneWire_GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_UP;
	TM2_OneWire_GPIO_InitDef.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(TM2_ONEWIRE_PORT, &TM2_OneWire_GPIO_InitDef);

	RCC_AHB1PeriphClockCmd(TM3_ONEWIRE_RCC, ENABLE);
	TM3_OneWire_GPIO_InitDef.GPIO_Pin = TM3_ONEWIRE_PIN;
	TM3_OneWire_GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	TM3_OneWire_GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	TM3_OneWire_GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_UP;
	TM3_OneWire_GPIO_InitDef.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(TM3_ONEWIRE_PORT, &TM3_OneWire_GPIO_InitDef);
	
	RCC_AHB1PeriphClockCmd(TM4_ONEWIRE_RCC, ENABLE);
	TM4_OneWire_GPIO_InitDef.GPIO_Pin = TM4_ONEWIRE_PIN;
	TM4_OneWire_GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	TM4_OneWire_GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	TM4_OneWire_GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_UP;
	TM4_OneWire_GPIO_InitDef.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(TM4_ONEWIRE_PORT, &TM4_OneWire_GPIO_InitDef);
}


uint8_t TM_OneWire_Reset(uint8_t nr) {
	uint8_t i;
	
	if(nr==1)
	{
		//Line low, and wait 480us
		TM1_ONEWIRE_LOW;
		TM1_ONEWIRE_OUTPUT;
		TM_ONEWIRE_DELAY(480);

		//Release line and wait for 80us
		TM1_ONEWIRE_INPUT;
		TM_ONEWIRE_DELAY(60);

		//Check bit value
		i = GPIO_ReadInputDataBit(TM1_ONEWIRE_PORT, TM1_ONEWIRE_PIN);
	} else if(nr==2) {
		//Line low, and wait 480us
		TM2_ONEWIRE_LOW;
		TM2_ONEWIRE_OUTPUT;
		TM_ONEWIRE_DELAY(480);
		//Release line and wait for 80us
		TM2_ONEWIRE_INPUT;
		TM_ONEWIRE_DELAY(60);
		//Check bit value
		i = GPIO_ReadInputDataBit(TM2_ONEWIRE_PORT, TM2_ONEWIRE_PIN);
	} else if(nr==3) {
		//Line low, and wait 480us
		TM3_ONEWIRE_LOW;
		TM3_ONEWIRE_OUTPUT;
		TM_ONEWIRE_DELAY(480);
		//Release line and wait for 80us
		TM3_ONEWIRE_INPUT;
		TM_ONEWIRE_DELAY(60);
		//Check bit value
		i = GPIO_ReadInputDataBit(TM3_ONEWIRE_PORT, TM3_ONEWIRE_PIN);
	} else if(nr==4) {
		//Line low, and wait 480us
		TM4_ONEWIRE_LOW;
		TM4_ONEWIRE_OUTPUT;
		TM_ONEWIRE_DELAY(480);
		//Release line and wait for 80us
		TM4_ONEWIRE_INPUT;
		TM_ONEWIRE_DELAY(60);
		//Check bit value
		i = GPIO_ReadInputDataBit(TM4_ONEWIRE_PORT, TM4_ONEWIRE_PIN);
	}
	TM_ONEWIRE_DELAY(420);
	//Return value of presence pulse, 0 = OK, 1 = ERROR
	return i;
}


uint8_t TM_OneWire_Reset_All(void) {
	uint8_t i=0;

	TM1_ONEWIRE_LOW;
	TM1_ONEWIRE_OUTPUT;
	TM2_ONEWIRE_LOW;
	TM2_ONEWIRE_OUTPUT;
	TM3_ONEWIRE_LOW;
	TM3_ONEWIRE_OUTPUT;
	TM4_ONEWIRE_LOW;
	TM4_ONEWIRE_OUTPUT;

	TM_ONEWIRE_DELAY(480);

	//Release line and wait for 80us
	TM1_ONEWIRE_INPUT;
	TM2_ONEWIRE_INPUT;
	TM3_ONEWIRE_INPUT;
	TM4_ONEWIRE_INPUT;

	TM_ONEWIRE_DELAY(60);

	//Check bit value
	if(!GPIO_ReadInputDataBit(TM1_ONEWIRE_PORT, TM1_ONEWIRE_PIN)) i|=0x01;
	if(!GPIO_ReadInputDataBit(TM2_ONEWIRE_PORT, TM2_ONEWIRE_PIN)) i|=0x02;
	if(!GPIO_ReadInputDataBit(TM3_ONEWIRE_PORT, TM3_ONEWIRE_PIN)) i|=0x04;
	if(!GPIO_ReadInputDataBit(TM4_ONEWIRE_PORT, TM4_ONEWIRE_PIN)) i|=0x08;

	TM_ONEWIRE_DELAY(420);
	//Return value of presence pulse, 0 = OK, 1 = ERROR
	return i;
}

//////////////////////////////////////////////////////

void TM_OneWire_ReadBit(uint8_t *mask, uint8_t *bits) {
	//Line low
	bits[0] = 0;
	bits[1] = 0;
	bits[2] = 0;
	bits[3] = 0;
	//startTim();
	if(*mask&0x01) {
		TM1_ONEWIRE_LOW;
		TM1_ONEWIRE_OUTPUT;
		TM1_ONEWIRE_INPUT;
		TM_ONEWIRE_DELAY(10);
		if (GPIO_ReadInputDataBit(TM1_ONEWIRE_PORT, TM1_ONEWIRE_PIN))
			bits[0] = 1;
	}
	if(*mask&0x02) {
		TM2_ONEWIRE_LOW;
		TM2_ONEWIRE_OUTPUT;
		TM2_ONEWIRE_INPUT;
		TM_ONEWIRE_DELAY(10);
		if (GPIO_ReadInputDataBit(TM2_ONEWIRE_PORT, TM2_ONEWIRE_PIN))
				bits[1] = 1;
	}
	if(*mask&0x04) {
		TM3_ONEWIRE_LOW;
		TM3_ONEWIRE_OUTPUT;
		TM3_ONEWIRE_INPUT;
		TM_ONEWIRE_DELAY(10);
		if (GPIO_ReadInputDataBit(TM3_ONEWIRE_PORT, TM3_ONEWIRE_PIN))
			bits[2] = 1;
	}
	if(*mask&0x08) {
		TM4_ONEWIRE_LOW;
		TM4_ONEWIRE_OUTPUT;
		TM4_ONEWIRE_INPUT;
		TM_ONEWIRE_DELAY(10);
		if (GPIO_ReadInputDataBit(TM4_ONEWIRE_PORT, TM4_ONEWIRE_PIN))
			bits[3] = 1;
	}

	TM_ONEWIRE_DELAY(20); //Wait 45us to complete 60us period
	//printf("\n\r time- %d us \n\r",stopTim());
}

/////////////////////////////////////////////////////

void TM_OneWire_ReadByte(uint8_t *mask, uint8_t *bytes) {
	uint8_t i = 8, bits[4];

	bytes[0]=0;
	bytes[1]=0;
	bytes[2]=0;
	bytes[3]=0;

	while (i--) {
		bytes[0] >>= 1;
		bytes[1] >>= 1;
		bytes[2] >>= 1;
		bytes[3] >>= 1;

		TM_OneWire_ReadBit(mask, bits);

		bytes[0] |= (bits[0] << 7);
		bytes[1] |= (bits[1] << 7);
		bytes[2] |= (bits[2] << 7);
		bytes[3] |= (bits[3] << 7);
	}
}


/////////////////////////////////////////////////

void TM_OneWire_WriteBit(uint8_t *maska, uint8_t bit) {
	//startTim();
	if(*maska&0x01) {
		TM1_ONEWIRE_LOW; //Line low
		TM1_ONEWIRE_OUTPUT;
		if (bit) TM1_ONEWIRE_INPUT;
	}
	if(*maska&0x02) {
		TM2_ONEWIRE_LOW;
		TM2_ONEWIRE_OUTPUT;
		if (bit) TM2_ONEWIRE_INPUT;
	}
	if(*maska&0x04) {
		TM3_ONEWIRE_LOW;
		TM3_ONEWIRE_OUTPUT;
		if (bit) TM3_ONEWIRE_INPUT;
	}
	if(*maska&0x08) {
		TM4_ONEWIRE_LOW;
		TM4_ONEWIRE_OUTPUT;
		if (bit) TM4_ONEWIRE_INPUT;
	}
	//printf("\n\r time- %d us \n\r",stopTim());
	TM_ONEWIRE_DELAY(1);

	TM_ONEWIRE_DELAY(50); //Wait for 60 us and release the line
	TM1_ONEWIRE_INPUT;
	TM2_ONEWIRE_INPUT;
	TM3_ONEWIRE_INPUT;
	TM4_ONEWIRE_INPUT;
}

////////////////////////////////////////////////////

void TM_OneWire_WriteByte(uint8_t *mask, uint8_t byte) {
	uint8_t i = 8;
	while (i--) {
		TM_OneWire_WriteBit(mask, byte & 0x01); //LSB bit is first
		byte >>= 1;
	}
}

////////////////////////////////////////////

//odczyt temp.
void TM_read_temp(uint8_t *mask, TM_temp_struct *sensors)
{
	uint8_t scratchpad1[9];
	uint8_t scratchpad2[9];
	uint8_t scratchpad3[9];
	uint8_t scratchpad4[9];
	uint8_t i=0;
	uint8_t msb = 0, lsb = 0, minus = 0;
	uint8_t bytes[4];

	for(i=0;i< SCRATCHPAD_SIZE;i++) {
		TM_OneWire_ReadByte(mask, bytes);
		scratchpad1[i] = bytes[0];
		scratchpad2[i] = bytes[1];
		scratchpad3[i] = bytes[2];
		scratchpad4[i] = bytes[3];
	}

	sensors[0].present=0;
	sensors[1].present=0;
	sensors[2].present=0;
	sensors[3].present=0;

	if(*mask&0x01) {
		sensors[0].present=1;

		if (crc8(scratchpad1, 8) != scratchpad1[8])
			sensors[0].crc_err=1;
		else
			sensors[0].crc_err=0;

		msb = 0;
		lsb = 0;
		minus = 0;

		lsb = scratchpad1[0]; 				  //czytamy LSB i MSB przechowujace temperature
		msb = scratchpad1[1];
		if (msb & 0x80) {       			  //dla liczb ujemnych negacja i +1
			msb=~msb;
			lsb=~lsb+1;
			sensors[0].minus=1;
		} else
			sensors[0].minus=0;

		sensors[0].calkowita_temp = (uint8_t) ((uint8_t) (msb&0x7)<<4 ) | ((uint8_t) (lsb&0xf0)>>4 );    //wyodrêbnia ca³kowit¹ wartoœæ temperatury
		sensors[0].ulamkowa_temp = ((lsb & 0x0F)*625)/1000;            //wyodrebnia ulamkowa czesc temperatury
	}

	if(*mask&0x02) {
		sensors[1].present=1;
		if (crc8(scratchpad2, 8) != scratchpad2[8])
			sensors[1].crc_err=1;
		else
			sensors[1].crc_err=0;

		msb = 0;
		lsb = 0;
		minus = 0;

		lsb = scratchpad2[0]; 				  //czytamy LSB i MSB przechowujace temperature
		msb = scratchpad2[1];
		if (msb & 0x80) {       			  //dla liczb ujemnych negacja i +1
			msb=~msb;
			lsb=~lsb+1;
			sensors[1].minus=1;
		} else
			sensors[1].minus=0;

		sensors[1].calkowita_temp = (uint8_t) ((uint8_t) (msb&0x7)<<4 ) | ((uint8_t) (lsb&0xf0)>>4 );    //wyodrêbnia ca³kowit¹ wartoœæ temperatury
		sensors[1].ulamkowa_temp = ((lsb & 0x0F)*625)/1000;            //wyodrebnia ulamkowa czesc temperatury
	}

	if(*mask&0x04) {
		sensors[2].present=1;
		if (crc8(scratchpad3, 8) != scratchpad3[8])
			sensors[2].crc_err=1;
		else
			sensors[2].crc_err=0;

		msb = 0;
		lsb = 0;

		lsb = scratchpad3[0]; 				  //czytamy LSB i MSB przechowujace temperature
		msb = scratchpad3[1];
		if (msb & 0x80) {       			  //dla liczb ujemnych negacja i +1
			msb=~msb;
			lsb=~lsb+1;
			sensors[2].minus=1;
		} else
			sensors[2].minus=0;

		sensors[2].calkowita_temp = (uint8_t) ((uint8_t) (msb&0x7)<<4 ) | ((uint8_t) (lsb&0xf0)>>4 );    //wyodrêbnia ca³kowit¹ wartoœæ temperatury

		sensors[2].ulamkowa_temp = ((lsb & 0x0F)*625)/1000;            //wyodrebnia ulamkowa czesc temperatury
	}

	if(*mask&0x08) {
		sensors[3].present=1;
		if (crc8(scratchpad4, 8) != scratchpad4[8])
			sensors[3].crc_err=1;
		else
			sensors[3].crc_err=0;


		msb = 0;
		lsb = 0;

		lsb = scratchpad4[0]; 				  //czytamy LSB i MSB przechowujace temperature
		msb = scratchpad4[1];
		if (msb & 0x80) {       			  //dla liczb ujemnych negacja i +1
			msb=~msb;
			lsb=~lsb+1;
			sensors[3].minus=1;
		} else
			sensors[3].minus=0;

		sensors[3].calkowita_temp = (uint8_t) ((uint8_t) (msb&0x7)<<4 ) | ((uint8_t) (lsb&0xf0)>>4 );    //wyodrêbnia ca³kowit¹ wartoœæ temperatury
		sensors[3].ulamkowa_temp = ((lsb & 0x0F)*625)/1000;            //wyodrebnia ulamkowa czesc temperatury
	}
}

unsigned char crc8 ( uint8_t *data_in, uint16_t number_of_bytes_to_read )
{
	uint8_t	 crc;
	uint16_t loop_count;
	uint8_t  bit_counter;
	uint8_t  data;
	uint8_t  feedback_bit;

	crc = CRC8INIT;

	for (loop_count = 0; loop_count != number_of_bytes_to_read; loop_count++)
	{
		data = data_in[loop_count];

		bit_counter = 8;
		do {
			feedback_bit = (crc ^ data) & 0x01;

			if ( feedback_bit == 0x01 ) {
				crc = crc ^ CRC8POLY;
			}
			crc = (crc >> 1) & 0x7F;
			if ( feedback_bit == 0x01 ) {
				crc = crc | 0x80;
			}

			data = data >> 1;
			bit_counter--;

		} while (bit_counter > 0);
	}

	return crc;
}

void delayuS(uint16_t time) {
	TIM_SetCounter(TIM2,0); //zerowanie licznika
	while (TIM_GetCounter(TIM2)<=time); //odczyt CNT i porównanie
}

void startTim(void) {
	TIM_SetCounter(TIM2,0); //zerowanie licznika
}

uint32_t stopTim(void) {
	return TIM_GetCounter(TIM2);
}
