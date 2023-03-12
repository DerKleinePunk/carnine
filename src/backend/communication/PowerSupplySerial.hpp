#pragma once
#include <functional>

#include "../../common/communication/BaseSerial.hpp"

typedef std::function<void()> powerOffCallbackFunction;

class PowerSupplySerial : public BaseSerial
{
    powerOffCallbackFunction _powerOffCallback;

    bool _shutdownInWork;
    void AnalyseBuffer(int count) override;
    void HandleSystemState(char value);
    void HandleCommandResult(char value);
    void HandleExternVoltage(char* value);
    void HandlePiAlive(char* value);
    void HandleKlemme15(char value);
    void PowerOffSystemdBus(int sec);

  public:
    PowerSupplySerial(const std::string& portname, powerOffCallbackFunction callback);
    ~PowerSupplySerial();
    void ShutdownSystem();
    void PowerOnAmp();
};