#include <time.h>
#include <string.h>

#include "lwip/api.h"


#if LWIP_NETCONN /* don't build if not configured for use in lwipopts.h */

/** This is an example of a "SNTP" client (with netconn API).
 *
 * For a list of some public NTP servers, see this link :
 * http://support.ntp.org/bin/view/Servers/NTPPoolServers
 *
 */


#include "sntp_netconn.h"

/**
 * SNTP request
 */
time_t sntp_request(void)
{
	unsigned char * sntp_request;
	unsigned char * sntp_response;
	struct ip_addr sntp_server_address;
	time_t timestamp = 0;

	struct netconn * sendUDPNetConn;
	struct netbuf * sendUDPNetBuf;
	struct netbuf * receiveUDPNetBuf;

	u16_t dataLen;

	err_t errLWIP;

	/* initialize SNTP server address */
	sntp_server_address.addr = SNTP_SERVER_ADDRESS;

	/* if we got a valid SNTP server address... */
	if (sntp_server_address.addr != 0)
	{
		/* create new socket */
		sendUDPNetConn = netconn_new( NETCONN_UDP );
		sendUDPNetBuf = netbuf_new();
		// Create data space for netbuf, if we can.
		sntp_request = (unsigned char *) netbuf_alloc(sendUDPNetBuf, SNTP_MAX_DATA_LEN);

		if ((NULL != sendUDPNetConn) && (NULL != sendUDPNetBuf) && (NULL != sntp_request))
		{
			errLWIP = netconn_connect(sendUDPNetConn, &sntp_server_address, SNTP_PORT);
			if(ERR_OK == errLWIP)
			{
				/* prepare SNTP request */
				memset(sntp_request, 0, SNTP_MAX_DATA_LEN);
				sntp_request[0] = SNTP_LI_NO_WARNING | SNTP_VERSION | SNTP_MODE_CLIENT;

				errLWIP = netconn_send(sendUDPNetConn, sendUDPNetBuf);
				// Send SNTP request to server.
				if (ERR_OK == errLWIP)
				{
					// Set recv timeout.
					sendUDPNetConn->recv_timeout = SNTP_RECV_TIMEOUT;
					// Receive SNTP server response.
					receiveUDPNetBuf = netconn_recv(sendUDPNetConn);

					if (NULL != receiveUDPNetBuf)
					{
						// Get pointer to response data.
						netbuf_data(receiveUDPNetBuf, (void **) &sntp_response,(u16_t *) &dataLen);

						// If the response size is good.
						if (dataLen == SNTP_MAX_DATA_LEN)
						{
							// If this is a SNTP response...
							if (((sntp_response[0] & SNTP_MODE_MASK) == SNTP_MODE_SERVER) || ((sntp_response[0]
									& SNTP_MODE_MASK) == SNTP_MODE_BROADCAST))
							{
								/* extract GMT time from response */
								memcpy(&timestamp, (sntp_response + SNTP_RCV_TIME_OFS), sizeof(timestamp));
								timestamp = (ntohl(timestamp) - DIFF_SEC_1900_1970);

								syslog(LOG_DEBUG | LOG_USER, "Received timestamp %u", timestamp);
							}
							else
							{
								syslog(LOG_INFO | LOG_USER, "Received data did not match frame code");
							}
						}
						else
						{
							syslog(LOG_NOTICE | LOG_USER, "Length of data did not match SNTP_MAX_DATA_LEN, received len %u", dataLen);
						}
					}
					else
					{
						syslog(LOG_WARNING | LOG_USER, "Netconn receive failed with %d", netconn_err(sendUDPNetConn));
					}
				} // netconn_send(sendUDPNetConn, sendUDPNetBuf);
				else
				{
					syslog(LOG_WARNING | LOG_USER, "Netconn sendto failed with %d", errLWIP);
				}
			} //netconn_connect(sendUDPNetConn, &sntp_server_address, SNTP_PORT);
			else
			{
				syslog(LOG_WARNING | LOG_USER, "Netconn connect to server %X, port %u failed with %d", sntp_server_address.addr, SNTP_PORT, errLWIP);
			}
		} // if ((NULL != sendUDPNetConn) && (NULL != sendUDPNetBuf) && (NULL != sntp_request))
		else
		{
			syslog(LOG_ERR | LOG_USER, "Netconn or netbuf or data allocation failed.");
		}
	} //if (sntp_server_address != 0)
	else
	{
		syslog(LOG_WARNING | LOG_USER, "Invalid NTP server address %X", SNTP_SERVER_ADDRESS);
	}

	netbuf_delete(sendUDPNetBuf);
	netbuf_delete(receiveUDPNetBuf);
	netconn_delete(sendUDPNetConn);

	return timestamp;
}

#endif /* LWIP_SOCKET */
