#ifndef CORSAIR_ETHCONTROLLER_H
#define CORSAIR_ETHCONTROLLER_H

#include <stdint.h>
#include <string>

#include <eth/ethcontroller.h>

class Corsair_EthController : public EthController
{
private:
    Corsair_EthController();
public:
    static Corsair_EthController & inst();
    ~Corsair_EthController();
};

#endif // CORSAIR_ETHCONTROLLER_H
