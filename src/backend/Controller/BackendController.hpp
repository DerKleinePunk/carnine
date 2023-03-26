#pragma once
#include "../Config/BackendConfig.hpp"
#include "../communication/PowerSupplySerial.hpp"
#include "../messages/WorkerMessage.hpp"

class BackendController
{
    std::shared_ptr<PowerSupplySerial> _powerSerial;
    BackendConfig* _config;
    int _backendPipe;
    void PowerOffEvent();

  public:
    BackendController(BackendConfig* config, int backendPipe);
    ~BackendController();

    int Init();
    int Start();
    void Stop();

    void CheckSystem();

    void HandleWorkerMessage(WorkerMessage* message);
};
