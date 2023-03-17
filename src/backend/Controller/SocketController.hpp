#pragma once

#include <thread>
#include <vector>

#include <sys/poll.h>

class SocketController
{
private:
    int _listenSocket;
    std::thread _loopThread;
    bool _run;
    int _internPipe[2];
    std::vector<int> _watchSockets;
    pollfd _fds[7]; //Max clients Listen + 2;
    void Loop();
    void HandleAccept();
    void BuildFds();
public:
    SocketController(int listenSocket);
    ~SocketController();

    int Start();
};

