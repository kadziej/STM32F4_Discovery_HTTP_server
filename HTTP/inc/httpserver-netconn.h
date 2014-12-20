#ifndef __HTTPSERVER_NETCONN_H__
#define __HTTPSERVER_NETCONN_H__

#include "sd_card.h"

void http_server_netconn_init(void);
void DynWebPage_tasks(struct netconn *conn);

void DynWebPage_main_bar(uint8_t nr_pos, char *PAGE_BODY);
void DynWebPage_footer(char *PAGE_BODY);
void DynWebPage_sensors_bar(SD_Stat *sensors_info,char *per, char *PAGE_BODY,char *PAGE_BODY_BUFF);
void DynWebPage_period_bar(struct netconn *conn, SD_Stat *sensors_info,char *per, char *PAGE_BODY,char *PAGE_BODY_BUFF);

void DynWebPage_IO(struct netconn *conn, char *get);
void DynWebPage_Temperature(struct netconn *conn, char *get);
void DynWebPage_Main(struct netconn *conn);

#endif /* __HTTPSERVER_NETCONN_H__ */
