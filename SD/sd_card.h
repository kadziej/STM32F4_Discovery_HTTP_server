/**************************************************************************//*****
 * @file     
 * @brief    
 ********************************************************************************/
#ifndef SD_CARD
#define SD_CARD

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "stm32f4xx.h"
#include "settings.h"

typedef struct {
	uint8_t sensor_selected;
	uint8_t name[32];
	int32_t count_to_show;
	int32_t count_in_database;
}  SD_Stat;

void SD_test(void);

void read_settings_from_SD(Settings_struct *settings);

void SD_read_download_day(char *nr, char *get_day, struct netconn *conn);

void SD_day_select(SD_Stat *sensors_info, struct netconn *conn);
void SD_read_temperature(char *period, struct netconn *conn, SD_Stat *stat, uint8_t *error);
void SD_read_temperature_day(char *get_day, struct netconn *conn, SD_Stat *sensors_info);


void SD_write_temperature(uint8_t nr, uint8_t minus, uint8_t *calkowita_temp, uint8_t *ulamkowa_temp);
void SD_write_data_from_sensor(uint8_t *nr, char *time_ch, char *date_ch, uint8_t *data_to_write);

void SD_read_Sensors_Names(SD_Stat *stat, uint8_t *error);

void SD_write_LOG(char * data_to_write);

#endif
/* --------------------------------- End Of File ------------------------------ */
