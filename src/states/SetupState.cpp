#include "states/SetupState.h"

void SetupState::enter() {
    log.debug("SetupState", "Entering SetupState");
    
    if (!wifiManager.begin()) {
        log.error("SetupState", "WiFi initialization failed");
        device->changeState(ActionState::getInstance(device));        

        return;
    }

    wifiManager.connect();

}

void SetupState::update() {
    wifiManager.update();

    if(wifiManager.getStatus() != lastWifiStatus) {
        char msgBuffer[64];
        snprintf(msgBuffer, sizeof(msgBuffer), "WiFi Status changed to: %s", wifiManager.getStatusString());
        log.debug("SetupState", msgBuffer);
        lastWifiStatus = wifiManager.getStatus();
    }

    switch (wifiManager.getStatus()) {
        case WifiStatus::CONNECTING:
            break;
        case WifiStatus::CONNECTED:
            char msgBuffer[128];
            snprintf(msgBuffer, sizeof(msgBuffer), 
                "Connected to WiFi: %s, IP: %s, RSSI: %d dBm", 
                wifiManager.getSSID().c_str(),
                wifiManager.getIP().c_str(),
                wifiManager.getRSSI());
            log.info("SetupState", msgBuffer);
            device->changeState(ActionState::getInstance(device));
            break;
        case WifiStatus::CONNECTION_FAILED:
            break;
        case WifiStatus::WRONG_PASSWORD:
            break;
        case WifiStatus::NO_SSID_AVAILABLE:
            log.error("SetupState", "WiFi connection failed");
            ErrorState::getInstance(device).setError(
                ErrorCode::WIFI_CONNECTION_FAILED,
                this,
                "Failed to establish WiFi connection"
            );            
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