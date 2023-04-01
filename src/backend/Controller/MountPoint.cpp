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
#include "../../../modules/SDL2GuiHelper/common/utils/commonutils.h"

//TODO find goot def
//https://github.com/util-linux/util-linux/blob/master/misc-utils/findmnt.c

#define PATH_PROC_MOUNTINFO "/proc/self/mountinfo"
#define PATH_PROC_MOUNTS "/proc/self/mounts"

/* error callback */
static int parser_errcb(struct libmnt_table *tb __attribute__ ((__unused__)),
			const char *filename, int line)
{
    LOG(WARNING) << "parse error";
    //warnx(_("%s: parse error at line %d -- ignored"), filename, line);
	return 1;
}

std::string GetData()
{
    //const char *root = mnt_fs_get_root(fs);
	//const char *spec = mnt_fs_get_srcpath(fs);

    return std::string();
}

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
    const auto path = access(PATH_PROC_MOUNTINFO, R_OK) == 0 ?
		PATH_PROC_MOUNTINFO :
		PATH_PROC_MOUNTS;
    
    mnt_table_set_parser_errcb(tb, parser_errcb);

    if (mnt_table_parse_file(tb, path) != 0)
    {
        LOG(ERROR) << "mnt_table_parse_file failed";

        return result;
    }

    auto itr = mnt_new_iter(MNT_ITER_FORWARD);
	if (!itr) {
		LOG(ERROR) << "failed to initialize libmount iterator";
		return result;
	}
    
    libmnt_fs *fs = nullptr;
    if(mnt_table_next_fs(tb, itr, &fs) == 0) {
        do
        {
            //const char* root = mnt_fs_get_root(fs);
            const auto source = utils::chartoStringNullSave(mnt_fs_get_source(fs));
            const auto target = utils::chartoStringNullSave(mnt_fs_get_target(fs));
            if(source == _devPath) {
                result = target;
                break;
            }
            
        }
        while(mnt_table_next_fs(tb, itr, &fs) == 0);
    }
   
    mnt_free_iter(itr);

    return result;
}

std::string MountPoint::Find(const std::string& devPath)
{
    _devPath = devPath;
    return Find();
}