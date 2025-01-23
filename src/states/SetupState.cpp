#include "states/SetupState.h"

void SetupState::enter() {
    log.debug("SetupState", "Entering SetupState");
    
    if (!wifiManager.begin()) {
        Logger::getInstance().error("SetupState", "WiFi initialization failed");
        device->changeState(ActionState::getInstance(device));        

        return;
    }

    wifiManager.connect();

}

void SetupState::update() {
    Serial.printf("Wifi Status: %s (%d)\n", wifiManager.getStatusString(), wifiManager.getConnectionAttempts());
    wifiManager.update();

    switch (wifiManager.getStatus()) {
        case WifiStatus::CONNECTING:
            break;
        case WifiStatus::CONNECTED:
            log.info("SetupState", "Init MQTT....");
            break;
        case WifiStatus::CONNECTION_FAILED:
            break;
        case WifiStatus::WRONG_PASSWORD:
            break;
        case WifiStatus::NO_SSID_AVAILABLE:
            log.error("SetupState", "WiFi connection failed");
            device->changeState(ErrorState::getInstance(device));
            break;
        default:
            break;
    }

    delay(500);
}

void SetupState::exit() {
    log.debug("SetupState", "Exiting SetupState");
}