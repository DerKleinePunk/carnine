
#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "BackendConfig"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "BackendConfig.hpp"
#include "../../../modules/SDL2GuiHelper/common/easylogging/easylogging++.h"

BackendConfig::BackendConfig()
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
}

BackendConfig::~BackendConfig()
{
}
