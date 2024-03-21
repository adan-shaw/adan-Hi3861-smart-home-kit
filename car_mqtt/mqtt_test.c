#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"

#include "hi_wifi_api.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"

#include "MQTTClient.h"

static MQTTClient mq_client;

unsigned char *onenet_mqtt_buf;
unsigned char *onenet_mqtt_readbuf;
int buf_size;

Network n;
MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

char recv_mqtt_buf[1024];

//消息回调函数
void mqtt_callback (MessageData * msg_data)
{
	size_t res_len = 0;
	uint8_t *response_buf = NULL;
	char topicname[45] = { "$crsp/" };

	LOS_ASSERT (msg_data);

	printf ("topic %.*s receive a message\r\n", msg_data->topicName->lenstring.len, msg_data->topicName->lenstring.data);

	printf ("message is %.*s\r\n", msg_data->message->payloadlen, msg_data->message->payload);

	memset (recv_mqtt_buf, 0, sizeof (recv_mqtt_buf));

	memcpy (recv_mqtt_buf, msg_data->message->payload, msg_data->message->payloadlen);

	cotrl_handle (recv_mqtt_buf, msg_data->message->payloadlen);

}

int mqtt_connect (void)
{
	int rc = 0;

	NetworkInit (&n);
	NetworkConnect (&n, "5.196.95.208", 1883);

	buf_size = 4096 + 1024;
	onenet_mqtt_buf = (unsigned char *) malloc (buf_size);
	onenet_mqtt_readbuf = (unsigned char *) malloc (buf_size);
	if (!(onenet_mqtt_buf && onenet_mqtt_readbuf))
	{
		printf ("No memory for MQTT client buffer!");
		return -2;
	}

	MQTTClientInit (&mq_client, &n, 1000, onenet_mqtt_buf, buf_size, onenet_mqtt_readbuf, buf_size);

	MQTTStartTask (&mq_client);

	data.keepAliveInterval = 30;
	data.cleansession = 1;
	data.clientID.cstring = "ohos_hi3861";
	data.username.cstring = "123456";
	data.password.cstring = "222222";
	data.cleansession = 1;

	mq_client.defaultMessageHandler = mqtt_callback;

	//连接服务器
	rc = MQTTConnect (&mq_client, &data);

	//订阅消息, 并设置回调函数
	MQTTSubscribe (&mq_client, "OpenHarmony", 0, mqtt_callback);

	while (1)
	{
		MQTTMessage message;

		message.qos = QOS1;
		message.retained = 0;
		message.payload = (void *) "openharmony";
		message.payloadlen = strlen ("openharmony");

		//发送消息
		if (MQTTPublish (&mq_client, "ohospub", &message) < 0)
		{
			printf ("MQTTPublish faild !\r\n");
		}
		usleep (1000000);
	}

	return 0;
}

void mqtt_test (void)
{
	mqtt_connect ();
}
