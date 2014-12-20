/**************************************************************************//*****
 * @file     
 * @brief    
 ********************************************************************************/

#include "stm32f4_discovery_debug.h"
#include "stm32f4_discovery_sdio_sd.h"
#include "tm_stm32f4_onewire.h"
#include "ff.h"



#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"

#include "settings.h"
#include <string.h>

extern xSemaphoreHandle SD_gatekeeper;

void default_settings(Settings_struct *ip_settings)
{
	ip_settings->ip[0]=192;
	ip_settings->ip[1]=168;
	ip_settings->ip[2]=1;
	ip_settings->ip[3]=150;

	ip_settings->ip_mask[0]=255;
	ip_settings->ip_mask[1]=255;
	ip_settings->ip_mask[2]=255;
	ip_settings->ip_mask[3]=0;

	ip_settings->ip_gw[0]=192;
	ip_settings->ip_gw[1]=168;
	ip_settings->ip_gw[2]=1;
	ip_settings->ip_gw[3]=254;

	ip_settings->ip_sntp[0]=149;
	ip_settings->ip_sntp[1]=156;
	ip_settings->ip_sntp[2]=44;
	ip_settings->ip_sntp[3]=126;

	ip_settings->ow_delay=60;
}



/* --------------------------------- End Of File ------------------------------ */
