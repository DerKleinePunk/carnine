#pragma once

#include <string>

struct ConfigFile {
    std::string udpLogServer;
};

class BackendConfig
{
private:
    std::string _fileName;
    ConfigFile _configFile;
public:
    BackendConfig(const std::string& fileName);
    ~BackendConfig();

    void Load();
    void Save();

    std::string GetUdpLogServer() const;
};

