#pragma once

class BackendController
{
private:
    /* data */
public:
    BackendController();
    ~BackendController();

    int Init();
    int Start();
    void Stop();

    void CheckSystem();
};


