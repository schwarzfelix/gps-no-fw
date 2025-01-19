#include "states/SetupState.h"

void SetupState::enter() {
    Serial.println(F("Entering SetupState"));
    
    ConfigManager& configManager = ConfigManager::getInstance();

    WiFi.mode(WIFI_STA);
    WiFi.begin(configManager.getWifiSSID(), configManager.getWifiPassword());
}

void SetupState::update() {

}

void SetupState::exit() {
    Serial.println(F("Exiting SetupState"));
}