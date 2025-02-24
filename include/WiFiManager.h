#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "ConfigManager.h"
#include "Logger.h"

enum class WiFiStatus {
    UNINITIALIZED,
    DISCONNECTED,
    CONNECTING,
    CONNECTED, 
    CONNECTION_FAILED,
    WRONG_PASSWORD,
    NO_SSID_AVAILABLE,
    __DELIMITER__
};

class WiFiManager {
private:
    WiFiManager()
        : status(WiFiStatus::DISCONNECTED)
        , lastAttempt(0)
        , connectionAttempts(0)
        , configManager(ConfigManager::getInstance())
        , log(Logger::getInstance()) {}

    WiFiStatus status;
    uint32_t lastAttempt;
    uint8_t connectionAttempts;

    bool ftmSuccess;
    SemaphoreHandle_t ftmSemaphore;

    ConfigManager& configManager;
    Logger& log;

    const char *getWifiStatusString(WiFiStatus status);
    constexpr size_t getWifiStatusCount() {return static_cast<size_t>(WiFiStatus::__DELIMITER__);};

public:
    WiFiManager(const WiFiManager&) = delete;
    void operator=(const WiFiManager&) = delete;

    static WiFiManager& getInstance() {
        static WiFiManager instance;
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
    uint8_t* getBSSID();
    int32_t getRSSI();
    uint8_t getConnectionAttempts();
    
    bool getFtmReportConnected();
    bool getFtmReportBssid(uint8_t channel, byte mac[]);
    static void onFtmReport(arduino_event_t *event);
    void softAP();
    void scan();

    WiFiStatus getStatus();
    const char* getStatusString() { return getWifiStatusString(status); };
};

#endif