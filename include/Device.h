#ifndef DEVICE_H
#define DEVICE_H

#include "ConfigManager.h"
#include "states/DeviceState.h"
#include "states/SetupState.h"
#include "Logger.h"

enum class DeviceStatus {
    BOOTING,
    IDLE,
    SETUP,
    TRANSITIONING,
    ACTION,
    ERROR,
    __DELIMITER__
};

class DeviceState;

class Device {
private:
    Device() : currentState(nullptr) {}
    
    DeviceState* currentState;

    const char* getDeviceStatusString(DeviceStatus status);
    constexpr size_t getDeviceStatusCount() {return static_cast<size_t>(DeviceStatus::__DELIMITER__);};

public:
    Device(const Device&) = delete;
    void operator=(const Device&) = delete;

    static Device& getInstance() {
        static Device instance;
        return instance;
    }

    void changeState(DeviceState& newState);
    void update();

    const char* getDeviceStatusString();
};

#endif