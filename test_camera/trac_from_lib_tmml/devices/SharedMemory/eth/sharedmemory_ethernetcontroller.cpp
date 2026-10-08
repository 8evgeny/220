#include "sharedmemory_ethernetcontroller.h"

#include "sharedmemoryserver.hpp"
#include "sharedmemoryclient.hpp"

SharedMemory_EthernetController::SharedMemory_EthernetController()
{
    Socket::init_sockets();
    client = new SharedMemoryClient();
    server = new SharedMemoryServer();
}

SharedMemory_EthernetController &SharedMemory_EthernetController::inst()
{
    static SharedMemory_EthernetController inst;
    return inst;
}

SharedMemory_EthernetController::~SharedMemory_EthernetController()
{
    delete client;
    delete server;
    Socket::deinit_sockets();
}
