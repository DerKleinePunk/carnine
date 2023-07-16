#pragma once

#include "../../modules/SDL2GuiHelper/src/MiniKernel.h"

class CarNiNeApp
{
private:
    MiniKernel* _kernel{};
    GUIElementManager* _manager{};
    
    void ApplicationEvent(AppEvent event, void* data1, void* data2);
    void KernelstateChanged(KernelState state);
public:
    CarNiNeApp(MiniKernel* kernel);
    ~CarNiNeApp();

    void Startup();
    void Shutdown();
};

