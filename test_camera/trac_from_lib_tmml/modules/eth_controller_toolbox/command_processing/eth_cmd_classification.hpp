#ifndef ETH_CMD_CLASSIFICATION_HPP
#define ETH_CMD_CLASSIFICATION_HPP

#include <memory>
#include <vector>
#include <array>
#include <functional>
#include <map>

#include "commands_zoo/eth_cmd_base.hpp"
#include "eth_cmd_common_types.hpp"
#include "commands_zoo/eth_cmd_factory.hpp"

namespace eth_cmd_processing
{
    class EthCmdDemux
    {
        public:
            EthCmdDemux();
            /*
                EthCmdDemux выполняет перечень задач:
                    1) предварительный парсинг байт-буфера пришедшего пакета;
                    2) интерпретация пакета как команды и классификация по ее типу;
                    3) делегирование задачи интерпретации байт-буфера
                        пакета как команды десериализатору определенного типа команд
                        (deserialize переопределяется каждым классом eth команды);
                    4) возврат развернутой команды через умный указатель;
            */

            bool eth_cmd_demux_to_handler(uint8_t* cmd_buffer, std::shared_ptr<EthCmdBase> &command) const;
        private:
            EthCmdFactory eth_cmd_factory;
    };
}

#endif //ETH_CMD_CLASSIFICATION_HPP