#include "corsair_ethcontroller.h"

#include "corsairserver.h"
#include "corsairclient.h"

// Host   UDP port - 52515
// Device UDP port - 52516

Corsair_EthController::Corsair_EthController()
{
    Socket::init_sockets();
    client = new CorsairClient();
    server = new CorsairServer();
}

Corsair_EthController & Corsair_EthController::inst()
{
    static Corsair_EthController inst;
    return inst;
}

Corsair_EthController::~Corsair_EthController()
{
    delete client;
    delete server;
    Socket::deinit_sockets();
}
