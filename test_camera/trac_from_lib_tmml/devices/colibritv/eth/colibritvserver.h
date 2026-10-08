#ifndef COLIBRITVSERVER_H
#define COLIBRITVSERVER_H

#include <mutex>
#include <iostream>
#include <condition_variable>

#include "eth/server.h"

class ColibriTVServer : public Server
{
public:
    ColibriTVServer();
    ~ColibriTVServer() override;
    void quit() override;

private:
    void handle(const SocketAddress & address, const void * data,
                const int & data_length) override;
private:
    void run_handle_frame();

    int packet_counter = 0;     // счетчик пакетов видеоданных
    std::atomic<bool> handle_frame_execute;
    std::condition_variable frame_cv;


};

#endif // COLIBRITVSERVER_H
