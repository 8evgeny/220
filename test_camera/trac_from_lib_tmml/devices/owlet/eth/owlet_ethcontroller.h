#ifndef Owlet_ETHCONTROLLER_H
#define Owlet_ETHCONTROLLER_H

#include <stdint.h>
#include <string>

#include <eth/ethcontroller.h>

class Owlet_EthController : public EthController
{
private:
    Owlet_EthController();
public:
    static Owlet_EthController & inst();
    ~Owlet_EthController();
};

#endif // Owlet_ETHCONTROLLER_H
