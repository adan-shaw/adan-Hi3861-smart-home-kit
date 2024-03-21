
#ifndef WIFI_STARTER_H
#define WIFI_STARTER_H

#include <stdint.h>
#include "wifi_hotspot.h"

int StartHotspot (const HotspotConfig * config);

void StopHotspot (void);

#endif // WIFI_STARTER_H
