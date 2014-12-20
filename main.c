/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011
  * @brief   Main program body
  ******************************************************************************

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_conf.h"
#include "stm32f4x7_eth.h"
#include "netconf.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "tcpip.h"
#include "httpserver-netconn.h"
#include "stm32f4_discovery_debug.h"
#include "stm32f4_discovery_sdio_sd.h"
#include "ff.h"
#include "tm_stm32f4_onewire.h"
#include <stdio.h>

#include "gpio_adc.h"
#include "settings.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/*--------------- Tasks Priority -------------*/
#define DHCP_TASK_PRIO   ( tskIDLE_PRIORITY + 2 )      
#define LED_TASK_PRIO    ( tskIDLE_PRIORITY + 1 )
#define SNTP_TASK_PRIO    ( tskIDLE_PRIORITY + 2 )
#define ONE_WIRE_TASK_PRIO    ( tskIDLE_PRIORITY + 1 )


/* Private variables ---------------------------------------------------------*/

void Delay(uint32_t nCount);
void delayuS(uint16_t time);
void fault_err (FRESULT rc);

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void ToggleLed4(void * pvParameters);
void ToggleLed5(void * pvParameters);
void sntp_threat(void * pvParameters);
void OneWire_threat(void * pvParameters);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */


xSemaphoreHandle Sem_TimeUpdated=0;
xSemaphoreHandle SD_gatekeeper=0;
xSemaphoreHandle SNTP_last_updated_clear_semaphore=0;

xQueueHandle powerontime_queue = NULL;
xQueueHandle last_sntp_response_queue = NULL;

Settings_struct settings;


int main(void)
{
	STM32f4_Discovery_Debug_Init();
	default_settings(&settings);
	read_settings_from_SD(&settings);

	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);

	vSemaphoreCreateBinary( Sem_TimeUpdated );
	vSemaphoreCreateBinary( SNTP_last_updated_clear_semaphore );

	SD_gatekeeper=xSemaphoreCreateMutex();

	powerontime_queue = xQueueCreate(1,sizeof(uint32_t));
	last_sntp_response_queue = xQueueCreate(1,sizeof(uint32_t));

	TM_OneWire_Init();
	RTC_Config();

  /* configure Ethernet (GPIOs, clocks, MAC, DMA) */ 
	ETH_BSP_Config();
    
  /* Initilaize the LwIP stack */
	LwIP_Init(&settings);
  
	 /* Initialize webserver demo */
	http_server_netconn_init();

#ifdef USE_DHCP
	/* Start DHCPClient */
	xTaskCreate(LwIP_DHCP_task, "DHCPClient", configMINIMAL_STACK_SIZE * 2, NULL,DHCP_TASK_PRIO, NULL);
#endif

  xTaskCreate(ToggleLed4, "LED4", configMINIMAL_STACK_SIZE, NULL, LED_TASK_PRIO, NULL);
  xTaskCreate(ToggleLed5, "LED5", configMINIMAL_STACK_SIZE*4, NULL, LED_TASK_PRIO, NULL);
  xTaskCreate(OneWire_threat, "1-Wire", configMINIMAL_STACK_SIZE*8, NULL, ONE_WIRE_TASK_PRIO, NULL);
  xTaskCreate(sntp_threat, "SNTP", configMINIMAL_STACK_SIZE*4, NULL, SNTP_TASK_PRIO, NULL);
  
  /* Start scheduler */
  vTaskStartScheduler();

  /* We should never get here as control is now taken by the scheduler */
  for( ;; );
}






void OneWire_threat(void * pvParameters)
{
	int8_t i,mask, temp1,temp2;
	uint8_t potega, TimeUpdated=0, onewire_delay;
	TM_temp_struct temp_sensors[4];
	onewire_delay=settings.ow_delay;
	for ( ;; ) {

		STM_EVAL_LEDOn(LED6);
		mask=TM_OneWire_Reset_All();
		if(mask)
		{
			TM_OneWire_WriteByte(&mask,TM_ONEWIRE_CMD_SKIPROM);
			TM_OneWire_WriteByte(&mask,TM_ONEWIRE_CMD_CONVERT_T);
		}
	  	STM_EVAL_LEDOff(LED6);
	  	//printf(" maska-(%d) \n\r",mask);

	  	vTaskDelay(750); //750ms

	  	STM_EVAL_LEDOn(LED6);
	  	mask=TM_OneWire_Reset_All();
	  	if(mask)
	  	{
	  		TM_OneWire_WriteByte(&mask,TM_ONEWIRE_CMD_SKIPROM);
	  		TM_OneWire_WriteByte(&mask,TM_ONEWIRE_CMD_RSCRATCHPAD);

	  		TM_read_temp(&mask, temp_sensors);
	  		RTC_TimeShow();
	  		for(i=0;i<4;i++)
	  		{
	  			if(temp_sensors[i].present)
	  			{
	  				if(temp_sensors[i].crc_err)
	  				{
	  					printf("[%d] crc8 ",i);
	  					potega=(0x01<<i);
	  					if(!TM_OneWire_Reset(i+1))
	  						{
	  						TM_OneWire_WriteByte(&potega,TM_ONEWIRE_CMD_SKIPROM);
	  						TM_OneWire_WriteByte(&potega,TM_ONEWIRE_CMD_RSCRATCHPAD);
	  						TM_read_temp(&potega, temp_sensors);
	  						}
	  				}
	  				if(!temp_sensors[i].crc_err)
	  				{
	  					if(temp_sensors[i].minus)
	  						printf("[%d] -%d.%d  ",i,temp_sensors[i].calkowita_temp, temp_sensors[i].ulamkowa_temp);
	  					else
	  						printf("[%d] %d.%d  ",i,temp_sensors[i].calkowita_temp, temp_sensors[i].ulamkowa_temp);
	  					if(xSemaphoreTake(Sem_TimeUpdated,1)) TimeUpdated=1;
	  					if(TimeUpdated==1){
	  							SD_write_temperature(i+1, temp_sensors[i].minus, temp_sensors[i].calkowita_temp, temp_sensors[i].ulamkowa_temp);
	  					}
	  				}

	  			} else {
	  				printf("[%d] N/P   ",i);
	  			}
	  		}

	  	}
	  	STM_EVAL_LEDOff(LED6);
	  	printf(" \n\r");
	  	vTaskDelay(onewire_delay*1000);
  }
}

void sntp_threat(void * pvParameters)
{
	xSemaphoreTake(Sem_TimeUpdated,1);
	for ( ;; ) {
		while(sntp_request()) // !0 - Fail, 0 - OK
		{
			STM_EVAL_LEDOff(LED3);
			vTaskDelay(2000);
		}
		xSemaphoreGive(Sem_TimeUpdated);
		xSemaphoreGive(SNTP_last_updated_clear_semaphore);
		//SinceSNTPupdate = 0;


		STM_EVAL_LEDOff(LED3);
		vTaskDelay(500);
		STM_EVAL_LEDOn(LED3);
		vTaskDelay(600000); //10 minut
	}
}

void ToggleLed4(void * pvParameters)
{
  for ( ;; ) {
    /* Toggle LED4 each 300ms */
    STM_EVAL_LEDToggle(LED4);
    vTaskDelay(300);
  }
}
void ToggleLed5(void * pvParameters)
{
	uint32_t PowerOnTime = 0;
	uint32_t SinceSNTPupdate = 0;
	uint32_t temp = 0;

	for ( ;; ) {
		//RTC_TimeShow();
		PowerOnTime+=1;
		SinceSNTPupdate+=1;

		if(xSemaphoreTake(SNTP_last_updated_clear_semaphore,0))
			SinceSNTPupdate=0;

		xQueueReceive(powerontime_queue, &temp, 0);
		xQueueReceive(last_sntp_response_queue, &temp, 0);

		xQueueSend(powerontime_queue, &PowerOnTime, 0);
		xQueueSend(last_sntp_response_queue, &SinceSNTPupdate, 0);

		vTaskDelay(1000);
	}
}

/**
  * @brief  Initializes the STM324xG-EVAL's LCD and LEDs resources.
  * @param  None
  * @retval None
  */
void fault_err (FRESULT rc)
{
  const char *str =
                    "OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
                    "INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0" "WRITE_PROTECTED\0"
                    "INVALID_DRIVE\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0"
                    "LOCKED\0" "NOT_ENOUGH_CORE\0" "TOO_MANY_OPEN_FILES\0";
  FRESULT i;

  for (i = (FRESULT)0; i != rc && *str; i++) {
    while (*str++) ;
  }
  printf("rc=%u FR_%s\n\r", (UINT)rc, str);
  STM_EVAL_LEDOn(LED6);
  while(1);
}

/**
  * @brief  Delay
  * @param  None
  * @retval None
  */
void Delay(uint32_t nCount)
{
  uint32_t index = 0;
  for (index = (100000 * nCount); index != 0; index--);
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif


/*********** Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.*****END OF FILE****/
