#ifndef ETH_CMD_FACTORY_HPP
#define ETH_CMD_FACTORY_HPP

#include "eth_cmd_base.hpp"
#include "../eth_cmd_common_types.hpp"

#include <memory>
#include <map>
#include <functional>

namespace eth_cmd_processing
{
    class EthCmdFactory
    {
        public:
            EthCmdFactory();
            std::shared_ptr<EthCmdBase> create(EthCommands cmd_type) const;
        private:
            std::map<EthCommands,
                std::function< std::shared_ptr<EthCmdBase> () > > factory_callbacks_map;
    }; // class EthCmdFactory
} // -- END eth_cmd_processing


#endif // ETH_CMD_FACTORY_HPP