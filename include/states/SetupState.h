#ifndef SETUP_STATE_H
#define SETUP_STATE_H

#include <Device.h>
#include <WiFi.h>
#include <Logger.h>

class SetupState : public DeviceState {
private:
    SetupState(Device* device) 
        : DeviceState(device, StateIdentifier::SETUP_STATE)
        , log(Logger::getInstance())
        , configManager(ConfigManager::getInstance())
        , lastWifiCheck(0)
        , isWifiConnected(false) {};
    Logger& log;
    ConfigManager& configManager;
    uint32_t lastWifiCheck;
    bool isWifiConnected;

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