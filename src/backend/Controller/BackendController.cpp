#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "BackendController"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "BackendController.hpp"

#include "../../../modules/SDL2GuiHelper/common/easylogging/easylogging++.h"

void BackendController::PowerOffEvent()
{
    // Be carfull other Thread
}

BackendController::BackendController(BackendConfig* config)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _config = config;
}

BackendController::~BackendController()
{
}

int BackendController::Init()
{
    LOG(DEBUG) << "Init ->";

    auto powerOffdelegate = std::bind(&BackendController::PowerOffEvent, this);
    if(!_config->GetPowerSupplyPort().empty()) {
        _powerSerial = std::make_shared<PowerSupplySerial>(_config->GetPowerSupplyPort(), powerOffdelegate);
        if(_powerSerial->Open(B38400)) {
            LOG(DEBUG) << "PowerSupplySerial is open";
        } else {
            LOG(ERROR) << "PowerSupplySerial not open";
        }
    } else {
        LOG(DEBUG) << "PowerSupplySerial is disabled";
    }

    LOG(DEBUG) << "Init <-";
    return 0;
}

int BackendController::Start()
{
    LOG(DEBUG) << "Start ->";
    if(_powerSerial != nullptr) {
        if(_powerSerial->Start() == 0) {
            LOG(DEBUG) << "PowerSupplySerial Up and running";
        }
    }
    LOG(DEBUG) << "Start <-";
    return 0;
}

void BackendController::Stop()
{
    LOG(DEBUG) << "Stop ->";
    if(_powerSerial != nullptr) {
        _powerSerial->Close();
    }
    LOG(DEBUG) << "Stop <-";
}

void BackendController::CheckSystem()
{
    LOG(DEBUG) << "CheckSystem ->";
    LOG(DEBUG) << "CheckSystem <-";
}