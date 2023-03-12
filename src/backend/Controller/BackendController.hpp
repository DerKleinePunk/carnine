#pragma once
#include "../Config/BackendConfig.hpp"
#include "../communication/PowerSupplySerial.hpp"

class BackendController
{
    std::shared_ptr<PowerSupplySerial> _powerSerial;
    BackendConfig* _config;
    void PowerOffEvent();

  public:
    BackendController(BackendConfig* config);
    ~BackendController();

    int Init();
    int Start();
    void Stop();

    void CheckSystem();
};
