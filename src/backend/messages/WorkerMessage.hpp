#pragma once

#include <string>
#include "../../../modules/SDL2GuiHelper/common/json/json.hpp"
using json = nlohmann::json;

enum class worker_message_type : int {
	die,
	controller,
    all
};

std::ostream& operator<<(std::ostream& os, const worker_message_type c);

class WorkerMessage
{
public:
    WorkerMessage();
    ~WorkerMessage();
    worker_message_type _messageType;
    json _messageJson;
};