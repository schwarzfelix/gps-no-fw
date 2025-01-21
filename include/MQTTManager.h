#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>
#include <WiFiClient.h>
#include "ConfigManager.h"

class MQTTManager {
private:
    MQTTManager() : client(espClient), initialized(false) {}

    WiFiClient espClient;
    PubSubClient client;
    bool initialized;
    String clientId;
};

#endif
