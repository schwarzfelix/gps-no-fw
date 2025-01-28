#ifndef DEVICE_H
#define DEVICE_H

#include "ConfigManager.h"
#include "states/DeviceState.h"
#include "states/SetupState.h"
#include "MQTTManager.h"
#include "Logger.h"
#include <ArduinoJson.h>

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
    Device() 
        : currentState(nullptr)
        , lastStatusUpdate(0)
        , mqttManager(MQTTManager::getInstance())
        , configManager(ConfigManager::getInstance())
        , log(Logger::getInstance()) {}
    
    MQTTManager& mqttManager;
    ConfigManager& configManager;
    Logger& log;

    static const size_t JSON_DOC_SIZE = 512;
    DeviceState* currentState;
    uint32_t lastStatusUpdate;

    void sendDeviceStatus();

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
    DeviceState* getCurrentState() { return currentState; }
};

#endif