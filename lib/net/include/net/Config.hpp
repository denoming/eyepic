#pragma once

#include "sdkconfig.h"

//--------------------------------------------------------------------------------------------------

// Network Config

#define EP_NET_MQTT_URL CONFIG_EP_NET_MQTT_URL
#define EP_NET_MQTT_TOPIC CONFIG_EP_NET_MQTT_TOPIC
#ifdef CONFIG_EP_NET_MQTT_AUTH
#define EP_NET_MQTT_AUTH CONFIG_EP_NET_MQTT_AUTH
#define EP_NET_MQTT_USER CONFIG_EP_NET_MQTT_USER
#define EP_NET_MQTT_PASS CONFIG_EP_NET_MQTT_PASS
#endif

// WiFi Config

#define EP_NET_WIFI_SSID CONFIG_EP_NET_WIFI_SSID
#define EP_NET_WIFI_PASSWORD CONFIG_EP_NET_WIFI_PASSWORD
#define EP_NET_MAXIMUM_RETRY CONFIG_EP_NET_MAXIMUM_RETRY

//--------------------------------------------------------------------------------------------------
