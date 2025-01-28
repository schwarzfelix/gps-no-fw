#include "states/SetupState.h"

void SetupState::enter() {
    log.debug("SetupState", "Entering SetupState");

    currentPhase = SetupPhase::INIT;
    setupStateTime = millis();
}

void SetupState::update() {
    switch(currentPhase) {
        case SetupPhase::INIT:
            if (!initializeManagers()) {
                currentPhase = SetupPhase::FAILED;
                return;
            }
            currentPhase = SetupPhase::WIFI_CONNECTING;
            break;
        case SetupPhase::WIFI_CONNECTING:
            handleWifiConnection();
            break;
        case SetupPhase::MQTT_CONNECTING:
            handleMqttConnection();
            break;
        case SetupPhase::COMPLETED:
            mqttManager.update();
            device->changeState(ActionState::getInstance(device));
            break;
        case SetupPhase::FAILED:
            handleSetupFailure();
            break;
        default:
            break;
    }

    if (millis() - setupStateTime > SETUP_TIMEOUT) {
        log.error("SetupState", "Setup timeout reached");
        currentPhase = SetupPhase::FAILED;
    }
}

void SetupState::exit() {
    log.debug("SetupState", "Exiting SetupState");

    if(mqttManager.isConnected()) {
        RuntimeConfig& config = configManager.getRuntimeConfig();
        String topic = config.mqtt.baseTopic + String(config.device.chipID) + "/status";
        mqttManager.publish(topic.c_str(), "online", true);
    }
}

bool SetupState::initializeManagers() {
    if(!wifiManager.begin()) {
        log.error("SetupState", "Failed to initialize WifiManager");
        return false;
    }

    if(!mqttManager.begin()) {
        log.error("SetupState", "Failed to initialize MQTTManager");
        return false;
    }

    return true;
}

void SetupState::handleWifiConnection(){
     wifiManager.update();
    
    switch (wifiManager.getStatus()) {
        case WifiStatus::DISCONNECTED:
            wifiManager.connect();
            break;
            
        case WifiStatus::CONNECTED:
            log.info("SetupState", "WiFi connected, proceeding to MQTT setup");
            currentPhase = SetupPhase::MQTT_CONNECTING;
            break;
            
        case WifiStatus::CONNECTION_FAILED:
        case WifiStatus::WRONG_PASSWORD:
        case WifiStatus::NO_SSID_AVAILABLE:
            handleConnectionError("WiFi connection failed");
            break;
            
        default:
            break;
    }
}

void SetupState::handleMqttConnection() {
    mqttManager.update();

    if (!mqttManager.isConnected()) {
        if (!mqttManager.connect()) {
            RuntimeConfig& config = configManager.getRuntimeConfig();
            mqttConnectionAttempts++;

            if (mqttConnectionAttempts >= config.mqtt.maxConnectionAttempts) {
                handleConnectionError("MQTT connection failed");
                return;
            }
            delay(500); 
            return;
        }
    }

    subscribeDefaultTopics();
    currentPhase = SetupPhase::COMPLETED;
}

void SetupState::subscribeDefaultTopics() {
    RuntimeConfig& config = configManager.getRuntimeConfig();
    String deviceTopic = config.mqtt.baseTopic + String(config.device.chipID) + "/#";
    
    mqttManager.subscribe(deviceTopic.c_str(), [this](const char* topic, const uint8_t* payload, unsigned int length) {
        handleDeviceMessage(topic, payload, length);
    });

    String configTopic = config.mqtt.baseTopic + String(config.device.chipID) + "/config";
    mqttManager.subscribe(configTopic.c_str(), [this](const char* topic, const uint8_t* payload, unsigned int length) {
        handleConfigMessage(topic, payload, length);
    });
}

void SetupState::handleDeviceMessage(const char* topic, const uint8_t* payload, unsigned int length) {
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    
    char logMessage[128];
    snprintf(logMessage, sizeof(logMessage), "Received message on topic %s: %s", topic, message);
    log.debug("SetupState", logMessage);
}

void SetupState::handleConfigMessage(const char* topic, const uint8_t* payload, unsigned int length) {
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    
    log.info("SetupState", "Received config update");
    // Hier könnte die Konfiguration verarbeitet werden
}

void SetupState::handleConnectionError(const char* message) {
    log.error("SetupState", message);
    ErrorState::getInstance(device).setError(ErrorCode::UNKNOWN_ERROR, this, message);
    currentPhase = SetupPhase::FAILED;
}

void SetupState::handleSetupFailure(){
    device->changeState(ErrorState::getInstance(device));
}
