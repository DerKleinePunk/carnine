#pragma once

#include <termios.h>

#include <fstream>
#include <string>
#include <thread>

#define MAXBUFFERSIZE 2048

class BaseSerial
{
    int _handle;
    std::string _portname;
    termios _oldtio;
    termios _newtio;
    struct epoll_event* _epoll_events;
    int _efd;
    std::thread loop_thread_;
    bool _run;
    std::ofstream _rawLogFile;
    std::string _rawLogFileName;
    void SetSpeed(speed_t speed);
    void SetParity();
    void Loop();

  protected:
    int pipefdwrite_;
    int currentReadPos_;
    char readBuffer_[MAXBUFFERSIZE];
    virtual void AnalyseBuffer(int count) = 0;

  public:
    BaseSerial(const std::string& portname);
    virtual ~BaseSerial();

    bool Open(const std::string& speed);
    bool Open(speed_t speed);
    int GetHandle() const;
    int Read();
    int Write(const std::string& message);
    int Start();
    void Close();
    void SetRawLogFile(const std::string& name);
};
