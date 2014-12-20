/**************************************************************************//*****
 * @file     
 * @brief    
 ********************************************************************************/
#ifndef SETTINGS
#define SETTINGS

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "stm32f4xx.h"

typedef struct {
	uint8_t ip[4];
	uint8_t ip_mask[4];
	uint8_t ip_gw[4];
	uint8_t DHCP_on;
	uint8_t ip_sntp[4];
	uint8_t ow_delay;
}  Settings_struct;

void default_settings(Settings_struct *ip_settings);

//void read_settings_from_SD(void);


#endif
/* --------------------------------- End Of File ------------------------------ */
