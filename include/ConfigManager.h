#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <MD5Builder.h>
#include "WiFi.h"
#include "config/ConfigDefines.h"

struct RuntimeConfig {
    char deviceName[32];
    char firmwareVersion[16];
    char wifiSSID[32];
    char wifiPassword[64];
    char mqttBroker[64];
    uint16_t mqttPort;
    char mqttUser[32];
    char mqttPassword[64];
    uint32_t wifiCheckInterval;
    uint32_t mqttRetryInterval;
    uint64_t chipID;
    char macAddress[18];    
    char hash[33];
};

class ConfigManager {
private:
    ConfigManager() : initialized(false) {
        loadDefaults();
    }
    RuntimeConfig config;
    static constexpr const char* CONFIG_FILE = "/config.bin";
    bool initialized;

    String calculateHash(RuntimeConfig* config);
    bool loadFromFlash();
    bool saveToFlash();
    void loadDefaults();
    void setConfigFromDefines(RuntimeConfig* config);

public:
    ConfigManager(const ConfigManager&) = delete;
    void operator=(const ConfigManager&) = delete;

    static ConfigManager& getInstance(){
        static ConfigManager instance;
        return instance;
    }
    bool begin();
    RuntimeConfig& getRuntimeConfig() { return config; }
    bool hasConfigDefinesChanged();
    void updateDeviceConfig();
    void print(RuntimeConfig* config);

    const char* getDeviceName() const { return config.deviceName; }
    const char* getFirmwareVersion() const { return config.firmwareVersion; }
    const char* getWifiSSID() const { return config.wifiSSID; }
    const char* getWifiPassword() const { return config.wifiPassword; }
    const char* getMqttBroker() const { return config.mqttBroker; }
    uint16_t getMqttPort() const { return config.mqttPort; }
    const char* getMqttUser() const { return config.mqttUser; }
    const char* getMqttPassword() const { return config.mqttPassword; }
    uint32_t getMqttRetryInterval() const { return config.mqttRetryInterval; }
    uint64_t getChipID() const { return config.chipID; }
    const char* getMacAddress() const { return config.macAddress; }
    const char* getHash() const { return config.hash; }
    uint32_t getWifiCheckInterval() const { return config.wifiCheckInterval; }
};

#endif