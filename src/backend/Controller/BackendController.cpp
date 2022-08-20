#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "BackendController"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "BackendController.hpp"
#include "../../../modules/SDL2GuiHelper/common/easylogging/easylogging++.h"

BackendController::BackendController()
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
}

BackendController::~BackendController()
{
}

int BackendController::Init()
{
    return 0;
}

int BackendController::Start()
{
    return 0;
}

void BackendController::Stop()
{

}

void BackendController::CheckSystem()
{

}