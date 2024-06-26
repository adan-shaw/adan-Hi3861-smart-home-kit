
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "net_demo.h"
#include "net_params.h"
#include "wifi_connecter.h"
#include "wifi_starter.h"
#include "config_params.h"

static int g_netId = -1;

char ssidValue[WIFI_MAX_SSID_LEN] = { 0 };
char pskKeyValue[WIFI_MAX_KEY_LEN] = { 0 };

unsigned short g_wifiStatus = WIFI_AP;

int ConnectToWifi (const char *ssid, const char *key, const int type)
{
	WifiDeviceConfig config = { 0 };
	strcpy (config.ssid, ssid);
	strcpy (config.preSharedKey, key);
	config.securityType = type;
	g_netId = ConnectToHotspot (&config);
	g_wifiStatus = WIFI_STA;
	printf ("netId = %d\r\n", g_netId);
	return g_netId;
}

int StartConfig (void)
{
	HotspotConfig config = { 0 };
	strcpy (config.ssid, "HarmonyOS-AP");
	config.securityType = WIFI_SEC_TYPE_OPEN;
	g_netId = StartHotspot (&config);
	printf ("netId = %d\r\n", g_netId);
	g_wifiStatus = WIFI_AP;
	return g_netId;
}

bool CheckKvStoreResult (void)
{
	int ret = UtilsGetValue (PARAM_HOTSPOT_SSID_KEY, ssidValue, WIFI_MAX_SSID_LEN);
	if (ret < 0)
	{
		printf ("get ssid value failed, ret is %d\r\n", ret);
		return false;
	}
	ret = UtilsGetValue (PARAM_HOTSPOT_PSK_KEY, pskKeyValue, WIFI_MAX_KEY_LEN);
	if (ret < 0)
	{
		printf ("get key value failed, ret is %d\r\n", ret);
		return false;
	}
	return true;
}

static void NetDemoTask (void *arg)
{
	(void) arg;

	int result = CheckKvStoreResult ();
	printf ("result is %d\r\n", result);
	if (result != true)
	{
		printf ("start hostpot\n");
		int g_netId = StartConfig ();
		if (g_netId < 0)
		{
			printf ("start hostpot failed!\r\n");
		}
	}
	else
	{
		printf ("start connect to hostpot\r\n");
		int g_netId = ConnectToWifi (ssidValue, pskKeyValue, WIFI_SEC_TYPE_PSK);
		if (g_netId < 0)
		{
			printf ("connect to hostpot failed\r\n");
		}
	}

	UdpServerStart ();
	NetDemoTest (PARAM_SERVER_PORT, PARAM_SERVER_ADDR);
}

static void NetDemoEntry (void)
{
	osThreadAttr_t attr;

	attr.name = "NetDemoTask";
	attr.attr_bits = 0U;
	attr.cb_mem = NULL;
	attr.cb_size = 0U;
	attr.stack_mem = NULL;
	attr.stack_size = 10240;
	attr.priority = osPriorityNormal;

	if (osThreadNew (NetDemoTask, NULL, &attr) == NULL)
	{
		printf ("[NetDemoEntry] Falied to create NetDemoTask!\n");
	}
}

APP_FEATURE_INIT (NetDemoEntry);
