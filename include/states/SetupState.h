#ifndef SETUP_STATE_H
#define SETUP_STATE_H

#include "ConfigManager.h"
#include "DeviceState.h"
#include <WiFi.h>
#include "Device.h"

class SetupState : public DeviceState {
private:
    SetupState(Device* device) 
        : DeviceState(device)
        , lastWifiCheck(0)
        , isWifiConnected(false) {};
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