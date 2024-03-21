
#ifndef CONFIG_PARAMS_H
#define CONFIG_PARAMS_H

// kv_store key
#define PARAM_HOTSPOT_SSID_KEY "hostpot_ssid"
#define PARAM_HOTSPOT_PSK_KEY "hostpot_psk"
#define PARAM_LIGHTNESS "lightness"
#define PARAM_LIGHT_TIME "time"
#define PARAM_TEMP_MAX "temp_max"
#define PARAM_TEMP_MIN "temp_min"
#define PARAM_HUMI_MAX "humi_max"
#define PARAM_HUMI_MIN "humi_min"
#define PARAM_GAS_VALUE "gas_value"

typedef enum
{
	WIFI_AP = 0,
	WIFI_STA = 1,
} WifiStatus;

#endif // CONFIG_PARAMS_H
