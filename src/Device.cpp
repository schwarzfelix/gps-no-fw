#include "Device.h"

void Device::changeState(DeviceState& newState) {
    if (currentState) {
        currentState->exit();
    }

    currentState = &newState;
    currentState->enter();
}

void Device::update() {
    if (currentState) {
        currentState->update();
    }
}