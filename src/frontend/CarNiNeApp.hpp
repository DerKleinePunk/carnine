#pragma once

#include "../../modules/SDL2GuiHelper/src/MiniKernel.h"

class CarNiNeApp
{
private:
    MiniKernel* _kernel;
public:
    CarNiNeApp(MiniKernel* kernel);
    ~CarNiNeApp();

    void Startup();
    void Shutdown();
};

