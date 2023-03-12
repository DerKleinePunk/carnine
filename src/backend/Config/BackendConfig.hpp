#pragma once

#include <string>

struct ConfigFile {
    std::string udpLogServer;
    std::string powerSupplyPort;
};

class BackendConfig
{
    std::string _fileName;
    ConfigFile _configFile;

  public:
    BackendConfig(const std::string& fileName);
    ~BackendConfig();

    void Load();
    void Save();

    std::string GetUdpLogServer() const;
    std::string GetPowerSupplyPort() const;
};
