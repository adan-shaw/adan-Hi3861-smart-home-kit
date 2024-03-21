
#include "wifi_starter.h"
#include "cmsis_os2.h"

#include "lwip/netifapi.h"

static volatile int g_hotspotStarted = 0;

static void OnHotspotStateChanged (int state)
{
	printf ("OnHotspotStateChanged: %d.\r\n", state);
	if (state == WIFI_HOTSPOT_ACTIVE)
	{
		g_hotspotStarted = 1;
	}
	else
	{
		g_hotspotStarted = 0;
	}
}

static volatile int g_joinedStations = 0;

static void PrintStationInfo (StationInfo * info)
{
	if (!info)
		return;
	static char macAddress[32] = { 0 };
	unsigned char *mac = info->macAddress;
	snprintf (macAddress, sizeof (macAddress), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	printf (" PrintStationInfo: mac=%s, reason=%d.\r\n", macAddress, info->disconnectedReason);
}

static void OnHotspotStaJoin (StationInfo * info)
{
	g_joinedStations++;
	PrintStationInfo (info);
	printf ("+OnHotspotStaJoin: active stations = %d.\r\n", g_joinedStations);
}

static void OnHotspotStaLeave (StationInfo * info)
{
	g_joinedStations--;
	PrintStationInfo (info);
	printf ("-OnHotspotStaLeave: active stations = %d.\r\n", g_joinedStations);
}

WifiEvent g_defaultWifiEventListener = {
	.OnHotspotStaJoin = OnHotspotStaJoin,
	.OnHotspotStaLeave = OnHotspotStaLeave,
	.OnHotspotStateChanged = OnHotspotStateChanged,
};

static struct netif *g_iface = NULL;

int StartHotspot (const HotspotConfig * config)
{
	WifiErrorCode errCode = WIFI_SUCCESS;

	errCode = RegisterWifiEvent (&g_defaultWifiEventListener);
	printf ("RegisterWifiEvent: %d\r\n", errCode);

	errCode = SetHotspotConfig (config);
	printf ("SetHotspotConfig: %d\r\n", errCode);

	g_hotspotStarted = 0;
	errCode = EnableHotspot ();
	printf ("EnableHotspot: %d\r\n", errCode);

	while (!g_hotspotStarted)
	{
		osDelay (10);
	}
	printf ("g_hotspotStarted = %d.\r\n", g_hotspotStarted);

	g_iface = netifapi_netif_find ("ap0");
	if (g_iface)
	{
		ip4_addr_t ipaddr;
		ip4_addr_t gateway;
		ip4_addr_t netmask;

		IP4_ADDR (&ipaddr, 192, 168, 1, 1);	/* input your IP for example: 192.168.1.1 */
		IP4_ADDR (&gateway, 192, 168, 1, 1);	/* input your gateway for example: 192.168.1.1 */
		IP4_ADDR (&netmask, 255, 255, 255, 0);	/* input your netmask for example: 255.255.255.0 */
		err_t ret = netifapi_netif_set_addr (g_iface, &ipaddr, &netmask, &gateway);
		printf ("netifapi_netif_set_addr: %d\r\n", ret);

		ret = netifapi_dhcps_start (g_iface, 0, 0);
		printf ("netifapi_dhcp_start: %d\r\n", ret);
	}
	return errCode;
}

void StopHotspot (void)
{
	if (g_iface)
	{
		err_t ret = netifapi_dhcps_stop (g_iface);
		printf ("netifapi_dhcps_stop: %d\r\n", ret);
	}

	WifiErrorCode errCode = UnRegisterWifiEvent (&g_defaultWifiEventListener);
	printf ("UnRegisterWifiEvent: %d\r\n", errCode);

	errCode = DisableHotspot ();
	printf ("EnableHotspot: %d\r\n", errCode);
}
