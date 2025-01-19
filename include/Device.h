#ifndef DEVICE_H
#define DEVICE_H

#include "ConfigManager.h"
#include "states/DeviceState.h"
#include "states/SetupState.h"

class DeviceState;

class Device {
private:
    Device() : currentState(nullptr) {}
    DeviceState* currentState;

public:
    Device(const Device&) = delete;
    void operator=(const Device&) = delete;

    static Device& getInstance() {
        static Device instance;
        return instance;
    }

    void changeState(DeviceState& newState);
    void update();
};

#endif