#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "ConfigManager.h"
#include "Logger.h"

enum class WifiStatus {
    DISCONNECTED,
    CONNECTING,
    CONNECTED, 
    CONNECTION_FAILED,
    WRONG_PASSWORD,
    NO_SSID_AVAILABLE,
    __DELIMITER__
};

class WifiManager {
private:
    WifiManager()
        : status(WifiStatus::DISCONNECTED)
        , lastAttempt(0)
        , connectionAttempts(0)
        , configManager(ConfigManager::getInstance())
        , log(Logger::getInstance()) {}

    WifiStatus status;
    uint32_t lastAttempt;
    uint8_t connectionAttempts;

    ConfigManager& configManager;
    Logger& log;

    const char *getWifiStatusString(WifiStatus status);
    constexpr size_t getWifiStatusCount() {return static_cast<size_t>(WifiStatus::__DELIMITER__);};

public:
    static WifiManager& getInstance() {
        static WifiManager instance;
        return instance;
    }

    bool begin();
    void update();
    bool connect();
    void disconnect();
    void reconnect();
    void printStatus();
    bool isConnected();
    void setAutoReconnect(bool isEnabled);
    
    String getIP();
    String getSSID();
    int32_t getRSSI();
    uint8_t getConnectionAttempts();

    WifiStatus getStatus();
    const char* getStatusString() { return getWifiStatusString(status); };
};

#endif