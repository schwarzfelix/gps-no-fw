#include "ConfigManager.h"

void ConfigManager::loadDefaults() {
    setConfigFromDefines(&config);

    config.chipID = ESP.getEfuseMac();

    String mac = WiFi.macAddress();
    mac.toCharArray(config.macAddress, sizeof(config.macAddress));
    config.macAddress[sizeof(config.macAddress) - 1] = '\0';

    String hash = calculateHash(&config);
    strncpy(config.hash, hash.c_str(), sizeof(config.hash) - 1);
    config.hash[sizeof(config.hash) - 1] = '\0';
}

void ConfigManager::print(RuntimeConfig* config) {
    Serial.printf("Device Name: %s\n", config->deviceName);
    Serial.printf("Firmware Version: %s\n", config->firmwareVersion);
    Serial.printf("WiFi SSID: %s\n", config->wifiSSID);
    Serial.printf("WiFi Password: %s\n", config->wifiPassword);
    Serial.printf("MQTT Broker: %s\n", config->mqttBroker);
    Serial.printf("MQTT Port: %d\n", config->mqttPort);
    Serial.printf("MQTT User: %s\n", config->mqttUser);
    Serial.printf("MQTT Password: %s\n", config->mqttPassword);
    Serial.printf("MQTT Retry Interval: %d\n", config->mqttRetryInterval);
    Serial.printf("Chip ID: %llu\n", config->chipID);
    Serial.printf("MAC Address: %s\n", config->macAddress);
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
        String(config->deviceName) +
        String(config->wifiSSID) +
        String(config->wifiPassword) +
        String(config->mqttBroker) +
        String(config->mqttPort) +
        String(config->mqttUser) +
        String(config->mqttPassword) +
        String(config->mqttRetryInterval);

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

    if(!loadFromFlash()) {
        Serial.println(F("Failed to load config from flash, using defaults"));        
        if (!saveToFlash()) {
            Serial.println(F("Failed to save defaults to flash"));
            return false;
        }
    }

    initialized = true;
    return true;
}

void ConfigManager::setConfigFromDefines(RuntimeConfig* config) {
    memset(config, 0, sizeof(RuntimeConfig));
    strncpy(config->deviceName, DEVICE_NAME, sizeof(config->deviceName) - 1);
    config->deviceName[sizeof(config->deviceName) - 1] = '\0';

    strncpy(config->wifiSSID, WIFI_SSID, sizeof(config->wifiSSID) - 1);
    config->wifiSSID[sizeof(config->wifiSSID) - 1] = '\0';

    strncpy(config->wifiPassword, WIFI_PASSWORD, sizeof(config->wifiPassword) - 1);
    config->wifiPassword[sizeof(config->wifiPassword) - 1] = '\0';

    strncpy(config->mqttBroker, MQTT_BROKER, sizeof(config->mqttBroker) - 1);
    config->mqttBroker[sizeof(config->mqttBroker) - 1] = '\0';

    config->mqttPort = MQTT_PORT;

    strncpy(config->mqttUser, MQTT_USER, sizeof(config->mqttUser) - 1);
    config->mqttUser[sizeof(config->mqttUser) - 1] = '\0';

    strncpy(config->mqttPassword, MQTT_PASSWORD, sizeof(config->mqttPassword) - 1);
    config->mqttPassword[sizeof(config->mqttPassword) - 1] = '\0';

    config->mqttRetryInterval = MQTT_RETRY_INTERVAL;

    config->error.maxRecoveryAttempts = ERROR_MAX_RECOVERY_ATTEMPTS;
    config->error.recoveryInterval = ERROR_RECOVERY_INTERVAL;

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