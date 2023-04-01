#pragma once

#include <string>

class MountPoint
{
private:
    std::string GetData();
    std::string _devPath;
public:
    MountPoint(const std::string& devPath);
    ~MountPoint();

    std::string Find();
    std::string Find(const std::string& devPath);
};
