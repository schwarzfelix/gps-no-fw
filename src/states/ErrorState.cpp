#include "states/ErrorState.h"

void ErrorState::enter() {
    log.debug("ErrorState", "Entering ErrorState");
}

void ErrorState::update() {
    
}

void ErrorState::exit() {
    log.debug("ErrorState", "Exiting ErrorState");
}

void ErrorState::setError(ErrorCode errorCode, DeviceState* sourceState, const char* message) {
    this->errorCode = errorCode;
    this->sourceState = sourceState;
    this->errorMessage = errorMessage;

    char msgBuffer[1024];
    snprintf(msgBuffer, sizeof(msgBuffer), "Error occurred: %s - %s", 
                getErrorCodeString(errorCode), message);
        log.error("ErrorState", msgBuffer);
    
    recoveryAttempts = 0;
    lastRecoveryAttempt = millis();
}

const char* ErrorState::getErrorCodeString(ErrorCode errorCode) {
    switch (errorCode) {
        case ErrorCode::BOOT_FAILED:
            return "BOOT_FAILED";
        case ErrorCode::CONFIG_INVALID:
            return "CONFIG_INVALID";
        case ErrorCode::WIFI_CONNECTION_FAILED:
            return "WIFI_CONNECTION_FAILED";
        case ErrorCode::MQTT_CONNECTION_FAILED:
            return "MQTT_CONNECTION_FAILED";
        default:
            return "UNKNOWN_ERROR";
    }
}