#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "BackendController"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include <unistd.h>

#include "BackendController.hpp"

#include "../../../modules/SDL2GuiHelper/common/easylogging/easylogging++.h"
#include "../../common/messages/BackendMessages.hpp"

void BackendController::PowerOffEvent()
{
    // Be carfull other Thread

    auto message = new WorkerMessage();
    message->_messageType = worker_message_type::all;
    message->_sender = worker_sender_type::Backenend;
    powerOff  messageJson;
    message->_messageJson = messageJson;
    const auto rc = write(_backendPipe, &message, sizeof(message));
    if(rc != sizeof(message)) {
        LOG(ERROR) << "Writing intern Pipe " << strerror(errno);
    }
}

BackendController::BackendController(BackendConfig* config, int backendPipe)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _config = config;
    _backendPipe = backendPipe;
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

void BackendController::HandleWorkerMessage(WorkerMessage* message)
{
    LOG(INFO) << "HandleWorkerMessage " << message->_messageType;
    if(message->_messageType == worker_message_type::controller) {
        baseMessage messageJson = message->_messageJson;
        if(messageJson.type == backend_message_type::powerSupplyWatchdog) {
            powerSupplyWatchdog messageIntern = message->_messageJson;
            if(_powerSerial != nullptr && messageIntern.state == false) {
                _powerSerial->ServiceModeOn();
            }
        }
        else if(messageJson.type == backend_message_type::newStorageDetected) {
            newStorageDetected messageIntern = message->_messageJson;
            LOG(INFO) << "newStorageDetected " << messageIntern.mountPath;
        }
        else if(messageJson.type == backend_message_type::newCamDetected) {
            newCamDetected messageIntern = message->_messageJson;
            LOG(INFO) << "newCamDetected " << messageIntern.devPath;
        }
    }

    LOG(DEBUG) << "HandleWorkerMessage <-";
}