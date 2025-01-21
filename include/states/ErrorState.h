#ifndef ERROR_STATE_H
#define ERROR_STATE_H

#include <Device.h>
#include <Logger.h>

enum class ErrorCode {
    BOOT_FAILED,
    CONFIG_INVALID,
    WIFI_CONNECTION_FAILED,
    MQTT_CONNECTION_FAILED,
    UNKNOWN_ERROR
};

class ErrorState : public DeviceState {
private:
    ErrorState(Device* device, ErrorCode errorCode) 
        : DeviceState(device, StateIdentifier::ERROR_STATE)
        , errorCode(ErrorCode::UNKNOWN_ERROR)
        , log(Logger::getInstance())
        , configManager(ConfigManager::getInstance()) {};
    Logger& log;
    ConfigManager& configManager;
    ErrorCode errorCode;
    DeviceState* sourceState;
    const char* errorMessage;
    uint8_t recoveryAttempts;
    unsigned long lastRecoveryAttempt;

    const char* getErrorCodeString(ErrorCode errorCode);

public:
    ErrorState(const ErrorState&) = delete;
    void operator=(const ErrorState&) = delete;

    static ErrorState& getInstance(Device* device, ErrorCode errorCode) {
        static ErrorState instance(device, errorCode);
        return instance;
    }

    void setError(ErrorCode errorCode, DeviceState* sourceState, const char* errorMessage);

    void enter() override;
    void update() override;
    void exit() override;
};

#endif  