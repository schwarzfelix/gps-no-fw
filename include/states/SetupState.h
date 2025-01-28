#ifndef SETUP_STATE_H
#define SETUP_STATE_H

#include <Device.h>
#include <WiFi.h>
#include <Logger.h>
#include "WifiManager.h"
#include "states/ErrorState.h"
#include "states/ActionState.h"

class SetupState : public DeviceState {
private:
    SetupState(Device* device) 
        : DeviceState(device, StateIdentifier::SETUP_STATE)
        , log(Logger::getInstance())
        , configManager(ConfigManager::getInstance())
        , wifiManager(WifiManager::getInstance())
        , lastWifiCheck(0)
        , isWifiConnected(false) 
        , lastWifiStatus(WifiStatus::UNINITIALIZED){};
    Logger& log;
    ConfigManager& configManager;
    WifiManager& wifiManager;
    uint32_t lastWifiCheck;
    bool isWifiConnected;
    WifiStatus lastWifiStatus;

public:
    SetupState(const SetupState&) = delete;
    void operator=(const SetupState&) = delete;

    static SetupState& getInstance(Device* device) {
        static SetupState instance(device);
        return instance;
    }

    void enter() override;
    void update() override;
    void exit() override;
};

#endif