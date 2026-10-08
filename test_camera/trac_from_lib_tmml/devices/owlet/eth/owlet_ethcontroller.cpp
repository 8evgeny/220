#include "owlet_ethcontroller.h"

#include "owletserver.h"
#include "owletclient.h"

// Host   UDP port - 52515
// Device UDP port - 52516

Owlet_EthController::Owlet_EthController()
{
    Socket::init_sockets();
    client = new OwletClient();
    server = new OwletServer();
}

Owlet_EthController & Owlet_EthController::inst()
{
    static Owlet_EthController inst;
    return inst;
}

Owlet_EthController::~Owlet_EthController()
{
    delete client;
    delete server;
    Socket::deinit_sockets();
}
