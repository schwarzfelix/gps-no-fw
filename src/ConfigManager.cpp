#include "ConfigManager.h"

ConfigManager* ConfigManager::instance = nullptr;

ConfigManager::ConfigManager() : initialized(false) {
    loadDefaults();
}

ConfigManager* ConfigManager::getInstance() {
    if (instance == nullptr) {
        instance = new ConfigManager();
    }
    return instance;
}

void ConfigManager::loadDefaults() {
    strncpy(config.wifiSSID, WIFI_SSID, sizeof(config.wifiSSID) - 1);
    strncpy(config.wifiPassword, WIFI_PASSWORD, sizeof(config.wifiPassword) - 1);
    strncpy(config.mqttBroker, MQTT_BROKER, sizeof(config.mqttBroker) - 1);
    config.mqttPort = MQTT_PORT;
    strncpy(config.mqttUser, MQTT_USER, sizeof(config.mqttUser) - 1);
    strncpy(config.mqttPassword, MQTT_PASSWORD, sizeof(config.mqttPassword) - 1);
    config.mqttRetryInterval = MQTT_RETRY_INTERVAL;
    config.chipID = ESP.getEfuseMac();
    String mac = WiFi.macAddress();
    mac.toCharArray(config.macAddress, sizeof(config.macAddress));      
    
    String hash = calculateHash(&config);
    strncpy(config.hash, hash.c_str(), sizeof(config.hash) - 1);
}

void ConfigManager::print() {
    Serial.printf("Device Name: %s\n", config.deviceName);
    Serial.printf("Firmware Version: %s\n", config.firmwareVersion);
    Serial.printf("WiFi SSID: %s\n", config.wifiSSID);
    Serial.printf("WiFi Password: %s\n", config.wifiPassword);
    Serial.printf("MQTT Broker: %s\n", config.mqttBroker);
    Serial.printf("MQTT Port: %d\n", config.mqttPort);
    Serial.printf("MQTT User: %s\n", config.mqttUser);
    Serial.printf("MQTT Password: %s\n", config.mqttPassword);
    Serial.printf("MQTT Retry Interval: %d\n", config.mqttRetryInterval);
    Serial.printf("Chip ID: %llu\n", config.chipID);
    Serial.printf("MAC Address: %s\n", config.macAddress);
    Serial.printf("Config Hash: %s\n", config.hash);
}

String ConfigManager::calculateHash(RuntimeConfig* config) {
    MD5Builder md5;
    md5.begin();
    
    String configString = 
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

    if(!LittleFS.begin(true)) {
        Serial.println(F("Failed to mount file system"));
        return false;
    }

    if(!loadFromFlash()) {
        Serial.println(F("Failed to load config from flash, using defaults"));        
        saveToFlash();
    }

    initialized = true;
    return true;
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
    RuntimeConfig runtimeConfig;
    strncpy(runtimeConfig.wifiSSID, WIFI_SSID, sizeof(runtimeConfig.wifiSSID) - 1);
    strncpy(runtimeConfig.wifiPassword, WIFI_PASSWORD, sizeof(runtimeConfig.wifiPassword) - 1);
    strncpy(runtimeConfig.mqttBroker, MQTT_BROKER, sizeof(runtimeConfig.mqttBroker) - 1);
    runtimeConfig.mqttPort = MQTT_PORT;
    strncpy(runtimeConfig.mqttUser, MQTT_USER, sizeof(runtimeConfig.mqttUser) - 1);
    strncpy(runtimeConfig.mqttPassword, MQTT_PASSWORD, sizeof(runtimeConfig.mqttPassword) - 1);
    runtimeConfig.mqttRetryInterval = MQTT_RETRY_INTERVAL;

    String fileConfigHash = calculateHash(&runtimeConfig);
    Serial.printf("File Config Hash: %s - Stored Config Hash: %s\n", fileConfigHash.c_str(), config.hash);
    return strcmp(fileConfigHash.c_str(), config.hash) != 0;
}

void ConfigManager::updateDeviceConfig() {
    if(hasConfigDefinesChanged()) {
        loadDefaults();
        saveToFlash();
        Serial.println(F("Updated config"));
    }
}