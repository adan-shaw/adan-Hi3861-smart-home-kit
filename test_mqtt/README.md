**摘要**: 本文简单介绍如何移植MQTT

**适合群体**: 适用于润和Hi3861开发板
文中所有代码仓库: https://gitee.com/qidiyun/hihope-3861-smart-home-kit 

## 11.1 MQTT介绍
MQTT 是当前最主流的物联网通信协议, 需要物联网云平台, 例如华为云、阿里云、移动OneNET都支持mqtt;而Hi3861则是一款专为IoT应用场景打造的芯片;本节主要讲如何在鸿蒙系统中通过移植第3方软件包 paho mqtt去实现MQTT协议功能, 最后会给出测试验证;为后续的物联网项目打好基础;

友情预告, 本节内容较多, 源码也贴出来了, 大家最好先看一遍, 然后再操作一次;

已经移植好的MQTT源码: 
https://gitee.com/qidiyun/harmony_mqtt  


## 11.2 MQTT移植
如果不想要自己移植的, 可以跳过本节
MQTT 全称为 Message Queuing Telemetry Transport(消息队列遥测传输)是一种基于发布/订阅范式的二进制"轻量级"消息协议, 由IB公司发布;针对于网络受限和嵌入式设备而设计的一种数据传输协议;MQTT最大优点在于, 可以以极少的代码和有限的带宽, 为连接远程设备提供实时可靠的消息服务;作为一种低开销、低带宽占用的即时通讯协议, 使其在物联网、小型设备、移动应用等方面有较广泛的应用;MQTT模型如图所示;

更多MQTT协议的介绍见这篇文章: MQTT 协议开发入门
![image.png](https://img-blog.csdnimg.cn/img_convert/0ba3e6856d7fb18ca91ae546515431d8.png)
 

### 1. 下载paho mqtt软件包, 添加到鸿蒙代码中

paho mqtt-c 是基于C语言实现的MQTT客户端, 非常适合用在嵌入式设备上;首先下载源码: 

https://github.com/eclipse/paho.mqtt.embedded-c

下载之后解压, 会得到这么一个文件夹: 

 ![image.png](https://img-blog.csdnimg.cn/img_convert/fc6997525d8dc3b7952c60d876fb968f.png)如何在鸿蒙系统中移植 Paho-MQTT 实现MQTT协议-鸿蒙HarmonyOS技术社区

我们在鸿蒙系统源码的 third_party 文件夹下创建一个 pahomqtt 文件夹, 然后把解压后的所有文件都拷贝到 pahomqtt 文件夹下

下一步, 我们在pahomqtt 文件夹下面新建BUILD.gn文件, 用来构建编译;其内容如下: 

```
import("//build/lite/config/component/lite_component.gni")
import("//build/lite/ndk/ndk.gni")

config("pahomqtt_config") {
	include_dirs = [
		"MQTTPacket/src",
		"MQTTClient-C/src",
		"MQTTClient-C/src/liteOS",
		"//kernel/liteos_m/components/cmsis/2.0",
	]
}

pahomqtt_sources = [
"MQTTClient-C/src/liteOS/MQTTLiteOS.c",
"MQTTClient-C/src/MQTTClient.c",

"MQTTPacket/src/MQTTConnectClient.c",
"MQTTPacket/src/MQTTConnectServer.c",
"MQTTPacket/src/MQTTDeserializePublish.c",
"MQTTPacket/src/MQTTFormat.c",
"MQTTPacket/src/MQTTPacket.c",
"MQTTPacket/src/MQTTSerializePublish.c",
"MQTTPacket/src/MQTTSubscribeClient.c",
"MQTTPacket/src/MQTTSubscribeServer.c",
"MQTTPacket/src/MQTTUnsubscribeClient.c",
"MQTTPacket/src/MQTTUnsubscribeServer.c",
]

lite_library("pahomqtt_static") {
	target_type = "static_library"
	sources = pahomqtt_sources
	public_configs = [ ":pahomqtt_config" ]
}

lite_library("pahomqtt_shared") {
	target_type = "shared_library"
	sources = pahomqtt_sources
	public_configs = [ ":pahomqtt_config" ]
}

ndk_lib("pahomqtt_ndk") {
	if (board_name != "hi3861v100") {
		lib_extension = ".so"
		deps = [
			":pahomqtt_shared"
		]
	} else {
		deps = [
			":pahomqtt_static"
		]
	}
	head_files = [
		"//third_party/pahomqtt"
	]
}
```

 

### 2. 移植
我们使用到的是MQTTClient-C的代码, 该代码支持多线程;

**(1)创建LiteOS文件夹**
MQTT已经提供了Linux和freertos的移植, 这里我们参考, 新建文件夹: 
third_party\pahomqtt\MQTTClient-C\src\liteOS
里面存放两个文件: 
MQTTLiteOS.c 和 MQTTLiteOS.h

内容如下: 
```
#include "MQTTLiteOS.h"

//用来创建线程
int ThreadStart(Thread* thread, void (*fn)(void*), void* arg)
{
	int rc = 0;
	thread = thread;

	osThreadAttr_t attr;

	attr.name = "MQTTTask";
	attr.attr_bits = 0U;
	attr.cb_mem = NULL;
	attr.cb_size = 0U;
	attr.stack_mem = NULL;
	attr.stack_size = 2048;
	attr.priority = osThreadGetPriority(osThreadGetId());

	rc = (int)osThreadNew((osThreadFunc_t)fn, arg, &attr);

	return rc;
}
//定时器初始化
void TimerInit(Timer* timer)
{
	timer->end_time = (struct timeval){0, 0};
}

char TimerIsExpired(Timer* timer)
{
	struct timeval now, res;
	gettimeofday(&now, NULL);
	timersub(&timer->end_time, &now, &res);
	return res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_usec <= 0);
}


void TimerCountdownMS(Timer* timer, unsigned int timeout)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	struct timeval interval = {timeout / 1000, (timeout % 1000) * 1000};
	timeradd(&now, &interval, &timer->end_time);
}


void TimerCountdown(Timer* timer, unsigned int timeout)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	struct timeval interval = {timeout, 0};
	timeradd(&now, &interval, &timer->end_time);
}


int TimerLeftMS(Timer* timer)
{
	struct timeval now, res;
	gettimeofday(&now, NULL);
	timersub(&timer->end_time, &now, &res);
	//printf("left %d ms\n", (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000);
	return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;
}




void MutexInit(Mutex* mutex)
{
	mutex->sem = osSemaphoreNew(1, 1, NULL);
}

int MutexLock(Mutex* mutex)
{
	return osSemaphoreAcquire(mutex->sem, LOS_WAIT_FOREVER);
}

int MutexUnlock(Mutex* mutex)
{
	return osSemaphoreRelease(mutex->sem);
}

//接受数据
int ohos_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	struct timeval interval = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
	if (interval.tv_sec < 0 || (interval.tv_sec == 0 && interval.tv_usec <= 0))
	{
		interval.tv_sec = 0;
		interval.tv_usec = 100;
	}

	setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&interval, sizeof(struct timeval));

	int bytes = 0;
	while (bytes < len)
	{
		int rc = recv(n->my_socket, &buffer[bytes], (size_t)(len - bytes), 0);
		if (rc == -1)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
		 bytes = -1;
			break;
		}
		else if (rc == 0)
		{
			bytes = 0;
			break;
		}
		else
			bytes += rc;
	}
	return bytes;
}

//写数据
int ohos_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	struct timeval tv;

	tv.tv_sec = 0;  /* 30 Secs Timeout */
	tv.tv_usec = timeout_ms * 1000;  // Not init'ing this can cause strange errors

	setsockopt(n->my_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));
	int	rc = send(n->my_socket, buffer, len, 0);
	return rc;
}

//网络初始化
void NetworkInit(Network* n)
{
	n->my_socket = 0;
	n->mqttread = ohos_read;
	n->mqttwrite = ohos_write;
}

//网络连接
int NetworkConnect(Network* n, char* addr, int port)
{
	int type = SOCK_STREAM;
	struct sockaddr_in address;
	int rc = -1;
	sa_family_t family = AF_INET;
	struct addrinfo *result = NULL;
	struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

	if ((rc = getaddrinfo(addr, NULL, &hints, &result)) == 0)
	{
		struct addrinfo* res = result;

		/* prefer ip4 addresses */
		while (res)
		{
			if (res->ai_family == AF_INET)
			{
				result = res;
				break;
			}
			res = res->ai_next;
		}

		if (result->ai_family == AF_INET)
		{
			address.sin_port = htons(port);
			address.sin_family = family = AF_INET;
			address.sin_addr = ((struct sockaddr_in*)(result->ai_addr))->sin_addr;
		}
		else
			rc = -1;

		freeaddrinfo(result);
	}

	if (rc == 0)
	{
		n->my_socket = socket(family, type, 0);
		if (n->my_socket != -1)
			rc = connect(n->my_socket, (struct sockaddr*)&address, sizeof(address));
		else
			rc = -1;
	}

	return rc;
}


void NetworkDisconnect(Network* n)
{
	close(n->my_socket);
}

```

至此我们移植基本结束


 

## 11.3 测试代码
测试代码比较好写;主要是3个文件, 内容我都贴出来了: 


**(1)BUILD.gn文件内容: **
```
static_library("mqtt_test") {
	sources = [
		"mqtt_test.c",
		"mqtt_entry.c"
	]

	include_dirs = [
		"//utils/native/lite/include",
		"//kernel/liteos_m/components/cmsis/2.0",
		"//base/iot_hardware/interfaces/kits/wifiiot_lite",
		"//vendor/hisi/hi3861/hi3861/third_party/lwip_sack/include",
		"//foundation/communication/interfaces/kits/wifi_lite/wifiservice",
		"//third_party/pahomqtt/MQTTPacket/src",
		"//third_party/pahomqtt/MQTTClient-C/src",
		"//third_party/pahomqtt/MQTTClient-C/src/liteOS",
	]
#表示需要a_myparty 软件包
	deps = [
		"//third_party/pahomqtt:pahomqtt_static",
	]
}

```


**(2)mqtt_entry.c文件**
主要是进行热点连接, 因为我们要使用MQTT需要用到网络;热点连接的代码之前在第9章已经讲说, 这里就不完全贴了, 代码仓库也有, 主要的代码部分: 
```
void wifi_sta_task(void *arg)
{
	arg = arg;
	
	//连接热点
	hi_wifi_start_sta();

	while(wifi_ok_flg == 0)
	{
		usleep(30000);
	}
	
	
	usleep(2000000);

	//开始进入MQTT测试
	mqtt_test();
}

```



**(3)mqtt_test.c 文件则是编写了一个简单的MQTT测试代码**

其中测试用的mqtt服务器是我自己的服务器: 5.196.95.208
大家也可以改成自己的;
```
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

//消息回调函数
void mqtt_callback(MessageData *msg_data)
{
	size_t res_len = 0;
	uint8_t *response_buf = NULL;
	char topicname[45] = { "$crsp/" };

	LOS_ASSERT(msg_data);

	printf("topic %.*s receive a message\r\n", msg_data->topicName->lenstring.len, msg_data->topicName->lenstring.data);

	printf("message is %.*s\r\n", msg_data->message->payloadlen, msg_data->message->payload);

}

int mqtt_connect(void)
{
	int rc = 0;
	
	NetworkInit(&n);
	NetworkConnect(&n, "5.196.95.208", 1883);

	buf_size  = 2048;
	onenet_mqtt_buf = (unsigned char *) malloc(buf_size);
	onenet_mqtt_readbuf = (unsigned char *) malloc(buf_size);
	if (!(onenet_mqtt_buf && onenet_mqtt_readbuf))
	{
		printf("No memory for MQTT client buffer!");
		return -2;
	}

	MQTTClientInit(&mq_client, &n, 1000, onenet_mqtt_buf, buf_size, onenet_mqtt_readbuf, buf_size);
	
	MQTTStartTask(&mq_client);


	data.keepAliveInterval = 30;
	data.cleansession = 1;
	data.clientID.cstring = "ohos_hi3861";
	data.username.cstring = "123456";
	data.password.cstring = "222222";

	data.keepAliveInterval = 10;
	data.cleansession = 1;
	
	mq_client.defaultMessageHandler = mqtt_callback;

	//连接服务器
	rc = MQTTConnect(&mq_client, &data);

	//订阅消息, 并设置回调函数
	MQTTSubscribe(&mq_client, "ohossub", 0, mqtt_callback);

	while(1)
	{
		MQTTMessage message;

		message.qos = QOS1;
		message.retained = 0;
		message.payload = (void *)"openharmony";
		message.payloadlen = strlen("openharmony");

		//发送消息
		if (MQTTPublish(&mq_client, "ohospub", &message) < 0)
		{
			return -1;
		}
	}

	return 0;
}


void mqtt_test(void)
{
	mqtt_connect();
}
```


 

到这里就完成了代码部分, 可以开始编译了;

 

## 11.4 实验
这里我们需要先下载一个 Windows电脑端的 MQTT客户端, 这样我们就可以用电脑订阅开发板的MQTT主题信息了;

电脑版的mqtt客户端下载链接: https://repo.eclipse.org/content/repositories/paho-releases/org/eclipse/paho/org.eclipse.paho.ui.app/1.1.1/ 

我们选择这一个: 
![image.png](https://img-blog.csdnimg.cn/img_convert/129e4b47db6ed539747e3c3466654695.png)
弄完后打开软件, 按图操作: 
![image.png](https://img-blog.csdnimg.cn/img_convert/870479f955abe6b2457bd0eab7782e9d.png)

此时我们去查看 我们电脑端的MQTT客户端软件, 可以看到右边已经有接收MQTT信息了, 主题未 ohospub, 消息内容为 openharmony, 说明实验成功;

电脑发送主题为ohossub, 内容为123456, 查看串口打印, 可以看到也收到了数据
![image.png](https://img-blog.csdnimg.cn/img_convert/b26f2932d131cafdf8f24bab38e2be8c.png)
