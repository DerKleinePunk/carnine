#pragma once

#define BACKENDPROTOCOLVER "0.1"

#include <string>

#include "../../../modules/SDL2GuiHelper/common/easylogging/easylogging++.h"
#include "../../../modules/SDL2GuiHelper/common/json/json.hpp"

using json = nlohmann::json;

enum class backend_message_type : int16_t {
    undefined = -1,
	base,
	welcome,
    powerSupplyWatchdog,
    powerOff,
    newCamDetected,
    newStorageDetected,
};

struct baseMessage {
    baseMessage(backend_message_type messageType, const std::string& ver)
    {
        type = messageType;
        version = ver;
    }
    baseMessage()
    {
        type = backend_message_type::undefined;
    }
    std::string version;
    backend_message_type type;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(baseMessage, version, type)
};



struct welcomeMessage : public baseMessage {
    welcomeMessage() : baseMessage(backend_message_type::welcome, BACKENDPROTOCOLVER) {

    }
    std::string systemName;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(welcomeMessage, version, type, systemName)
};

struct powerSupplyWatchdog : public baseMessage {
    powerSupplyWatchdog() : baseMessage(backend_message_type::powerSupplyWatchdog, BACKENDPROTOCOLVER) {

    }
    bool state;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(powerSupplyWatchdog, version, type, state)
};



struct powerOff : public baseMessage {
    powerOff() : baseMessage(backend_message_type::powerOff, BACKENDPROTOCOLVER) {

    }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(powerOff, version, type)
};

struct newCamDetected : public baseMessage {
    newCamDetected() : baseMessage(backend_message_type::newCamDetected, BACKENDPROTOCOLVER) {
        devPath = "";
    }

    std::string devPath;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(newCamDetected, version, type, devPath)
};



struct newStorageDetected : public baseMessage {
    newStorageDetected() : baseMessage(backend_message_type::newStorageDetected, BACKENDPROTOCOLVER) {
        mountPath = "";
    }

    std::string mountPath;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(newStorageDetected, version, type, mountPath)
};
