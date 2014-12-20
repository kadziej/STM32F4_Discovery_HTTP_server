#include "usart.h"
#include "string.h"

void SendChar (signed char ch)  {

    USART1->DR = ( ch & 0x1FF);
    while (!(USART1->SR & USART_FLAG_TXE));

}

int GetKey (void)  {

  while (!(USART1->SR & USART_FLAG_RXNE));

  return ((int)(USART1->DR & 0x1FF));
}

void usart_printf(char *tablica) {
int i=0;

	for(i=0;i<strlen(tablica); i++)
	{
		USART1->DR = ( tablica[i] & 0x1FF);
		while (!(USART1->SR & USART_FLAG_TXE));
	}

}

void usart_const_printf(const char *tablica) {
int i=0;

	for(i=0;i<strlen(tablica); i++)
	{
		USART1->DR = ( tablica[i] & 0x1FF);
		while (!(USART1->SR & USART_FLAG_TXE));
	}

}




void get_line (char *buff, uint8_t len)
{
	uint8_t i;
	char c;
 
	i = 0;
	for (;;) {
		c = GetKey();
		SendChar(c);
		if (c == '\r') break;
		if ((c == '\b') && i) i--;
		if ((c >= ' ') && (i < len - 1))
				buff[i++] = c;
	}
	buff[i] = 0;
}

u8 USART_Scanf(u32 value)
{
  u32 index;
  u32 tmp[2] = {0, 0};
  do {
	  index=0;

	  while (index < 2)
	  {

	  	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) ;		// Loop until data has been received

    	tmp[index++] = (uint16_t)(USART1->DR & (uint16_t)0x01FF);

    	SendChar(tmp[index - 1]);											//make visible

    	if((tmp[index - 1])=='\n' || (tmp[index - 1])=='\r')
    	{
      		index = 5;
    		break;
    	}

    	if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
    	{
    		//usart_printf("\n\rErrror! Type again: ");
    		SendChar((char) 0x8);
    		index--;
    	}
  	  }

  	  // Calculate the Corresponding value
  	  if (index == 5)
  		  index = tmp[0] - 0x30;
  	  else
  		  index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
  	  /* Checks */
  	  if (index > value)
  	  {
  		  //usart_printf("\n\rIncorrect value, try again\r\n");
  		  index = 3;
  	  }
  }while(index==3);

  //usart_printf("\n\rSet!");
  return index;
}

void usartSetup (void) {
	   USART_InitTypeDef USART_InitStructure;
	   GPIO_InitTypeDef GPIO_InitStructure;

	   USART_InitStructure.USART_BaudRate = 9600;
	   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	   USART_InitStructure.USART_StopBits = USART_StopBits_1;
	   USART_InitStructure.USART_Parity = USART_Parity_No ;
	   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // UART1 Tx PA.9
	   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	   GPIO_Init(GPIOA, &GPIO_InitStructure);

	   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // UART1 Rx PA.10
		 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	   GPIO_Init(GPIOA, &GPIO_InitStructure);

	   USART_Init(USART1, &USART_InitStructure);
	   USART_Cmd(USART1, ENABLE);
		
  }
