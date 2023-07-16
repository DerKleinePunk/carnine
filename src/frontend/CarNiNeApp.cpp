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

const SDL_Color my_light_blue = { 0x00, 0xA1, 0x9C, 0xff };

void CarNiNeApp::ApplicationEvent(AppEvent event, void* data1, void* data2)
{

}

void CarNiNeApp::KernelstateChanged(KernelState state)
{
    if (state == KernelState::Startup) {
        LOG(INFO) << "Kernel Start Core Services";
#ifdef ELPP_FEATURE_PERFORMANCE_TRACKING
        TIMED_SCOPE_IF(timerCoreServices, "StartCoreServices", VLOG_IS_ON(4));
#endif
        _kernel->StartCoreServices();

        LOG(INFO) << "Kernel is up Create Screen";

#ifdef ELPP_FEATURE_PERFORMANCE_TRACKING
    	TIMED_SCOPE_IF(timerCreateScreen, "CreateScreen", VLOG_IS_ON(4));
#endif
        _manager = _kernel->CreateScreen("CarNiNe", "", "", false);
        _kernel->DrawTextOnBootScreen("Starting Services");

        LOG(INFO) << "Starting Services";
    }
}

CarNiNeApp::CarNiNeApp(MiniKernel* kernel)
{
    _kernel = kernel;
}

CarNiNeApp::~CarNiNeApp()
{
}

void CarNiNeApp::Startup()
{
    auto statedelegate = std::bind(&CarNiNeApp::KernelstateChanged, this, std::placeholders::_1);
    _kernel->SetStateCallBack(statedelegate);
    auto eventdelegate = std::bind(&CarNiNeApp::ApplicationEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    _kernel->RegisterApplicationEvent(eventdelegate);

    KernelConfig config;
    //config.AudioFileForClick = _config->GetSoundForClick();
    //config.AudioFileForLongClick = _config->GetSoundForClick();
    config.mapDataPath = "/home/punky/develop/sdl2guitests/src/maps/europe/germany/hessen";
    std::vector<std::string> iconPaths;
    iconPaths.emplace_back("/home/punky/develop/libosmscout/libosmscout/data/icons/14x14/standard/");
    iconPaths.emplace_back("/home/punky/develop/libosmscout/libosmscout/data/icons/svg/standard/");
    config.mapIconPaths = iconPaths;
    config.mapStyle = "/home/punky/develop/libosmscout/stylesheets/standard.oss";
    //config.markerImageFile = _config->GetMarkerImageFile();
    config.startMapPosition.Set(50.094,8.49617);
    config.BackgroundScreen = black_color;
    config.ForegroundScreen = my_light_blue;

    _kernel->SetConfig(config);
}

void CarNiNeApp::Shutdown()
{
    
}