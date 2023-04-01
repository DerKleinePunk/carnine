#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "MountPoint"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "MountPoint.hpp"
#include "../../../modules/SDL2GuiHelper/common/easylogging/easylogging++.h"
#include <libmount.h>
#include <unistd.h>

MountPoint::MountPoint(const std::string& devPath)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _devPath = devPath;
	mnt_init_debug(0);
}

MountPoint::~MountPoint()
{
}

std::string MountPoint::Find()
{
    std::string result;
    auto tb = mnt_new_table();
    //auto cache = mnt_new_cache();
    //mnt_table_set_cache(tb, cache);
    const auto path = access(_PATH_PROC_MOUNTINFO, R_OK) == 0 ?
		_PATH_PROC_MOUNTINFO :
		_PATH_PROC_MOUNTS;
    if (mnt_table_parse_file(tb, path) != 0)
    {
        LOG(ERROR) << "mnt_table_parse_file failed";

        return result;
    }

    return result;
}

std::string MountPoint::Find(const std::string& devPath)
{
    _devPath = devPath;
    return Find();
}