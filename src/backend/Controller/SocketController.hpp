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
    int _backendPipe;

    void Loop();
    void HandleAccept();
    void BuildFds();
    void HandleReceived(int socket);
    void HandleClosed(int socket);
public:
    SocketController(int listenSocket, int backendPipe);
    ~SocketController();

    int Start();
};

