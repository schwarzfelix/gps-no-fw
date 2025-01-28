#include "MQTTManager.h"

void MQTTManager::handleCallback(char* topic , uint8_t* payload, uint32_t length) {
    char msgBuffer[1024];
    snprintf(msgBuffer, sizeof(msgBuffer), "Received message ('%s', '%s')", topic, payload);
    log.debug("MQTTManager", msgBuffer);

    for(const auto& subscription : subscriptions) {
        if(matchTopic(subscription.topic.c_str(), topic)) {
            subscription.callback(topic, payload, length);
        }
    }
}

bool MQTTManager::matchTopic(const char* pattern, const char* topic) {
    const char* patternPtr = pattern;
    const char* topicPtr = topic;

    while(*patternPtr && *topicPtr) {
        if(*patternPtr == '+'){
            while (*topicPtr && *topicPtr != '/') {
                topicPtr++;
            }
            patternPtr++;
        } else if (*patternPtr == '#'){
            return true;
        } else if (*patternPtr != *topicPtr) {
            return false;
        } else {
            patternPtr++;
            topicPtr++;
        }
    }

    return *patternPtr == *topicPtr;
}

bool MQTTManager::begin(){
    if(initialized) {
        return true;
    }

    RuntimeConfig& config = configManager.getRuntimeConfig();

    if(strlen(config.mqtt.broker) == 0) {
        log.warning("MQTTManager", "No MQTT broker available, skipping MQTTManager initialization");
        return false;
    } else if (config.mqtt.port == 0) {
        log.warning("MQTTManager", "No MQTT port available, skipping MQTTManager initialization");
        return false;
    }

    clientId = String(config.device.name) + "-" + String((uint32_t)config.device.chipID, HEX);

    client.setServer(config.mqtt.broker, config.mqtt.port);
    initialized = true;

    return true;
}

bool MQTTManager::connect(){
    if(!initialized) {
        log.error("MQTTManager", "MQTTManager not initialized");
        return false;
    }

    RuntimeConfig& config = configManager.getRuntimeConfig();

    if(client.connected()) {
        return true;
    }

    log.debug("MQTTManager", "Attempting to connect to MQTT broker...");
    bool connectionResult;
    if(strlen(config.mqtt.user) > 0) {
        connectionResult = client.connect(clientId.c_str(), config.mqtt.user, config.mqtt.password);
    } else {
        connectionResult = client.connect(clientId.c_str());
    }

    if (connectionResult) {
        log.info("MQTTManager", "Connected to MQTT broker");

        for(const auto& subscription : subscriptions) {
            client.subscribe(subscription.topic.c_str());
        }

        return true;
    } else {
        char msgBuffer[64];
        snprintf(msgBuffer, sizeof(msgBuffer), "Connection failed, rc=%d", client.state());
        log.error("MQTTManager", msgBuffer);
        return false;
    }
}

void MQTTManager::disconnect(){
    if(client.connected()) {
        client.disconnect();
        log.info("MQTTManager", "Disconnected from MQTT broker");
    }
}

bool MQTTManager::subscribe(const char* topic, MQTTCallback callback) {
    if(!client.connected()) {
        log.error("MQTTManager", "MQTT client not connected");
        return false;
    }

    for(const auto& subscription : subscriptions) {
        if(subscription.topic == topic) {
            log.warning("MQTTManager", "Already subscribed to topic");
            return false;
        }
    }

    if(client.subscribe(topic)){
        subscriptions.push_back({String(topic), callback});
        
        char msgBuffer[128];
        snprintf(msgBuffer, sizeof(msgBuffer), "Subscribed to topic: %s", topic);
        log.info("MQTTManager", msgBuffer);

        return true;
    }

    char msgBuffer[128];
    snprintf(msgBuffer, sizeof(msgBuffer), "Failed to subscribe to topic: %s", topic);
    log.error("MQTTManager", msgBuffer);

    return false;
}

bool MQTTManager::unsubscribe(const char* topic){
    if(!client.connected()) {
        log.error("MQTTManager", "MQTT client not connected");
        return false;
    }

    if(client.unsubscribe(topic)){
        subscriptions.erase(
            std::remove_if(
                subscriptions.begin(), 
                subscriptions.end(),
                [topic](const Subscription& subscription){
                    return subscription.topic == topic;
                }
            ),
            subscriptions.end()
        );
        return true;
    }

    return false;
}

bool MQTTManager::publish(const char* topic, const char* payload, bool retained) {
    if(!client.connected()) {
        log.error("MQTTManager", "MQTT client not connected");
        return false;
    }

    if(client.publish(topic, payload, retained)){
        char msgBuffer[128];
        snprintf(msgBuffer, sizeof(msgBuffer), "Published message ('%s', '%s')", topic, payload);
        log.debug("MQTTManager", msgBuffer);
        return true;
    }

    return false;
}

void MQTTManager::update(){
    if (!initialized) {
        return;
    }

    if(!client.connected()){
        RuntimeConfig& config = configManager.getRuntimeConfig();
        uint32_t now = millis();

        if(now - lastAttempt >= config.mqtt.retryInterval) {
            lastAttempt = now;
            if (connect()) {
                lastAttempt = 0;
            }
        }
    }else {
        client.loop();
    }
}

bool MQTTManager::isConnected(){
    return client.connected();
}   