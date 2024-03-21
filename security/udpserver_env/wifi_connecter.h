
#ifndef WIFI_CONNECTER_H
#define WIFI_CONNECTER_H

#include "wifi_device.h"

int ConnectToHotspot (WifiDeviceConfig * apConfig);

void DisconnectWithHotspot (int netId);

#endif // WIFI_CONNECTER_H
