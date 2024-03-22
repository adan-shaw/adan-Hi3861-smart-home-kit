
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "net_demo.h"
#include "net_common.h"
#include "config_params.h"

extern unsigned short g_wifiStatus;
int sendMessage = 0;
static char message[256] = "";

void UdpServerTest (unsigned short port)
{
	char *megsend;
	ssize_t retval = 0;
	struct sockaddr_in clientAddr = { 0 }, serverAddr = { 0 };
	socklen_t clientAddrLen = sizeof (clientAddr);
	int ret, sockfd = socket (AF_INET, SOCK_DGRAM, 0);// UDP socket

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons (port);
	serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);

	retval = bind (sockfd, (struct sockaddr *) &serverAddr, sizeof (serverAddr));
	if (retval < 0)
	{
		printf ("bind failed, %ld!\r\n", retval);
		goto do_cleanup;
	}
	printf ("bind to port %d success!\r\n", port);

	while (1)
	{
		osDelay (10);
		retval = recvfrom (sockfd, message, sizeof (message), 0, (struct sockaddr *) &clientAddr, &clientAddrLen);
		printf ("recv message %s %ld done!\r\n", message, retval);
		printf ("peer info: ipaddr = %s, port = %d\r\n", inet_ntoa (clientAddr.sin_addr), ntohs (clientAddr.sin_port));

		if (g_wifiStatus == WIFI_AP)
		{
			ret = cJSONParseAP (message);
			if (ret < 0)
			{
				printf ("parse message failed\r\n");
			}
			break;
		}
		else
		{
			if (retval < 0)
			{
				printf ("recvfrom failed, %ld!\r\n", retval);
				goto do_cleanup;
			}
			ret = cJSONParseSTA (message);
			if (ret < 0)
			{
				printf ("parse message failed\r\n");
			}
		}

		if (sendMessage)
		{
			osDelay (100);
			megsend = cJSONReport ();
			retval = sendto (sockfd, megsend, strlen (megsend), 0, (struct sockaddr *) &clientAddr, sizeof (clientAddr));
			if (retval <= 0)
			{
				printf ("send failed, %ld!\r\n", retval);
				goto do_cleanup;
			}
			printf ("send message {%s} %ld done!\r\n", megsend, retval);
			sendMessage = 0;
		}
	}

do_cleanup:
	printf ("do_cleanup...\r\n");

	close (sockfd);
}

SERVER_TEST_DEMO (UdpServerTest);
