#pragma once

#include <libudev.h> 

class UdevController
{
private:
    int _backendPipe;
    udev* _udevContext;
    udev_monitor* _udevDeviceMonitor;
    sd_event_source* _udev_device_event_source;
    sd_event* _event;
public:
    UdevController(int backendPipe);
    ~UdevController();

    int Init();
    int HandleDeviceMessage();
};


