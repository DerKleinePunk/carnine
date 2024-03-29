#pragma once
#include "map"
#include "string"
#include "vector"

namespace utils
{
class CommandLineArgs
{
  private:
    std::map<std::string, std::string> _parameterWithValue;
    std::vector<std::string> _parameter;
    std::string _basePath;

  public:
    CommandLineArgs();
    ~CommandLineArgs();

    void Pharse(int argc, char** argv);
    bool HasParamWithValue(const std::string& key);
    bool HasParam(const std::string& key);
    std::string GetParamValue(const std::string& key);
    std::string GetBasePath();
};

} // namespace utils
