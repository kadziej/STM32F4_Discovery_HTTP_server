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

#include "sd_card.h"
#include "settings.h"
#include <string.h>

extern xSemaphoreHandle SD_gatekeeper;

SD_Error Status = SD_OK;
FATFS filesystem;		/* volume lable */
FRESULT ret;			  /* Result code */
FIL file;				    /* File object */
DIR dir;				    /* Directory object */
FILINFO fno;			  /* File information object */
UINT bw, br;




void read_settings_from_SD(Settings_struct *settings)
{
	char *my_string;
	uint8_t i, buff[32],buff2[16],line[64];

	if (f_mount(0, &filesystem) != FR_OK) {
		printf("could not open filesystem \n\r");
	}
	//printf(" (data poprawna) ");
	strcpy(&buff, "settings.txt");

	if(f_open(&file, &buff, FA_READ) == FR_OK)
	{
		printf("otwarto plik settings\r\n");

		while(f_gets(line, sizeof line, &file))
		{
			//printf("%s",line);
			my_string = strtok(line,"=#");

			if(!strcmp(my_string,"ip_address")) {
				for(i=0;i<4;i++) {
					my_string = strtok(NULL,".");
					settings->ip[i]=atoi(my_string);
				}
				printf("rozpoznano ip_address=%d.%d.%d.%d\r\n",settings->ip[0],settings->ip[1],settings->ip[2],settings->ip[3]);
			} else if(!strcmp(my_string,"ip_mask")) {
				for(i=0;i<4;i++) {
					my_string = strtok(NULL,".");
					settings->ip_mask[i]=atoi(my_string);
				}
				printf("rozpoznano ip_mask=%d.%d.%d.%d\r\n",settings->ip_mask[0],settings->ip_mask[1],settings->ip_mask[2],settings->ip_mask[3]);
			} else if(!strcmp(my_string,"gw_addr")) {
				for(i=0;i<4;i++) {
					my_string = strtok(NULL,".");
					settings->ip_gw[i]=atoi(my_string);
				}
				printf("rozpoznano gw_addr=%d.%d.%d.%d\r\n",settings->ip_gw[0],settings->ip_gw[1],settings->ip_gw[2],settings->ip_gw[3]);
			} else if(!strcmp(my_string,"sntp_addr")) {
				for(i=0;i<4;i++) {
					my_string = strtok(NULL,".");
					settings->ip_sntp[i]=atoi(my_string);
				}
				printf("rozpoznano sntp_addr=%d.%d.%d.%d\r\n",settings->ip_sntp[0],settings->ip_sntp[1],settings->ip_sntp[2],settings->ip_sntp[3]);

			} else if(!strcmp(my_string,"ow_delay")) {
				my_string = strtok(NULL,".");
				settings->ow_delay=atoi(my_string);

				printf("rozpoznano ow_delay=%d\r\n",settings->ow_delay);

			} else {
				printf("nie rozpoznano ustawienia [%s]\r\n",my_string);
			}



		}
	} else {
		printf(">(brak pliku SETTINGS.txt)\r\n");
	}
	f_close(&file);
}





void SD_read_download_day(char *nr, char *get_day, struct netconn *conn)
{
	if(xSemaphoreTake(SD_gatekeeper,1000))
	{
		char *my_string;
		uint8_t buff[32],buff2[16],line[64];

		if (f_mount(0, &filesystem) != FR_OK) {
			printf("could not open filesystem \n\r");
		}
		ret = f_stat("sensors",&fno);
		if (ret) {
			printf("Nie znaleziono folderu sensors\n\r");
		}
		ret = f_chdir("sensors");
		if(ret) {
			xSemaphoreGive(SD_gatekeeper);
			return;
		}
		//sprintf(&buff2, "%s",*nr);
		ret = f_chdir(nr);
		if(ret) {
			xSemaphoreGive(SD_gatekeeper);
			return;
		}

		// Jestesmy w folderze
		if(strlen(get_day)==8 && get_day[2]=='-' && get_day[5]=='-')
		{
			printf(" (data poprawna) ");
			sprintf(&buff, "%s.txt", get_day);
			if(f_open(&file, &buff, FA_READ) == FR_OK)
			{
				strcpy(line, "\"Godzina\";\"Temperatura\"\r\n");
				netconn_write(conn, line, strlen(line), NETCONN_COPY);
				while(f_gets(line, sizeof line, &file))
				{
					netconn_write(conn, line, strlen(line), NETCONN_COPY);
				}
			} else {
				printf(" (brak pliku day) ");
			}
			f_close(&file);
		}

		xSemaphoreGive(SD_gatekeeper);
	} else printf("timeout:nie mozna pobrac semafora SD (read day)\n\r");

}







void SD_day_select(SD_Stat *sensors_info, struct netconn *conn)
{
	if(xSemaphoreTake(SD_gatekeeper,1000))
	{
		portCHAR PAGE[256];
		char *my_string;
		uint8_t buff[16];
		uint8_t i;
		PAGE[0]=0;

		uint8_t sensors_selected_ch[32];
		sensors_selected_ch[0]=0;

		for(i=1;i<=4;i++) // przygotowanie linku
		{
			if(sensors_info[i].sensor_selected) {
				sprintf((char *)buff, "/%d",i);
				strcat((char *)sensors_selected_ch, (char *)buff);
			}
		}



		if (f_mount(0, &filesystem) != FR_OK) {
			printf("could not open filesystem \n\r");
		}

		for(i=1;i<=4;i++) {
			if(!sensors_info[i].sensor_selected) continue;

			sprintf((char *)PAGE, "<p>%s :</p><div id=\"navigation2\"><ul>", sensors_info[i].name);
			netconn_write(conn, PAGE, strlen(PAGE), NETCONN_COPY);

			printf("Open directory\n\r");
			sprintf(&buff, "sensors/%d",i);
			ret = f_opendir(&dir, &buff);

			if (ret) {
				printf("Open root directory error\n\r");
			} else {
				printf("Directory listing...\n\r");
				for (;;) {
					ret = f_readdir(&dir, &fno);		/* Read a directory item */
					if (ret || !fno.fname[0]) {
						break;	/* Error or end of dir */
					}
					if (fno.fattrib & AM_DIR) {
						//printf("  <dir>  %s\n\r", fno.fname);
					} else {
						//printf("%8lu  %s\n\r", fno.fsize, fno.fname);
						my_string = strtok (fno.fname,".");
						printf("<file> %s\n\r", fno.fname);
						if(strlen(my_string)==8 && my_string[2]=='-' && my_string[5]=='-')
						{
							sprintf((char *)PAGE, "<li><a href=\"/sensors/day/%s%s\">%s</a></li>", my_string, sensors_selected_ch, my_string);
							netconn_write(conn, PAGE, strlen(PAGE), NETCONN_COPY);
							sprintf((char *)PAGE, "<li><a href=\"/download/%d/%s\" download=\"%s.csv\">csv</a></li>",i , my_string, my_string);
							netconn_write(conn, PAGE, strlen(PAGE), NETCONN_COPY);
						}

					}
				}
			}
			PAGE[0]=0;
			strcat((char *)PAGE, "</ul></div><br><br>" );
			netconn_write(conn, PAGE, strlen(PAGE), NETCONN_COPY);
		} // for i



		xSemaphoreGive(SD_gatekeeper);
	} else printf("timeout:nie mozna pobrac semafora SD (day select)\n\r");
}



void SD_read_temperature_day(char *get_day, struct netconn *conn, SD_Stat *sensors_info)
{
	if(xSemaphoreTake(SD_gatekeeper,1000))
	{

		uint8_t buff[32],buff2[16],line[64],date[16],time_now[16], first_line=1;
		uint8_t hour_now_int, minute_now_int, hour_int, minute_int,month_int;
		uint8_t i;
		char *hour, *minute, *second, *temp, *year, *month, *day, *hour_now, *minute_now;
		portCHAR PAGE[128];
		RTC_GetTime_char(&time_now);
		hour_now = strtok (time_now,":");
		minute_now = strtok (NULL,":");
		hour_now_int = atoi(hour_now);
		minute_now_int = atoi(minute_now);


		if (f_mount(0, &filesystem) != FR_OK) {
			printf("could not open filesystem \n\r");
		}
		ret = f_stat("sensors",&fno);
		if (ret) {
			printf("Nie znaleziono folderu sensors\n\r");
		}
		ret = f_chdir("sensors");
		if(ret) {
			xSemaphoreGive(SD_gatekeeper);
			return;
		}

		strcpy((char*)date, (char*)get_day);
		day = strtok (get_day,"-");
		month = strtok (NULL,"-");
		month_int=atoi(month);
		year = strtok (NULL,"-");
		for(i=1;i<=4;i++)
		{////////////////////////////////////////////////////////////////
			if(!sensors_info[i].sensor_selected) continue;

			sprintf(&PAGE, " { name: '%s',data: [",sensors_info[i].name);
			netconn_write(conn, PAGE, strlen(PAGE), NETCONN_COPY);

			sprintf(&buff, "%d/%s.txt", i, (char*)date);
			printf("[%s]\r\n", &buff);

			if(f_open(&file, &buff, FA_READ) == FR_OK)
			{
				while(f_gets(line, sizeof line, &file))
				{
					hour = strtok (line,":");
					minute = strtok (NULL,":");
					second = strtok (NULL,";");
					temp = strtok (NULL,",");

					sprintf(&PAGE, "[Date.UTC(20%s,%d,%s,%s,%s,%s), %s],", year, month_int-1, day, hour, minute, second, temp);
					netconn_write(conn, PAGE, strlen(PAGE), NETCONN_COPY);

				}
			} else {
				printf(" (brak pliku day) ");
			}
			f_close(&file);
			sprintf(&PAGE, "]},");
			netconn_write(conn, PAGE, strlen(PAGE), NETCONN_COPY);
		} // for
		xSemaphoreGive(SD_gatekeeper);
	} else printf("timeout:nie mozna pobrac semafora SD (read day)\n\r");

}

void SD_read_temperature(char *period, struct netconn *conn, SD_Stat *stat, uint8_t *error)
{

	if(xSemaphoreTake(SD_gatekeeper,1000))
	{
		uint8_t buff[32], buff2[16], line[64], date[16],time_now[16];
		uint8_t i, first_line=1, first_day=1;
		uint8_t hour_now_int, minute_now_int, hour_int, month_int, minute_int,skip=0, skip_count=0;
		int16_t day_counter, day_counter_temp;
		char *hour, *minute, *second, *temp, *year, *month, *day, *hour_now, *minute_now;
		portCHAR PAGE[128];

		stat->count_in_database=0;
		stat->count_to_show=0;

		RTC_GetTime_char(&time_now);
		hour_now = strtok (time_now,":");
		minute_now = strtok (NULL,":");
		hour_now_int = atoi(hour_now);
		minute_now_int = atoi(minute_now);


		if(!strcmp(period,"1h")) {
			skip=0;
			printf("wykryto 1h\n\r");
			if(hour_now_int==0)
			{
				hour_now_int=23;
				day_counter=1;
			} else {
				hour_now_int-=1;
				day_counter=0;
			}
		} else if(!strcmp(period,"2h")) {
			skip=0;
			printf("wykryto 2h\n\r");
			if(hour_now_int==0) {
				hour_now_int=22;
				day_counter=1;
			} else if(hour_now_int==1) {
				hour_now_int=23;
				day_counter=1;
			} else {
				hour_now_int-=2;
				day_counter=0;
			}
		} else if(!strcmp(period,"2d")) {
			printf("wykryto 2d\n\r");
			day_counter=2;
			skip=5;
		} else if(!strcmp(period,"3d")) {
			printf("wykryto 3d\n\r");
			day_counter=3;
			skip=10;
		} else if(!strcmp(period,"7d")) {
			printf("wykryto 7d\n\r");
			day_counter=7;
			skip=20;
		} else if(!strcmp(period,"1m")) {
			day_counter=30;
			skip=40;
		} else if(!strcmp(period,"3m")) {
			day_counter=90;
			skip=50;
		} else if(!strcmp(period,"6m")) {
			day_counter=183;
			skip=120;
		} else if(!strcmp(period,"1y")) {
			day_counter=365;
			skip=200;
		} else {
			printf("wykryto 1d\n\r");
			day_counter=1;
			skip=3;
		}


		if (f_mount(0, &filesystem) != FR_OK) {
			printf("could not open filesystem \n\r");
		}

		ret = f_stat("sensors",&fno);
		if (ret) {
			printf("Nie znaleziono folderu sensors\n\r");
		}
		ret = f_chdir("sensors");
		if(ret) {
			xSemaphoreGive(SD_gatekeeper);
			*error=-2;
			return;
		}

		for(i=1;i<=4;i++)
		{
			first_day=1;
			day_counter_temp=day_counter;

			// Wczytujemy nazwe czujnika
			//sprintf(&buff, "%d/name.txt",i);
			//if(f_open(&file, &buff, FA_READ) == FR_OK)
			//{
			//	if(f_gets(line, sizeof line, &file))
			//		strcpy(stat[i].name, line);
			//}
			//f_close(&file);

			if(!stat[i].sensor_selected) continue;

			sprintf(&PAGE, " { name: '%s',data: [",stat[i].name);
			netconn_write(conn, PAGE, strlen(PAGE), NETCONN_COPY);

			// Jestesmy w folderze
			while(day_counter_temp>=0)
			{
				RTC_GetDate_yesterday_char(&date, day_counter_temp);
				printf("wczytuje-%s\n\r",&date);
				sprintf(&buff, "%d/%s.txt", i, &date);
				day = strtok (date,"-");
				month = strtok (NULL,"-");
				month_int=atoi(month);
				year = strtok (NULL,"-");
				if(f_open(&file, &buff, FA_READ) == FR_OK)
				{


					while(f_gets(line, sizeof line, &file))
					{
						stat[i].count_in_database++;
						if(skip_count>0)
						{
							skip_count--;
							continue;
						} else skip_count=skip;

						hour = strtok (line,":");
						minute = strtok (NULL,":");
						second = strtok (NULL,";");
						temp = strtok (NULL,",");

						if(first_day)
						{
							hour_int = atoi(hour);
							minute_int = atoi(minute);
							if(hour_int < hour_now_int) continue;
							else if(hour_int == hour_now_int && minute_int < minute_now_int) continue;
						}

						//strcat((char *)PAGE, "[new Date(2012,10,3,11,30,0), 12.2]," );
						sprintf(&PAGE, "[Date.UTC(20%s,%d,%s,%s,%s,%s), %s],", year, month_int-1, day, hour, minute, second, temp);
						//first_line=0;
						netconn_write(conn, PAGE, strlen(PAGE), NETCONN_COPY);
						stat[i].count_to_show++;
					}

				} else {
					*error=-1;
					printf(" (brak pliku) ");
				}
				f_close(&file);
				day_counter_temp-=1;
				first_day=0;
			} //while
			sprintf(&PAGE, "]},");
			netconn_write(conn, PAGE, strlen(PAGE), NETCONN_COPY);
		} //for
		xSemaphoreGive(SD_gatekeeper);
	} else printf("timeout:nie mozna pobrac semafora SD (read)\n\r");
}

void SD_write_temperature(uint8_t nr, uint8_t minus, uint8_t *calkowita_temp, uint8_t *ulamkowa_temp)
{

	uint8_t buff_ch[32], date_ch[16], time_ch[16];

	RTC_GetTime_char(&time_ch); //odczytaj czas z RTC gg:mm:ss
	RTC_GetDate_char(&date_ch); // pobieramy datê dd-mm-rr
	if(minus)
		sprintf(&buff_ch, "-%d.%d", calkowita_temp, ulamkowa_temp);
	else
		sprintf(&buff_ch, "%d.%d", calkowita_temp, ulamkowa_temp);

	SD_write_data_from_sensor(&nr, &time_ch, &date_ch, &buff_ch);
}




void SD_write_data_from_sensor(uint8_t *nr, char *time_ch, char *date_ch, uint8_t *data_to_write)
{
	if(xSemaphoreTake(SD_gatekeeper,1000))
	{
		uint8_t buff_ch[32];

		if (f_mount(0, &filesystem) != FR_OK) {
			printf("could not open filesystem \n\r");
		 }

		ret = f_stat("sensors",&fno);
		if (ret) {
			ret = f_mkdir("sensors");
			if(!ret) printf("Utworzono folder sensors\n\r");
		}
		ret = f_chdir("sensors");
		if(ret) {
			xSemaphoreGive(SD_gatekeeper);
			return;
		}

		sprintf(&buff_ch, "%d",*nr);
		ret = f_stat(&buff_ch, &fno);
		if (ret) {
			ret = f_mkdir(&buff_ch);
			if(!ret) printf("Utworzono folder %d \n\r",nr);
		}
		ret = f_chdir(&buff_ch);
		if(ret) {
			xSemaphoreGive(SD_gatekeeper);
			return;
		}


		sprintf(&buff_ch, "%s.txt", date_ch);

		if( f_open(&file, &buff_ch, FA_READ | FA_WRITE | FA_OPEN_ALWAYS) == FR_OK )
		{
			f_lseek(&file, f_size(&file)); // przechodzimy na koniec
			sprintf(&buff_ch, "%s;%s\r\n", time_ch, data_to_write);
			f_write(&file, &buff_ch, strlen(&buff_ch), &bw);
			printf("zapisano ");
			f_close(&file);

		} else printf("nie mo¿na otworzyc pliku \n\r");

		xSemaphoreGive(SD_gatekeeper);
	} else printf("timeout:nie mozna pobrac semafora SD (write)\n\r");
}










void SD_test(void)
{
	SD_Error Status = SD_OK;
	FATFS filesystem;		/* volume lable */
	FRESULT ret;			  /* Result code */
	FIL file;				    /* File object */
	DIR dir;				    /* Directory object */
	FILINFO fno;			  /* File information object */
	UINT bw, br;
	uint8_t buff[128];

	/* mount the filesystem */
		  if (f_mount(0, &filesystem) != FR_OK) {
		    printf("could not open filesystem \n\r");
		  }
		  //Delay(10);
		  //ret = f_opendir(&dir, "DS");

		  printf("Open a test file (message.txt) \n\r");
		  ret = f_open(&file, "MESSAGE.TXT", FA_READ);
		  if (ret) {
		    printf("not exist the test file (message.txt)\n\r");
		  } else {
		    printf("Type the file content\n\r");
		    for (;;) {
		      ret = f_read(&file, buff, sizeof(buff), &br);	/* Read a chunk of file */
		      if (ret || !br) {
		        break;			/* Error or end of file */
		      }
		      buff[br] = 0;
		      printf("%s",buff);
		      printf("\n\r");
		    }
		    if (ret) {
		      printf("Read the file error\n\r");
		      fault_err(ret);
		    }

		    printf("Close the file\n\r");
		    ret = f_close(&file);
		    if (ret) {
		      printf("Close the file error\n\r");
		    }
		  }

		  /*  hello.txt write test*/
		  Delay(50);
		  printf("Create a new file (hello.txt)\n\r");
		  ret = f_open(&file, "HELLO.TXT", FA_WRITE | FA_CREATE_ALWAYS);
		  if (ret) {
		    printf("Create a new file error\n\r");
		    fault_err(ret);
		  } else {
		    printf("Write a text data. (hello.txt)\n\r");
		    ret = f_write(&file, "Hello world!", 14, &bw);
		    if (ret) {
		      printf("Write a text data to file error\n\r");
		    } else {
		      printf("%u bytes written\n\r", bw);
		    }
		    Delay(50);
		    printf("Close the file\n\r");
		    ret = f_close(&file);
		    if (ret) {
		      printf("Close the hello.txt file error\n\r");
		    }
		  }

		  /*  hello.txt read test*/
		  Delay(50);
		  printf("read the file (hello.txt)\n\r");
		  ret = f_open(&file, "HELLO.TXT", FA_READ);
		  if (ret) {
		    printf("open hello.txt file error\n\r");
		  } else {
		    printf("Type the file content(hello.txt)\n\r");
		    for (;;) {
		      ret = f_read(&file, buff, sizeof(buff), &br);	/* Read a chunk of file */
		      if (ret || !br) {
		        break;			/* Error or end of file */
		      }
		      buff[br] = 0;
		      printf("%s",buff);
		      printf("\n\r");
		    }
		    if (ret) {
		      printf("Read file (hello.txt) error\n\r");
		      fault_err(ret);
		    }

		    printf("Close the file (hello.txt)\n\r");
		    ret = f_close(&file);
		    if (ret) {
		      printf("Close the file (hello.txt) error\n\r");
		    }
		  }

		  /*  directory display test*/
		  Delay(50);
		  printf("Open root directory\n\r");
		  ret = f_opendir(&dir, "");
		  if (ret) {
		    printf("Open root directory error\n\r");
		  } else {
		    printf("Directory listing...\n\r");
		    for (;;) {
		      ret = f_readdir(&dir, &fno);		/* Read a directory item */
		      if (ret || !fno.fname[0]) {
		        break;	/* Error or end of dir */
		      }
		      if (fno.fattrib & AM_DIR) {
		        printf("  <dir>  %s\n\r", fno.fname);
		      } else {
		        //printf("%8lu  %s\n\r", fno.fsize, fno.fname);
		    	  printf("<file> %s\n\r", fno.fname);
		      }
		    }
		    if (ret) {
		      printf("Read a directory error\n\r");
		      fault_err(ret);
		    }
		  }
		  Delay(50);
		  printf("Test completed\n\r");

}






void SD_read_Sensors_Names(SD_Stat *stat, uint8_t *error)
{

	if(xSemaphoreTake(SD_gatekeeper,1000))
	{
		uint8_t buff[32], buff2[16], line[64], date[16],time_now[16];
		uint8_t i, first_line=1, first_day=1;
		uint8_t hour_now_int, minute_now_int, hour_int, month_int, minute_int,skip=0, skip_count=0;
		int16_t day_counter, day_counter_temp;
		char *hour, *minute, *second, *temp, *year, *month, *day, *hour_now, *minute_now;
		portCHAR PAGE[128];

		if (f_mount(0, &filesystem) != FR_OK) {
			printf("could not open filesystem \n\r");
		}

		ret = f_stat("sensors",&fno);
		if (ret) {
			printf("Nie znaleziono folderu sensors\n\r");
		}
		ret = f_chdir("sensors");
		if(ret) {
			xSemaphoreGive(SD_gatekeeper);
			*error=-2;
			return;
		}

		for(i=1;i<=4;i++)
		{
			// Wczytujemy nazwe czujnika
			sprintf(&buff, "%d/name.txt",i);
			if(f_open(&file, &buff, FA_READ) == FR_OK)
			{
				if(f_gets(line, sizeof line, &file))
					strcpy(stat[i].name, line);
			}
			f_close(&file);
		} //for
		xSemaphoreGive(SD_gatekeeper);
	} else printf("timeout:nie mozna pobrac semafora SD (read)\n\r");
}







/* --------------------------------- End Of File ------------------------------ */
