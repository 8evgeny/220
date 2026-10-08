#ifndef SHAREDMEMORY_ETHERNETCONTROLLER_H
#define SHAREDMEMORY_ETHERNETCONTROLLER_H

#include "eth/ethcontroller.h"

class SharedMemory_EthernetController : public EthController
{
private:
    SharedMemory_EthernetController();
public:
    static SharedMemory_EthernetController & inst();
    ~SharedMemory_EthernetController();
};

#endif // SHAREDMEMORY_ETHERNETCONTROLLER_H
