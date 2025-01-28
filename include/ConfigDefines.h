#ifndef CONFIG_DEFINES_H
#define CONFIG_DEFINES_H

#define DEVICE_NAME ""

#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define WIFI_AUTO_RECONNECT true
#define WIFI_CHECK_INTERVAL 500
#define WIFI_RECONNECT_INTERVAL 5000
#define WIFI_MAX_CONNECTION_ATTEMPTS 20

#define MQTT_BROKER ""
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_RETRY_INTERVAL 5000
#define MQTT_MAX_CONNECTION_ATTEMPTS 20
#define MQTT_BASE_TOPIC "gpsno/devices/"

#define ERROR_MAX_RECOVERY_ATTEMPTS 3
#define ERROR_RECOVERY_INTERVAL 5000

#define LOGGING_LEVEL 0 // 0: DEBUG, 1: INFO, 2: WARNING, 3: ERROR
#define LOGGING_ALLOW_MQTT_LOG true
#define LOGGING_MQTT_TOPIC ""

#define DEBUG_FORCE_CONFIG true
#endif