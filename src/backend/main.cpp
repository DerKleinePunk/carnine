#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "Main"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include <systemd/sd-daemon.h>
#include <systemd/sd-event.h>
#include <systemd/sd-journal.h>

#include <iostream>

#include "../../modules/SDL2GuiHelper/common/easylogging/easylogging++.h"
#include "../../modules/SDL2GuiHelper/common/utils/commonutils.h"
#include "../common/utils/CommandLineArgs.hpp"
#include "../common/utils/LogNetDispatcher.hpp"
#include "../common/version.hpp"
#include "Controller/BackendController.hpp"
#include "Config/BackendConfig.hpp"

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

static sd_event* event = nullptr;

void handleUserInterrupt(int sig)
{
    LOG(DEBUG) << "handleUserInterrupt " << std::to_string(sig);
    if(sig == SIGINT || sig == SIGABRT) {
        auto const result = sd_event_exit(event, 99);
        std::cout << "sd_event_exit " << result << std::endl;
    } else if(sig == SIGTERM) {
        // usb->SetInShutdown();
        auto const result = sd_event_exit(event, 0);
        std::cout << "sd_event_exit " << result << std::endl;
    }
}

BackendConfig* config = nullptr;
BackendController* backendController = nullptr;

//Time Callback called every ?
static const uint64_t stats_every_usec = 10 * 1000000;
/* These counters are reset in display_stats(). */
static size_t received_counter = 0, sent_counter = 0;

/* display_stats is expected to be called by the event loop. */
static int display_stats(sd_event_source *es, uint64_t now, void *userdata) {
  
    sd_notifyf(false, "STATUS=%zu telegrams send in the last %d seconds.", sent_counter, (unsigned int)(stats_every_usec / 1000000));
    sd_notifyf(false, "STATUS=%zu telegrams received in the last %d seconds.", received_counter, (unsigned int)(stats_every_usec / 1000000));
    
    if(backendController != nullptr) {
        backendController->CheckSystem();
    }
    
    sd_event_source_set_time(es, now + stats_every_usec); /* reschedules */
    sent_counter = received_counter = 0;
    
    return 0;
}

int main(int argc, char** argv)
{
    int exit_code = EXIT_SUCCESS;
    int eventLoopResult = 0;
    int functionResult = 0;
    sd_event_source *timer_source = nullptr;
    uint64_t now;

    std::cout << "Starting CarNiNe Backend " << PROJECT_VER << std::endl;
    START_EASYLOGGINGPP(argc, argv);

    std::string argv_str(argv[0]);
    std::string base = argv_str.substr(0, argv_str.find_last_of("/"));

    if(utils::FileExists(base + "/loggerBackend.conf")) {
        // Load configuration from file
        el::Configurations conf(base + "/loggerBackend.conf");
        // Now all the loggers will use configuration from file and new loggers
        auto configValue = conf.get(el::Level::Global, el::ConfigurationType::Filename);
        auto fileName = configValue->value();
        if(fileName.at(0) == '.') {
            fileName = base + fileName.substr(1, fileName.length() - 1);
            conf.setGlobally(el::ConfigurationType::Filename, fileName);
        }
        // el::Loggers::setDefaultConfigurations(conf, true);
        std::cout << "Logfile is written at  " << fileName << std::endl;
        el::Loggers::reconfigureAllLoggers(conf);
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

    utils::CommandLineArgs commandLineArgs;
    commandLineArgs.Pharse(argc, argv);

    const auto configFileName = commandLineArgs.GetParamValue("--c");

    if(configFileName.empty()) {
        sd_journal_print(LOG_ERR, "Missing Commanline Parameter Configfile");
        LOG(ERROR) << "Missing Commandline Parameter Configfile";
        std::cout << "Missing Commandline Parameter Configfile" << std::endl;
        exit_code = -6;
        goto finish;
    } else {
        sd_journal_print(LOG_INFO, "Using Config File %s", configFileName.c_str());
        LOG(INFO) << "Using Config File" << configFileName;
        config = new BackendConfig(configFileName);
        config->Load();

        if(!config->GetUdpLogServer().empty()) {
            el::Helpers::installLogDispatchCallback<LogNetDispatcher>("NetworkDispatcher");
            auto dispatcher = el::Helpers::logDispatchCallback<LogNetDispatcher>("NetworkDispatcher");
            dispatcher->setEnabled(true);
            dispatcher->updateServer(config->GetUdpLogServer(), 9090, 8001);
        }
    }

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

    if(sigprocmask(SIG_BLOCK, &ss, NULL) < 0) {
        exit_code = errno;
        goto finish;
    }

    functionResult = sd_event_add_signal(event, NULL, SIGTERM, NULL, NULL);
    if(functionResult < 0) {
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
    if(functionResult < 0) {
        sd_journal_print(LOG_ERR, "Cannot add_signal SIGHUP %d", functionResult);
        exit_code = 204;
        goto finish;
    }
    
    backendController = new BackendController();

    functionResult = backendController->Init();
    if(functionResult != 0) {
        sd_journal_print(LOG_ERR, "backendController Init %d", functionResult);
        exit_code = 205;
        goto finish;
    }

    /* Genrate an Timer for Stats */
    sd_event_now(event, CLOCK_MONOTONIC, &now);
    sd_event_add_time(event, &timer_source, CLOCK_MONOTONIC, now + stats_every_usec, 0, display_stats, NULL);

    functionResult = sd_event_source_set_enabled(timer_source, SD_EVENT_ON);
    if( functionResult < 0) {
        sd_journal_print(LOG_ERR, "Cannot enabled timer %d", functionResult);
    }

    sd_journal_print(LOG_INFO, "Written by M. Nenninger http://www.carnine.de");
    sd_journal_print(LOG_INFO, "Done setting everything up. Serving.");

    functionResult = backendController->Start();
    if(functionResult != 0) {
        sd_journal_print(LOG_ERR, "backendController Start %d", functionResult);
        exit_code = 206;
        goto finish;
    }

    sd_notify(false, "READY=1\n"
                     "STATUS=Up and running.");

    LOG(INFO) << "Up and running.";

    eventLoopResult = sd_event_loop(event);
    if(eventLoopResult < 0) {
        sd_journal_print(LOG_ERR, "Failure: %s\n", strerror(-eventLoopResult));
        exit_code = -eventLoopResult;
    }

    LOG(INFO) << "After loop begin shutdown";

    backendController->Stop();

finish:

    if(backendController != nullptr) {
        delete backendController;
    }

    if(config != nullptr) {
        delete config;
    }

    LOG(INFO) << "Last LogEntry";
    el::Helpers::uninstallPreRollOutCallback();

    return exit_code;
}