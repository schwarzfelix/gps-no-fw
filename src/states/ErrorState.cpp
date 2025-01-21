#include "states/ErrorState.h"

void ErrorState::enter() {
    log.debug("ErrorState", "Entering ErrorState");
}

void ErrorState::update() {
    
}

void ErrorState::exit() {
    log.debug("ErrorState", "Exiting ErrorState");
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