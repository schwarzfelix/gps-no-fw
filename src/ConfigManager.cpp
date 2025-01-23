#include "ConfigManager.h"

void ConfigManager::loadDefaults() {
    setConfigFromDefines(&config);

    config.device.chipID = ESP.getEfuseMac();

    String mac = WiFi.macAddress();
    mac.toCharArray(config.device.macAddress, sizeof(config.device.macAddress));
    config.device.macAddress[sizeof(config.device.macAddress) - 1] = '\0';

    String hash = calculateHash(&config);
    strncpy(config.hash, hash.c_str(), sizeof(config.hash) - 1);
    config.hash[sizeof(config.hash) - 1] = '\0';
}

void ConfigManager::print(RuntimeConfig* config) {
    Serial.printf("Device Name: %s\n", config->device.name);
    Serial.printf("Firmware Version: %s\n", config->device.firmwareVersion);
    Serial.printf("WiFi SSID: %s\n", config->wifi.ssid);
    Serial.printf("WiFi Password: %s\n", config->wifi.password);
    Serial.printf("WiFi Auto Reconnect: %s\n", config->wifi.autoReconnect ? "true" : "false");
    Serial.printf("WiFi Check Interval: %d\n", config->wifi.checkInterval);
    Serial.printf("WiFi Reconnect Interval: %d\n", config->wifi.reconnectInterval);
    Serial.printf("WiFi Max Connection Attempts: %d\n", config->wifi.maxConnectionAttempts);
    Serial.printf("MQTT Broker: %s\n", config->mqtt.broker);
    Serial.printf("MQTT Port: %d\n", config->mqtt.port);
    Serial.printf("MQTT User: %s\n", config->mqtt.user);
    Serial.printf("MQTT Password: %s\n", config->mqtt.password);
    Serial.printf("MQTT Retry Interval: %d\n", config->mqtt.retryInterval);
    Serial.printf("Chip ID: %llu\n", config->device.chipID);
    Serial.printf("MAC Address: %s\n", config->device.macAddress);
    Serial.printf("Error Max Recovery Attempts: %d\n", config->error.maxRecoveryAttempts);
    Serial.printf("Error Recovery Interval: %d\n", config->error.recoveryInterval);
    Serial.printf("Logging Allow MQTT Log: %s\n", config->logging.allowMqttLog ? "true" : "false");
    Serial.printf("Logging MQTT Topic: %s\n", config->logging.mqttTopic);
    Serial.printf("Logging Level: %d\n", config->logging.logLevel);
    Serial.printf("Config Hash: %s\n", config->hash);
}

String ConfigManager::calculateHash(RuntimeConfig* config) {
    MD5Builder md5;
    md5.begin();
    
    String configString = 
        String(config->device.name) +
        String(config->wifi.ssid) +
        String(config->wifi.password) +
        String(config->mqtt.broker) +
        String(config->mqtt.port) +
        String(config->mqtt.user) +
        String(config->mqtt.password) +
        String(config->mqtt.retryInterval);

    md5.add(configString);
    md5.calculate();

    return md5.toString();
}

bool ConfigManager::begin() {
    if (initialized) {
        return true;
    }

    if (ESP.getFreeHeap() < 10000) {
        Serial.println(F("Warning: Low memory"));
    }

    if(!LittleFS.begin(true)) {
        Serial.println(F("Failed to mount file system"));
        loadDefaults();
        return false;
    }

    if (LittleFS.totalBytes() - LittleFS.usedBytes() < sizeof(RuntimeConfig)) {
        Serial.println(F("Warning: Low storage space"));
    }

    if(!DEBUG_FORCE_CONFIG){
        if(!loadFromFlash()) {
            Serial.println(F("Failed to load config from flash, using defaults"));        
            if (!saveToFlash()) {
                Serial.println(F("Failed to save defaults to flash"));
                return false;
            }
        }
    }

    initialized = true;
    return true;
}

void ConfigManager::setConfigFromDefines(RuntimeConfig* config) {
    memset(config, 0, sizeof(RuntimeConfig));
    /* #### DEVICE ####*/
    strncpy(config->device.name, DEVICE_NAME, sizeof(config->device.name) - 1);
    config->device.name[sizeof(config->device.name) - 1] = '\0';

    /* #### WIFI ####*/
    strncpy(config->wifi.ssid, WIFI_SSID, sizeof(config->wifi.ssid) - 1);
    config->wifi.ssid[sizeof(config->wifi.ssid) - 1] = '\0';

    strncpy(config->wifi.password, WIFI_PASSWORD, sizeof(config->wifi.password) - 1);
    config->wifi.password[sizeof(config->wifi.password) - 1] = '\0';

    config->wifi.autoReconnect = WIFI_AUTO_RECONNECT;
    config->wifi.checkInterval = WIFI_CHECK_INTERVAL;
    config->wifi.reconnectInterval = WIFI_RECONNECT_INTERVAL;
    config->wifi.maxConnectionAttempts = WIFI_MAX_CONNECTION_ATTEMPTS;

    /* #### MQTT ####*/
    strncpy(config->mqtt.broker, MQTT_BROKER, sizeof(config->mqtt.broker) - 1);
    config->mqtt.broker[sizeof(config->mqtt.broker) - 1] = '\0';

    config->mqtt.port = MQTT_PORT;

    strncpy(config->mqtt.user, MQTT_USER, sizeof(config->mqtt.user) - 1);
    config->mqtt.user[sizeof(config->mqtt.user) - 1] = '\0';

    strncpy(config->mqtt.password, MQTT_PASSWORD, sizeof(config->mqtt.password) - 1);
    config->mqtt.password[sizeof(config->mqtt.password) - 1] = '\0';

    config->mqtt.retryInterval = MQTT_RETRY_INTERVAL;

    /* #### ERROR ####*/
    config->error.maxRecoveryAttempts = ERROR_MAX_RECOVERY_ATTEMPTS;
    config->error.recoveryInterval = ERROR_RECOVERY_INTERVAL;

    /* #### LOGGING ####*/
    config->logging.allowMqttLog = LOGGING_ALLOW_MQTT_LOG;
    config->logging.logLevel = LOGGING_LEVEL;

    strncpy(config->logging.mqttTopic, LOGGING_MQTT_TOPIC, sizeof(config->logging.mqttTopic) - 1);
    config->logging.mqttTopic[sizeof(config->logging.mqttTopic) - 1] = '\0';
}

bool ConfigManager::loadFromFlash() {
    File file = LittleFS.open(CONFIG_FILE, "r");
    if(!file || file.size() != sizeof(RuntimeConfig)) {
        if (file) file.close();
        return false;
    }

    if (file.readBytes((char*)&config, sizeof(RuntimeConfig)) != sizeof(RuntimeConfig)) {
        file.close();
        return false;
    }

    file.close();
    return true;
}

bool ConfigManager::saveToFlash() {
    File file = LittleFS.open(CONFIG_FILE, "w");
    if(!file) {
        return false;
    }

    if (file.write((const uint8_t*)&config, sizeof(RuntimeConfig)) != sizeof(RuntimeConfig)) {
        file.close();
        return false;
    }

    file.close();
    return true;
}

bool ConfigManager::hasConfigDefinesChanged() {
    RuntimeConfig defaultConfig;
    setConfigFromDefines(&defaultConfig);

    String defaultConfigHash = calculateHash(&defaultConfig);
    Serial.printf("File Config Hash: %s - Stored Config Hash: %s\n", defaultConfigHash.c_str(), config.hash);
    return strcmp(defaultConfigHash.c_str(), config.hash) != 0;
}

void ConfigManager::updateDeviceConfig() {
    loadDefaults();
    saveToFlash();
    Serial.println(F("Updated config"));
}