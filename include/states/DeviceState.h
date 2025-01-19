#ifndef DEVICE_STATE_H
#define DEVICE_STATE_H

class Device;

class DeviceState {
protected:
    Device* device;
    DeviceState(Device* device);

public:
    DeviceState(const DeviceState&) = delete;
    void operator=(const DeviceState&) = delete;
    virtual ~DeviceState() = default;    
    
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update() = 0;
};

#endif