#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "UdevController"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include <systemd/sd-daemon.h>
#include <systemd/sd-event.h>
#include <systemd/sd-journal.h>
#include <libudev.h> 
#include <unistd.h>

#include "UdevController.hpp"

#include "../../../modules/SDL2GuiHelper/common/easylogging/easylogging++.h"
#include "../../../modules/SDL2GuiHelper/common/utils/commonutils.h"
#include "../messages/WorkerMessage.hpp"
#include "../../common/messages/BackendMessages.hpp"
#include "MountPoint.hpp"

#define from_hex(c)		(isdigit(c) ? c - '0' : tolower(c) - 'a' + 10)

size_t unhexmangle_to_buffer(const char *s, char *buf, size_t len)
{
	size_t sz = 0;
	const char *buf0 = buf;

	if (!s)
		return 0;

	while(*s && sz < len - 1) {
		if (*s == '\\' && sz + 3 < len - 1 && s[1] == 'x' &&
		    isxdigit(s[2]) && isxdigit(s[3])) {

			*buf++ = from_hex(s[2]) << 4 | from_hex(s[3]);
			s += 4;
			sz += 4;
		} else {
			*buf++ = *s++;
			sz++;
		}
	}
	*buf = '\0';
	return buf - buf0 + 1;
}

static int dispatch_device_udev(sd_event_source *es, int fd, uint32_t revents, void *userdata) {
    const auto controller = (UdevController*) userdata;
    return controller->HandleDeviceMessage();
}

UdevController::UdevController(int backendPipe)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _backendPipe = backendPipe;
    _udevContext = nullptr;
    _udevDeviceMonitor = nullptr;
    _udev_device_event_source = nullptr;
    _event = nullptr;
}

UdevController::~UdevController()
{
    _udev_device_event_source = sd_event_source_unref(_udev_device_event_source);
    udev_monitor_unref(_udevDeviceMonitor);
    udev_unref(_udevContext);
    _event = sd_event_unref(_event);
}

int UdevController::Init()
{
    auto result = sd_event_default(&_event);
    if(result < 0) { 
        return result;
    }
    
    _udevContext = udev_new();
    _udevDeviceMonitor = udev_monitor_new_from_netlink(_udevContext, "udev");
    if (!_udevDeviceMonitor) {
        return -ENOMEM;
    }

    //result = udev_monitor_filter_add_match_subsystem_devtype(udevDeviceMonitor, "usb", NULL);
    result = udev_monitor_filter_add_match_subsystem_devtype(_udevDeviceMonitor, "usb", "usb_device");
    if (result < 0) {
        return result;
    }

    //Cam Connected
    result = udev_monitor_filter_add_match_subsystem_devtype(_udevDeviceMonitor, "video4linux", NULL);
    if (result < 0) {
        return result;
    }

    //Usb Stick Mounted
    result = udev_monitor_filter_add_match_subsystem_devtype(_udevDeviceMonitor, "block", NULL);
    if (result < 0) {
        return result;
    }

    result = udev_monitor_enable_receiving(_udevDeviceMonitor);
    if (result < 0) {
        return result;
    }

    result = sd_event_add_io(_event, &_udev_device_event_source, udev_monitor_get_fd(_udevDeviceMonitor), EPOLLIN, dispatch_device_udev, this);
    if (result < 0) {
        return result;
    }

    sd_event_source_set_description(_udev_device_event_source, "CarNiNe Message Udev");

    return 0;
}

int UdevController::HandleDeviceMessage()
{
    const auto device = udev_monitor_receive_device(_udevDeviceMonitor);
    if (!device) {
        return -ENOMEM;
    }

    const auto action = utils::chartoStringNullSave(udev_device_get_action(device));
    const auto devType = utils::chartoStringNullSave(udev_device_get_devtype(device));
    const auto subSystem = utils::chartoStringNullSave(udev_device_get_subsystem(device));

    LOG(INFO) << "subSystem " << subSystem << " action " << action << " DevType " << devType;

    if (action == "remove") {
                
    } else if(action == "add" ) {
        const auto devPath = utils::chartoStringNullSave(udev_device_get_devpath(device));
        const auto sysPath = utils::chartoStringNullSave(udev_device_get_syspath(device));
        const auto sysName = utils::chartoStringNullSave(udev_device_get_sysname(device));

        LOG(DEBUG) << "devPath " << devPath << " sysPath " << sysPath << " sysName " << sysName;

        if(subSystem == "video4linux" && !sysName.empty()) {
            auto message = new WorkerMessage();
            message->_messageType = worker_message_type::all;
            message->_sender = worker_sender_type::Udev;
            newCamDetected messageJson;
            messageJson.devPath = "/dev/" + sysName;

            message->_messageJson = messageJson;
            const auto rc = write(_backendPipe, &message, sizeof(message));
            if(rc != sizeof(message)) {
                LOG(ERROR) << "Writing intern Pipe " << strerror(errno);
            }

        }
    } else if(action == "bind" ) {
        const auto devPath = utils::chartoStringNullSave(udev_device_get_devpath(device));
        const auto sysPath = utils::chartoStringNullSave(udev_device_get_syspath(device));
        const auto sysName = utils::chartoStringNullSave(udev_device_get_sysname(device));

        LOG(DEBUG) << "bind devPath " << devPath << " sysPath " << sysPath << " sysName " << sysName;
        
    } else if(action == "change" && devType == "partition") {
        
        auto label = std::string("");
        auto labelChars = udev_device_get_property_value(device, "ID_FS_LABEL_ENC");
        if(labelChars != nullptr) {

            auto buffer = new char[strlen(labelChars) + 1];
            auto size = unhexmangle_to_buffer(labelChars, buffer, strlen(labelChars) + 1);
            label = std::string(buffer, size);
            
            delete [] buffer;
        }

        const auto devPath = utils::chartoStringNullSave(udev_device_get_devpath(device));
        const auto devNode = utils::chartoStringNullSave(udev_device_get_devnode(device));
        auto sysName = utils::chartoStringNullSave(udev_device_get_sysname(device));

        LOG(INFO) << "label" << label << "\ndevPath " << devPath << "\nsysName " << sysName;

        sysName = "/dev/" + sysName;
        auto mountPoint = new MountPoint(sysName);

        auto message = new WorkerMessage();
        message->_messageType = worker_message_type::controller;
        message->_sender = worker_sender_type::Udev;
        newStorageDetected messageJson;
        messageJson.mountPath = mountPoint->Find();

        message->_messageJson = messageJson;
        const auto rc = write(_backendPipe, &message, sizeof(message));
        if(rc != sizeof(message)) {
            LOG(ERROR) << "Writing intern Pipe " << strerror(errno);
        }

        delete mountPoint;
    }
    
    udev_device_unref(device);

    return 0;
}