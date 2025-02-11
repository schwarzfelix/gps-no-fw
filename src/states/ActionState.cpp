#include "states/ActionState.h"
#include "WiFi.h"
#include "Logger.h"

extern "C" {
    #include "esp_wifi.h"  // ESP-IDF WiFi-API
    #include "esp_log.h"
}

void ftm_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_FTM_REPORT) {
        wifi_event_ftm_report_t* report = (wifi_event_ftm_report_t*) event_data;

        switch(report->status) {
            case FTM_STATUS_SUCCESS:
                Serial.println("FTM_STATUS_SUCCESS");
                break;
            case FTM_STATUS_UNSUPPORTED:
                Serial.println("FTM_STATUS_UNSUPPORTED");
                break;
            case FTM_STATUS_CONF_REJECTED:
                Serial.println("FTM_STATUS_CONF_REJECTED");
                break;
            case FTM_STATUS_NO_RESPONSE:
                Serial.println("FTM_STATUS_NO_RESPONSE");
                break;
            case FTM_STATUS_FAIL:
                Serial.println("FTM_STATUS_FAIL");
                break;
            default:
                Serial.println("Unknown status");
                break;
        }

        char msgBuffer[256];
        snprintf(msgBuffer, sizeof(msgBuffer), "rtt_raw=%d rtt_est=%d dist_est=%d ftm_report_num_entries=%d",
        //snprintf(msgBuffer, sizeof(msgBuffer), "peer_mac=%x:%x:%x:%x:%x:%x status=%d rtt_raw=%d rtt_est=%d dist_est=%d ftm_report_num_entries=%d",
                                                                                            /*report->peer_mac[0],
                                                                                            report->peer_mac[1],
                                                                                            report->peer_mac[2],
                                                                                            report->peer_mac[3],
                                                                                            report->peer_mac[4],
                                                                                            report->peer_mac[5],*/
                                                                                            report->rtt_raw,
                                                                                            report->rtt_est,
                                                                                            report->dist_est,
                                                                                            report->ftm_report_num_entries);
        Serial.println(msgBuffer);
    }
    else if (event_id == WIFI_EVENT_SCAN_DONE) {
        Serial.println("WIFI_EVENT_SCAN_DONE");
        wifi_ap_record_t ap_info;
        //esp_wifi_scan_get_ap_records(&ap_info);
        char msgBuffer[256];
        snprintf(msgBuffer, sizeof(msgBuffer), "ssid=%s rssi=%d authmode=%d pairwise_cipher=%d group_cipher=%d ftm_responder=%d ftm_initiator=%d",
                                                                                            ap_info.ssid,
                                                                                            ap_info.rssi,
                                                                                            ap_info.authmode,
                                                                                            ap_info.pairwise_cipher,
                                                                                            ap_info.group_cipher,
                                                                                            ap_info.ftm_responder,
                                                                                            ap_info.ftm_initiator);
        Serial.println(msgBuffer);
    }
    else {
        Serial.println("Unknown event");
    }
}

void ActionState::enter() {
    log.debug("ActionState", "Entering ActionState");
    WiFi.mode(WIFI_STA);
    log.debug("ActionState", "Starting FTM measurement");
    wifi_ftm_initiator_cfg_t cfg = {
        //.resp_mac = {0xd4, 0x24, 0xdd, 0xdb, 0x72, 0x55},
        .resp_mac = {0x7c, 0xdf, 0xa1, 0xe6, 0x87, 0x45},
        //.resp_mac = {0x45, 0x87, 0xe6, 0xa1, 0xdf, 0x7c},
        //7c:df:a1:e6:87:45
        .channel = 1,
        .frm_count = 32,
        .burst_period = 2
    };
    //esp_wifi_ftm_initiate_session(&cfg);
    //esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_FTM_REPORT, &ftm_event_handler, NULL, NULL);
    esp_wifi_scan_start(NULL, false);
    esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_SCAN_DONE, &ftm_event_handler, NULL, NULL);
    log.debug("ActionState", "Waiting for Response");
}

void ActionState::update() {
    MQTTManager::getInstance().update();
    //log.debug("ActionState", "Starting FTM measurement");
}

void ActionState::exit() {
    log.debug("ActionState", "Exiting ActionState");
}