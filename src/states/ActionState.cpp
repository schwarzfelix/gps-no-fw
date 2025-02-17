#include "states/ActionState.h"
#include "WiFi.h"
#include "Logger.h"

void ActionState::enter() {
    log.debug("ActionState", "Entering ActionState");
}

void ActionState::update() {
    MQTTManager::getInstance().update();
}

void ActionState::exit() {
    log.debug("ActionState", "Exiting ActionState");
}