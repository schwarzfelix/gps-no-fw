#include "states/SetupState.h"

void SetupState::enter() {
    log.debug("SetupState", "Entering SetupState");
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(configManager.getWifiSSID(), configManager.getWifiPassword());
}

void SetupState::update() {
}

void SetupState::exit() {
    log.debug("SetupState", "Exiting SetupState");
}