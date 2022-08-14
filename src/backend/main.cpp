#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "Main"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include <iostream>
#include <systemd/sd-daemon.h>
#include <systemd/sd-journal.h>
#include <systemd/sd-event.h>

#include "../../modules/SDL2GuiHelper/common/easylogging/easylogging++.h"
#include "../../modules/SDL2GuiHelper/common/utils/commonutils.h"
#include "../common/utils/CommandLineArgs.h"
#include "../common/version.hpp"

INITIALIZE_EASYLOGGINGPP

void PreRollOutCallback(const char* fullPath, std::size_t s)
{
    char newPath[1024];
    char newPath2[1024];

    std::string str(fullPath);

    std::string pathNoExtension = str.substr(0, str.find_last_of('.'));

    int ver = 99;

    sprintf(newPath, "%s.log.%d", pathNoExtension.c_str(), ver);
    if(utils::FileExists(newPath)) {
        remove(newPath);
    }

    for(ver = 98; ver >= 1; ver--) {
        sprintf(newPath, "%s.log.%d", pathNoExtension.c_str(), ver);
        sprintf(newPath2, "%s.log.%d", pathNoExtension.c_str(), ver + 1);
        if(utils::FileExists(newPath)) {
            rename(newPath, newPath2);
        }
    }

    rename(fullPath, newPath);
}

static sd_event *event = nullptr;

void handleUserInterrupt(int sig) {
    LOG(DEBUG) << "handleUserInterrupt " << std::to_string(sig);
    if(sig == SIGINT || sig == SIGABRT) {
        auto const result = sd_event_exit(event, 99);
        std::cout << "sd_event_exit " << result << std::endl;
    } else if(sig == SIGTERM) {
        //usb->SetInShutdown();
        auto const result = sd_event_exit(event, 0);
        std::cout << "sd_event_exit " << result << std::endl;
    }
  
}

int main(int argc, char **argv)
{
    int exit_code = EXIT_SUCCESS;
    int eventLoopResult;
    int functionResult;

    std::cout << "Starting CarNiNe Backend " << PROJECT_VER << std::endl;
    START_EASYLOGGINGPP(argc, argv);

    std::string argv_str(argv[0]);
    std::string base = argv_str.substr(0, argv_str.find_last_of("/"));

    if(utils::FileExists(base+"/loggerBackend.conf")) {
        // Load configuration from file
        el::Configurations conf(base+"/loggerBackend.conf");
        // Now all the loggers will use configuration from file and new loggers
        el::Loggers::setDefaultConfigurations(conf, true);
    } else {
        sd_journal_print(LOG_WARNING, "Logger Config not Found");
        std::cout << "Logger Config not Found" << std::endl;
    }

    el::Helpers::setThreadName("Main");
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    el::Helpers::installPreRollOutCallback(PreRollOutCallback);

    signal(SIGINT, handleUserInterrupt);
    signal(SIGTERM, handleUserInterrupt);
    signal(SIGABRT, handleUserInterrupt);

    LOG(INFO) << "Starting CarNiNe Backend " << PROJECT_VER;

    if(sd_event_default(&event) < 0) {
        sd_journal_print(LOG_ERR, "Cannot instantiate the event loop.");
        exit_code = 201;
        goto finish;
    }

    sigset_t ss;
    if(sigemptyset(&ss) < 0) {
        exit_code = errno;
        goto finish;
    } else {
        LOG(DEBUG) << "sigemptyset ok";
    }
    
    if(sigaddset(&ss, SIGTERM) < 0) {
        exit_code = errno;
        goto finish;
    } else {
        LOG(DEBUG) << "sigaddset SIGTERM ok";
    }

    if(sigaddset(&ss, SIGINT) < 0) {
        exit_code = errno;
        goto finish;
    }
    
    if(sigaddset(&ss, SIGHUP) < 0) {
        exit_code = errno;
        goto finish;
    }
    
    if (sigprocmask(SIG_BLOCK, &ss, NULL) < 0) {
        exit_code = errno;
        goto finish;
    }

    functionResult = sd_event_add_signal(event, NULL, SIGTERM, NULL, NULL);
    if( functionResult < 0) {
        exit_code = 202;
        sd_journal_print(LOG_ERR, "Cannot add_signal SIGTERM");
        goto finish;
    }

    functionResult = sd_event_add_signal(event, NULL, SIGINT, NULL, NULL);
    if(functionResult < 0) {
        sd_journal_print(LOG_ERR, "Cannot add_signal SIGINT");
        exit_code = 203;
        goto finish;
    }
    
    functionResult = sd_event_add_signal(event, NULL, SIGHUP, NULL, NULL);
    if( functionResult < 0) {
        sd_journal_print(LOG_ERR, "Cannot add_signal SIGHUP %d", functionResult);
        exit_code = 204;
        goto finish;
    }
    
    sd_notify(false, "READY=1\n" "STATUS=Up and running.");

    LOG(INFO) << "Up and running.";
    
    eventLoopResult = sd_event_loop(event);
    if (eventLoopResult < 0) {
        sd_journal_print(LOG_ERR, "Failure: %s\n", strerror(-eventLoopResult));
        exit_code = -eventLoopResult;
    }

    LOG(INFO) << "After loop begin shutdown";
    
finish:

    LOG(INFO) << "Last LogEntry";
    el::Helpers::uninstallPreRollOutCallback();

    return exit_code;
}