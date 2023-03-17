#pragma once

#include <string>
#include "../../../modules/SDL2GuiHelper/common/json/json.hpp"
using json = nlohmann::json;

class WorkerMessage
{
public:
    WorkerMessage();
    ~WorkerMessage();
    std::string _messageType;
    std::string _messageData;
    json _messageJson;
};