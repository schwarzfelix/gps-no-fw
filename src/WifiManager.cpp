#include "WiFiManager.h"

const uint8_t FTM_FRAME_COUNT = 16;
const uint16_t FTM_BURST_PERIOD = 2;
const char *WIFI_FTM_SSID = "FTM";
const char *WIFI_FTM_PASS = "password";

const char* WiFiManager::getWifiStatusString(WiFiStatus status) {
    switch (status) {
        case WiFiStatus::DISCONNECTED: return "DISCONNECTED";
        case WiFiStatus::CONNECTING: return "CONNECTING";
        case WiFiStatus::CONNECTED: return "CONNECTED";
        case WiFiStatus::CONNECTION_FAILED: return "CONNECTION_FAILED";
        case WiFiStatus::WRONG_PASSWORD: return "WRONG_PASSWORD";
        case WiFiStatus::NO_SSID_AVAILABLE: return "NO_SSID_AVAILABLE";
        default: return "UNKNOWN";
    }
};

bool WiFiManager::begin(){
    log.debug("WiFiManager", "Initializing WiFiManager...");

    RuntimeConfig& config = configManager.getRuntimeConfig();

    if(strlen(config.wifi.ssid) == 0) {
        log.warning("WiFiManager", "No SSID available, skipping WiFiManager initialization");
        return false;
    } else if (strlen(config.wifi.password) == 0) {
        log.warning("WiFiManager", "No password available, skipping WiFiManager initialization");
        return false;
    } 

    ftmSemaphore = xSemaphoreCreateBinary();
    WiFi.onEvent(onFtmReport, ARDUINO_EVENT_WIFI_FTM_REPORT);

    WiFi.mode(WIFI_STA);
    return true;
}

bool WiFiManager::isConnected() {
    return status == WiFiStatus::CONNECTED && WiFi.status() == WL_CONNECTED;
}

bool WiFiManager::connect() { 
    if(status == WiFiStatus::CONNECTED) {
        return true;
    }

    RuntimeConfig& config = configManager.getRuntimeConfig();

    char msgBuffer[256];
    snprintf(msgBuffer, sizeof(msgBuffer), "Attempting to connect to Wifi-AP '%s' (['%s', %d], ['%s', %d])", config.wifi.ssid, config.wifi.ssid, strlen(config.wifi.ssid), config.wifi.password, strlen(config.wifi.password));
    log.debug("WiFiManager", msgBuffer);

    //WiFi.setMinSecurity(WIFI_AUTH_WEP); 
    WiFi.begin(config.wifi.ssid, config.wifi.password);

    status = WiFiStatus::CONNECTING;
    lastAttempt = millis();
    connectionAttempts = 1;

    return true;
}

void WiFiManager::disconnect() {
    log.debug("WiFiManager", "Disconnecting from Wifi...");

    WiFi.disconnect();
    status = WiFiStatus::DISCONNECTED;
}

void WiFiManager::update(){
    RuntimeConfig &config = configManager.getRuntimeConfig();
    if(status == WiFiStatus::CONNECTING){
        if (WiFi.status() == WL_CONNECTED){
            status = WiFiStatus::CONNECTED;
            connectionAttempts = 0;

            char msgBuffer[64];
            snprintf(msgBuffer, sizeof(msgBuffer), "Connected to Wifi-AP with IP: %s", WiFi.localIP().toString().c_str());
            log.debug("WiFiManager", msgBuffer);

            return;
        }

        if (millis() - lastAttempt >= config.wifi.checkInterval) {
            lastAttempt = millis();
            connectionAttempts++;

            Serial.printf("Connection Attempts: %d (%d)\n", connectionAttempts, config.wifi.maxConnectionAttempts);
            if (connectionAttempts >= config.wifi.maxConnectionAttempts) {
                status = WiFiStatus::CONNECTION_FAILED;
                char msgBuffer[192];
                snprintf(msgBuffer, sizeof(msgBuffer), "Failed to connect to Wifi-AP ('%s') due reaching max connection attempts", config.wifi.ssid);
                log.error("WiFiManager", msgBuffer);

                return;
            }
        }
    } else if (status == WiFiStatus::CONNECTED && WiFi.status() != WL_CONNECTED) {
        status = WiFiStatus::DISCONNECTED;
        char msgBuffer[192];
        snprintf(msgBuffer, sizeof(msgBuffer), "Lost connection to Wifi-AP ('%s')", config.wifi.ssid);
        log.warning("WiFiManager", msgBuffer);

        if (config.wifi.autoReconnect && millis() - lastAttempt >= config.wifi.reconnectInterval) {
            connect();
        }
    }
}

WiFiStatus WiFiManager::getStatus(){
    return status;
}

String WiFiManager::getIP() {
    IPAddress localIP = WiFi.localIP();
    return String(localIP[0]) + "." + String(localIP[1]) + "." + String(localIP[2]) + "." + String(localIP[3]);
}

String WiFiManager::getSSID() {
    return WiFi.SSID();
}

int32_t WiFiManager::getRSSI() {
    return WiFi.RSSI();
}

uint8_t WiFiManager::getConnectionAttempts() {
    return connectionAttempts;
}

uint8_t* WiFiManager::getBSSID() {
    return WiFi.BSSID();
}

bool WiFiManager::getFtmReportConnected() {

    Serial.print("Initiating FTM session to Connected with Frame Count ");
    Serial.print(FTM_FRAME_COUNT);
    Serial.print(" and Burst Period ");
    Serial.print(FTM_BURST_PERIOD * 100);
    Serial.println(" ms");

    if (!WiFi.initiateFTM(FTM_FRAME_COUNT, FTM_BURST_PERIOD)) {
        Serial.println("FTM Error: Initiate Session Failed");
        return false;
    }
    
    return xSemaphoreTake(ftmSemaphore, portMAX_DELAY) == pdPASS && ftmSuccess;
}

//TODO single method with optional parameters

bool WiFiManager::getFtmReportBssid(uint8_t channel, byte mac[]) {

    Serial.print("Initiating FTM session to Other with Frame Count ");
    Serial.print(FTM_FRAME_COUNT);
    Serial.print(" and Burst Period ");
    Serial.print(FTM_BURST_PERIOD * 100);
    Serial.println(" ms");

    if (!WiFi.initiateFTM(FTM_FRAME_COUNT, FTM_BURST_PERIOD, channel, mac)) {
        Serial.println("FTM Error: Initiate Session Failed");
        return false;
    }
    
    return xSemaphoreTake(ftmSemaphore, portMAX_DELAY) == pdPASS && ftmSuccess;
}

void WiFiManager::onFtmReport(arduino_event_t *event) {
    const char *status_str[5] = {"SUCCESS", "UNSUPPORTED", "CONF_REJECTED", "NO_RESPONSE", "FAIL"};
    wifi_event_ftm_report_t *report = &event->event_info.wifi_ftm_report;
    WiFiManager::getInstance().ftmSuccess = report->status == FTM_STATUS_SUCCESS;
    if (WiFiManager::getInstance().ftmSuccess) {
        Serial.printf("FTM Estimate: Distance: %.2f m, Return Time: %lu ns\n", (float)report->dist_est / 100.0, report->rtt_est);
        free(report->ftm_report_data);
    } else {
        Serial.print("FTM Error: ");
        Serial.println(status_str[report->status]);
    }
    xSemaphoreGive(WiFiManager::getInstance().ftmSemaphore);
}

void WiFiManager::softAP(){
    WiFi.softAP(WIFI_FTM_SSID, WIFI_FTM_PASS, 1, 0, 4, true);
}