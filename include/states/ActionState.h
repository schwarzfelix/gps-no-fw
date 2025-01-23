#ifndef ACTION_SATE_H
#define ACTION_SATE_H

#include <Device.h>

class ActionState : public DeviceState {
private:
    ActionState(Device* device) 
        : DeviceState(device, StateIdentifier::ACTION_STATE)
        , log(Logger::getInstance())
        , configManager(ConfigManager::getInstance()) {};
    Logger& log;
    ConfigManager& configManager;

public:
    ActionState(const ActionState&) = delete;
    void operator=(const ActionState&) = delete;

    static ActionState& getInstance(Device* device) {
        static ActionState instance(device);
        return instance;
    }

    void enter() override;
    void update() override;
    void exit() override;

};

#endif