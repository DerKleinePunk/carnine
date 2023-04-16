/**
* @file main.cpp
* @author Michael Nenninger
* @brief The App Class
*/

#ifndef ELPP_DEFAULT_LOGGER
#   define ELPP_DEFAULT_LOGGER "CarNiNeApp"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#   define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "../../modules/SDL2GuiHelper/common/easylogging/easylogging++.h"
#include "CarNiNeApp.hpp"

CarNiNeApp::CarNiNeApp(MiniKernel* kernel)
{
    _kernel = kernel;
}

CarNiNeApp::~CarNiNeApp()
{
}

void CarNiNeApp::Startup()
{

}

void CarNiNeApp::Shutdown()
{
    
}