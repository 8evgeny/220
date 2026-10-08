#ifndef COLIBRITV_ETHCONTROLLER_H
#define COLIBRITV_ETHCONTROLLER_H

#include <stdint.h>
#include <string>

#include <eth/ethcontroller.h>

class ColibriTV_EthController : public EthController
{
private:
    ColibriTV_EthController();
public:
    static ColibriTV_EthController & inst();
    ~ColibriTV_EthController();
};

#endif // COLIBRITV_ETHCONTROLLER_H
