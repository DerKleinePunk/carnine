/*visit: http://www.teuniz.net/RS-232/*/

#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "MNSerial"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include <errno.h> // Error number definitions
#include <fcntl.h> // File control definitions
#include <signal.h>
#include <sys/epoll.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <termios.h> // POSIX terminal control definitions
#include <unistd.h> // UNIX standard function definitions
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

#include "../../../modules/SDL2GuiHelper/common/easylogging/easylogging++.h"
#include "../../../modules/SDL2GuiHelper/common/json/json.hpp"
#include "../../../modules/SDL2GuiHelper/common/utils/commonutils.h"
#include "MNSerial.h"

std::mutex m;
std::condition_variable cv;

#define MAXEVENTS 10

void MNSerial::SetSpeed(speed_t speed)
{
    struct termios Opt;
    tcgetattr(_handle, &Opt);
    cfsetispeed(&Opt, speed);
    cfsetospeed(&Opt, speed);
    tcsetattr(_handle, TCSANOW, &Opt);
}

void MNSerial::SetParity()
{
    struct termios options;
    tcgetattr(_handle, &options);
    /*
    options.c_cflag    |= (CLOCAL | CREAD) ;
    options.c_cflag    &= ~PARENB ;
    options.c_cflag    &= ~CSTOPB ;
    options.c_cflag    &= ~CSIZE ;
    options.c_cflag    |= CS8 ;
    options.c_lflag    &= ~(ICANON | ECHO | ECHOE | ISIG );  //Input
    options.c_oflag    &= ~OPOST;   //Output
    options.c_iflag    &= ~(INLCR | IGNCR | ICRNL ); // do nothing with cr or lf
    */

    cfmakeraw(&options);
    options.c_cflag |= (CLOCAL | CREAD); // Enable the receiver and set local mode
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB; // 1 stop bit
    options.c_cflag &= ~CRTSCTS; // Disable hardware flow control
    options.c_cc[VTIME] = 0; /* inter-character timer unused */
    options.c_cc[VMIN] = 1;

    tcsetattr(_handle, TCSANOW, &options);
}

void MNSerial::Loop()
{
    el::Helpers::setThreadName("MNSerial Reader");

    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // required to make flush work, for some reason
    tcflush(_handle, TCIOFLUSH);
    cv.notify_one();
    while(_run) {
        auto result = epoll_wait(_efd, _epoll_events, MAXEVENTS, 500);
        if(result > 0) {
            int bytes;
            ioctl(_handle, FIONREAD, &bytes);
            if(currentReadPos_ + bytes > MAXBUFFERSIZE) {
                LOG(ERROR) << "serial buffer over run";
                bytes = MAXBUFFERSIZE;
                currentReadPos_ = 0;
            }
            auto count = read(_handle, &readBuffer_[currentReadPos_], bytes);
            try {
                if(count > 0) {
                    if(!_rawLogFileName.empty()) {
                        if(!_rawLogFile.is_open()) {
                            _rawLogFile.open(_rawLogFileName, std::ios::out | std::ios::app | std::ios::ate);
                        }
                        // Todo Handle not Printable Char
                        std::string buffer(std::begin(readBuffer_), std::end(readBuffer_));
                        _rawLogFile << utils::dateTimeToString() << " -> "
                                    << utils::GetPrintAbleString(buffer.substr(currentReadPos_, count))
                                    << std::endl;
                        _rawLogFile << std::flush;
                    }
                    AnalyseBuffer(count);
                } else {
                    LOG(DEBUG) << "0 bytes Read";
                }
            } catch(const std::exception& e) {
                LOG(ERROR) << e.what();
            }
        } else if(result == 0) {
            // Timeout
        } else {
            // Error
            LOG(ERROR) << "epoll_wait failed with " << strerror(errno);
        }
    }
    LOG(DEBUG) << "Thread is leaving the loop";
}

MNSerial::MNSerial(const std::string& portname) : _portname(portname)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _handle = -1;
    _run = true;
    currentReadPos_ = 0;
    _epoll_events = nullptr;
    _efd = -1;
    pipefdwrite_ = -1;
    memset(readBuffer_, 0, MAXBUFFERSIZE);
    _rawLogFileName = "";
}

MNSerial::~MNSerial()
{
    if(_rawLogFile.is_open()) {
        _rawLogFile.close();
    }
}

bool MNSerial::Open(const std::string& speed)
{
    LOG(DEBUG) << "Try Open with " << speed;
    speed_t speedInt = B1200;
    if(speed == "1200") {
        speedInt = B1200;
    } else if(speed == "4800") {
        speedInt = B4800;
    } else if(speed == "9600") {
        speedInt = B9600;
    } else if(speed == "19200") {
        speedInt = B19200;
    } else if(speed == "38400") {
        speedInt = B38400;
    } else if(speed == "57600") {
        speedInt = B57600;
    } else if(speed == "115200") {
        speedInt = B115200;
    } else {
        LOG(ERROR) << "Unkown Baud Using 1200 it was " << speed;
    }

    return Open(speedInt);
}

bool MNSerial::Open(speed_t speed)
{
    _run = true;
    LOG(DEBUG) << "Try open " << _portname;
    _handle = open(_portname.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(_handle == -1) {
        LOG(ERROR) << _portname << " open failed with " << strerror(errno);
        return false;
    }

    /* lock access so that another process can't also use the port */
    if(flock(_handle, LOCK_EX | LOCK_NB) != 0) {
        close(_handle);
        LOG(ERROR) << "Another process has locked the comport.";
        _handle = -1;
        return false;
    }

    tcgetattr(_handle, &_oldtio); /* save current port settings */
    SetSpeed(speed);
    SetParity();

    return true;
}

int MNSerial::GetHandle() const
{
    return _handle;
}

int MNSerial::Read()
{
    char buff[512];
    auto length = read(_handle, buff, sizeof(buff));

    LOG(DEBUG) << std::to_string(length) << " Bytes Get";

    return length;
}

int MNSerial::Write(const std::string& message)
{
    auto length = write(_handle, message.c_str(), message.size());
    if(VLOG_IS_ON(1)) {
        VLOG(1) << "Write " << message << " with length " << std::to_string(length);
    }
    if(length != (int)message.size()) {
        LOG(ERROR) << "writing error " << std::to_string(length) << " written message.size was "
                   << std::to_string(message.size());
    } else {
        if(!_rawLogFileName.empty()) {
            if(!_rawLogFile.is_open()) {
                _rawLogFile.open(_rawLogFileName, std::ios::out | std::ios::app | std::ios::ate);
            }
            _rawLogFile << utils::dateTimeToString() << " <- " << utils::GetPrintAbleString(message)
                        << std::endl;
            _rawLogFile << std::flush;
        }
    }
    return length;
}

int MNSerial::Start()
{
    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));

    _efd = epoll_create1(EPOLL_CLOEXEC);
    event.data.fd = _handle;
    event.events = EPOLLIN; // | EPOLLET;
    auto result = epoll_ctl(_efd, EPOLL_CTL_ADD, _handle, &event);
    if(result < 0) {
        LOG(ERROR) << "epoll_ctl failed with " << strerror(errno);
        return result;
    }
    _epoll_events = new epoll_event[MAXEVENTS];

    std::unique_lock<std::mutex> lk(m);
    loop_thread_ = std::thread(&MNSerial::Loop, this);
    cv.wait(lk);
    LOG(DEBUG) << "Serial is open and Thread Running";
    return 0;
}

void MNSerial::Close()
{
    if(_handle == -1) return;

    _run = false;
    if(loop_thread_.joinable()) {
        loop_thread_.join();
    }

    tcsetattr(_handle, TCSANOW, &_oldtio);
    flock(_handle, LOCK_UN);
    close(_handle);

    delete[] _epoll_events;
    _handle = -1;
}

void MNSerial::SetRawLogFile(const std::string& name)
{
    _rawLogFileName = name;
}
