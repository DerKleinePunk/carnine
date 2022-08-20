
#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "BackendConfig"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "BackendConfig.hpp"

#include <fstream>
#include <iomanip>

#include "../../../modules/SDL2GuiHelper/common/easylogging/easylogging++.h"
#include "../../../modules/SDL2GuiHelper/common/json/json.hpp"
#include "../../../modules/SDL2GuiHelper/common/utils/commonutils.h"

using json = nlohmann::json;

void to_json(json& j, const ConfigFile& p)
{
    j = json{
        { "UdpLogServer", p.udpLogServer },
    };
}

void from_json(const json& j, ConfigFile& p)
{
    auto it_value = j.find("UdpLogServer");
    if(it_value != j.end()) {
        p.udpLogServer = j.at("UdpLogServer").get<std::string>();
    } else {
        p.udpLogServer = "";
    }
}

BackendConfig::BackendConfig(const std::string& fileName)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _fileName = fileName;
}

BackendConfig::~BackendConfig()
{
}

void BackendConfig::Load()
{
    LOG(DEBUG) << "Try Loading Config from " << _fileName;

    if(utils::FileExists(_fileName)) {
        try {
            std::ifstream ifs(_fileName);
            const auto jConfig = json::parse(ifs);
            _configFile = jConfig;
            ifs.close();
        } catch(std::domain_error& exp) {
            LOG(ERROR) << "Loadconfig  " << exp.what();
        } catch(std::exception& exp) {
            LOG(ERROR) << "Loadconfig  " << exp.what();
        }
    } else {
        LOG(DEBUG) << "Create new config file";
        std::ofstream o(_fileName);
        const json jConfig = _configFile;
        o << std::setw(4) << jConfig << std::endl;
        o.close();
    }
}

void BackendConfig::Save()
{
}

std::string BackendConfig::GetUdpLogServer() const
{
    return _configFile.udpLogServer;
}
