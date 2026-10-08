#include "colibritv_ethcontroller.h"

#include "colibritvserver.h"
#include "colibritvclient.h"

ColibriTV_EthController::ColibriTV_EthController()
{
    Socket::init_sockets();
    client = new ColibriTVClient();
    server = new ColibriTVServer();
}

ColibriTV_EthController &ColibriTV_EthController::inst()
{
    static ColibriTV_EthController inst;
    return inst;
}

ColibriTV_EthController::~ColibriTV_EthController()
{
    delete client;
    delete server;
    Socket::deinit_sockets();
}
