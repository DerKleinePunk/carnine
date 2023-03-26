#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "PowerSupplySerial"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

// Todo  ann this to an shared h file for this and AVR

// Protokoll Defs
#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define IDKL15 0x10
#define PIALIVECOUNT 0x11
#define SYSTEMSTATE 0x12
#define COMMANDRESULT 0x13
#define EXTERNVOLTAGE 0x14
#define NACK 0x15

#define STATE_IDLE 0x00
#define STATE_POWERON 0x01
#define STATE_PIBOOT 0x02
#define STATE_RUN 0x03
#define STATE_POWEROFF 0x04


#include <linux/reboot.h>
#include <sys/reboot.h>
#include <systemd/sd-bus.h>
#include <systemd/sd-journal.h>

#include "../../../modules/SDL2GuiHelper/common/easylogging/easylogging++.h"
#include "../../common/utils/time-util.h"
#include "PowerSupplySerial.hpp"

void PowerSupplySerial::PowerOffSystemdBus(int sec)
{
    if(_shutdownInWork) {
        LOG(DEBUG) << "Shudown ist already started";
        return;
    }

    sd_bus* bus = nullptr;
    sd_bus_error error = SD_BUS_ERROR_NULL;
    uint64_t arg_when = 0;

    sd_journal_print(LOG_DEBUG, "powerOffSystemdBus %d sec", sec);

    char date[FORMAT_TIMESTAMP_MAX];

    arg_when = now(CLOCK_REALTIME) + USEC_PER_SEC * sec;

    // Connect to the system bus
    int result = sd_bus_open_system(&bus);
    if(result < 0) {
        sd_journal_print(LOG_ERR, "Failed to connect to system bus: %s", strerror(-result));
        return;
    }

    // https://freedesktop.org/wiki/Software/systemd/logind/

    result = sd_bus_call_method(bus, "org.freedesktop.login1", "/org/freedesktop/login1",
                                "org.freedesktop.login1.Manager", "SetWallMessage", &error, NULL,
                                "sb", "Car Engine Stopp", true);
    if(result < 0) {
        sd_journal_print(LOG_ERR, "Failed to call SetWallMessage in logind");
    }

    result = sd_bus_call_method(bus, "org.freedesktop.login1", "/org/freedesktop/login1",
                                "org.freedesktop.login1.Manager", "ScheduleShutdown", &error, NULL,
                                "st", "poweroff", arg_when);
    if(result < 0) {
        LOG(ERROR)
        << "Failed to call ScheduleShutdown in logind, proceeding with immediate shutdown: "
        << error.message;
        sd_journal_print(LOG_ERR, "Failed to call ScheduleShutdown in logind, proceeding with immediate shutdown: %s %d",
                         error.message, result);
    } else {
        _shutdownInWork = true;
        LOG(INFO) << "Shutdown scheduled for " << format_timestamp(date, sizeof(date), arg_when);
        sd_journal_print(LOG_INFO, "Shutdown scheduled for %s, use 'shutdown -c' to cancel.",
                         format_timestamp(date, sizeof(date), arg_when));
    }
}

PowerSupplySerial::PowerSupplySerial(const std::string& portname, powerOffCallbackFunction callback)
    : BaseSerial(portname)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _shutdownInWork = false;
    LOG(DEBUG) << "ctor PowerSupplySerial with " << portname;
    _powerOffCallback = callback;
}

PowerSupplySerial::~PowerSupplySerial()
{
}

void PowerSupplySerial::AnalyseBuffer(int count)
{
    currentReadPos_ += count;
    VLOG(3) << currentReadPos_ << " Bytes in buffer";
    auto lastEnding = 0;
    for(auto pos = 0; pos < currentReadPos_; pos++) {
        if(readBuffer_[pos] == ETX) { // ETX Found Binary Message
            lastEnding = pos + 1;
            auto inPos = pos;
            while(inPos >= 0) {
                inPos--;
                if(readBuffer_[inPos] == STX) { // STX Found
                    switch(readBuffer_[inPos + 1]) {
                    case SYSTEMSTATE:
                        HandleSystemState(readBuffer_[inPos + 2]);
                        break;
                    case COMMANDRESULT:
                        HandleCommandResult(readBuffer_[inPos + 2]);
                        break;
                    case EXTERNVOLTAGE: {
                        auto count = pos - inPos - 1;
                        const auto buffer = new char[count];
                        memset(buffer, 0, count);
                        memcpy(buffer, &readBuffer_[inPos + 2], count - 1);
                        HandleExternVoltage(buffer);
                        delete[] buffer;
                    } break;
                    case PIALIVECOUNT: {
                        auto count = pos - inPos - 1;
                        const auto buffer = new char[count];
                        memset(buffer, 0, count);
                        memcpy(buffer, &readBuffer_[inPos + 2], count - 1);
                        HandlePiAlive(buffer);
                        delete[] buffer;
                    } break;
                    case IDKL15:
                        HandleKlemme15(readBuffer_[inPos + 2]);
                        break;
                    default:
                        LOG(DEBUG) << std::to_string((int)readBuffer_[inPos + 1]) << " not implemented";
                    }
                    break;
                }
            }
        } else if(readBuffer_[pos] == 0x0A) { // New Line Found Text Message
            std::string received(&readBuffer_[lastEnding], pos - lastEnding - 1);
            lastEnding = pos + 1;
            VLOG(1) << "Debug Info: " << received;
            // only a test
            if(received == "off") {
                // powerOffSystemdBus(10);
                if(_powerOffCallback != nullptr) {
                    _powerOffCallback();
                }
            }
        } else if(readBuffer_[pos] == 0x1B) {
            readBuffer_[pos] = '*';
        }
    }

    if(lastEnding == currentReadPos_) {
        VLOG(3) << "reset readbuffer";
        memset(readBuffer_, 0, MAXBUFFERSIZE);
        currentReadPos_ = 0;
    } else if(lastEnding > 0) {
        VLOG(3) << "move readbuffer";
        memcpy(readBuffer_, &readBuffer_[lastEnding], currentReadPos_ - lastEnding);
        currentReadPos_ = currentReadPos_ - lastEnding;
        memset(&readBuffer_[currentReadPos_], 0, MAXBUFFERSIZE - currentReadPos_);
        if(VLOG_IS_ON(3)) {
            std::string received;
            for(auto pos = 0; pos < currentReadPos_; pos++) {
                if(readBuffer_[pos] == STX) {
                    received += "<STX>";
                } else if(readBuffer_[pos] == ETX) {
                    received += "<ETX>";
                } else if(readBuffer_[pos] == 0x06) {
                    received += "<0x06>";
                } else if(readBuffer_[pos] == 0x10) {
                    received += "<0x10>";
                } else if(readBuffer_[pos] == 0x11) {
                    received += "<0x11>";
                } else if(readBuffer_[pos] == 0x0A) {
                    received += "<0x0A>";
                } else if(readBuffer_[pos] == 0x0D) {
                    received += "<0x0D>";
                } else if(readBuffer_[pos] == 0x1B) {
                    received += "<ESC>";
                } else {
                    received += readBuffer_[pos];
                }
            }
            VLOG(3) << received;
        }
    }
}

void PowerSupplySerial::HandleSystemState(char value)
{
    if(value - 0x30 == STATE_IDLE) {
        LOG(INFO) << "PowerSupply State STATE_IDLE";
    } else if(value - 0x30 == STATE_POWERON) {
        LOG(INFO) << "PowerSupply State STATE_POWERON";
    } else if(value - 0x30 == STATE_PIBOOT) {
        LOG(INFO) << "PowerSupply State STATE_PIBOOT";
    } else if(value - 0x30 == STATE_RUN) {
        LOG(INFO) << "PowerSupply State STATE_RUN";
    } else {
        LOG(INFO) << "PowerSupply State " << value;
    }
    value = value - 0x30;
    if(value == STATE_POWEROFF) {
        if(!_shutdownInWork) {
            if(_powerOffCallback != nullptr) {
                _powerOffCallback();
            }
            PowerOffSystemdBus(1);
        }
    } else if(value == STATE_IDLE) {
        // Nothing
    } else {
        Write("+"); // Write Alive
    }
}

void PowerSupplySerial::HandleCommandResult(char value)
{
    if(value == NACK) {
        LOG(WARNING) << "PowerSuply return Nack";
    }
}

void PowerSupplySerial::HandleExternVoltage(char* value)
{
    LOG(INFO) << "ExternVoltage " << value;
}

void PowerSupplySerial::HandlePiAlive(char* value)
{
}

void PowerSupplySerial::HandleKlemme15(char value)
{
}

void PowerSupplySerial::ShutdownSystem()
{
    if(!_shutdownInWork) {
        PowerOffSystemdBus(2);
    }
    Write("$");
}

void PowerSupplySerial::PowerOnAmp()
{
    //TODO make Config able
    Write("3");
}

void PowerSupplySerial::ServiceModeOn()
{
    Write("!");
}