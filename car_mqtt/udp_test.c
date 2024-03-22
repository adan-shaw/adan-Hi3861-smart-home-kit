
#include <stdio.h>
#include <unistd.h>

#include "hi_wifi_api.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "cJSON.h"

#include "car_test.h"

char recvline[1024];

void cotrl_handle (char recvline, int ret)
{
	cJSON *recvjson = cJSON_Parse (recvline);

	if (recvjson != NULL)
	{

		if (cJSON_GetObjectItem (recvjson, "cmd")->valuestring != NULL)
		{
			printf ("cmd : %s\r\n", cJSON_GetObjectItem (recvjson, "cmd")->valuestring);

			if (strcmp ("forward", cJSON_GetObjectItem (recvjson, "cmd")->valuestring) == 0)
			{
				set_car_status (CAR_STATUS_FORWARD);
				printf ("forward\r\n");
			}

			if (strcmp ("backward", cJSON_GetObjectItem (recvjson, "cmd")->valuestring) == 0)
			{
				set_car_status (CAR_STATUS_BACKWARD);
				printf ("backward\r\n");
			}

			if (strcmp ("left", cJSON_GetObjectItem (recvjson, "cmd")->valuestring) == 0)
			{
				set_car_status (CAR_STATUS_LEFT);
				printf ("left\r\n");
			}

			if (strcmp ("right", cJSON_GetObjectItem (recvjson, "cmd")->valuestring) == 0)
			{
				set_car_status (CAR_STATUS_RIGHT);
				printf ("right\r\n");
			}

			if (strcmp ("stop", cJSON_GetObjectItem (recvjson, "cmd")->valuestring) == 0)
			{
				set_car_status (CAR_STATUS_STOP);
				printf ("stop\r\n");
			}
		}

		if (cJSON_GetObjectItem (recvjson, "mode")->valuestring != NULL)
		{
			if (strcmp ("step", cJSON_GetObjectItem (recvjson, "mode")->valuestring) == 0)
			{
				set_car_mode (CAR_MODE_STEP);
				printf ("mode step\r\n");
			}

			if (strcmp ("alway", cJSON_GetObjectItem (recvjson, "mode")->valuestring) == 0)
			{
				set_car_mode (CAR_MODE_ALWAY);
				printf ("mode alway\r\n");
			}
		}

		cJSON_Delete (recvjson);
	}
}

void udp_thread (void *pdata)
{
	int ret, sizeClientAddr = sizeof (struct sockaddr_in);
	struct sockaddr_in servaddr, addrClient;

	pdata = pdata;

	int sockfd = socket (PF_INET, SOCK_DGRAM, 0);

	//填写服务器ip+port
	//bzero (&servaddr, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	servaddr.sin_port = htons (50001);

	printf ("udp_thread \r\n");
	bind (sockfd, (struct sockaddr *) &servaddr, sizeof (servaddr));

	while (1)
	{
		memset (recvline, sizeof (recvline), 0);
		ret = recvfrom (sockfd, recvline, 1024, 0, (struct sockaddr *) &addrClient, (socklen_t *) & sizeClientAddr);

		if (ret > 0)
		{
			char *pClientIP = inet_ntoa (addrClient.sin_addr);

			printf ("%s-%d(%d) says:%s\n", pClientIP, ntohs (addrClient.sin_port), addrClient.sin_port, recvline);

			cotrl_handle (recvline, ret);
		}
	}
}

void start_udp_thread (void)
{
	osThreadAttr_t attr;

	attr.name = "wifi_config_thread";
	attr.attr_bits = 0U;
	attr.cb_mem = NULL;
	attr.cb_size = 0U;
	attr.stack_mem = NULL;
	attr.stack_size = 2048;
	attr.priority = 36;

	if (osThreadNew ((osThreadFunc_t) udp_thread, NULL, &attr) == NULL)
	{
		printf ("[LedExample] Falied to create LedTask!\n");
	}
}
