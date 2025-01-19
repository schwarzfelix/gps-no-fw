#include <Arduino.h>
#include <Wire.h>
#include <ConfigManager.h>
#include <Device.h>


void setup() {
  Serial.begin(MONITOR_SPEED);
  Serial.println(F("###################################################"));
  Serial.println(F("(c) 2023-2024 Hochschule Bochum GPS:NO - Martin Peth"));
  Serial.printf("Compiled with c++ version %s", __VERSION__);
  Serial.println();
  Serial.printf("Version v--- @--- %s at %s", __DATE__, __TIME__);
  Serial.println();
  Serial.println(F("###################################################"));

  ConfigManager& configManager = ConfigManager::getInstance();
  Device& device = Device::getInstance();

  if(!configManager.begin()) {
    Serial.println(F("Failed to initialize ConfigManager"));
    while(true);
  }

  if (configManager.hasConfigDefinesChanged()) {
    Serial.println(F("ConfigDefines has changed, updating device config"));
    configManager.updateDeviceConfig();
  }

  Serial.println(F("###################################################"));
  configManager.print(&configManager.getRuntimeConfig());
  Serial.println(F("###################################################"));

  device.changeState(SetupState::getInstance(&device));
}

void loop() {
  Device::getInstance().update();
}