#ifndef CMD_HANDLERS_DISTRIBUTOR_HPP
#define CMD_HANDLERS_DISTRIBUTOR_HPP

#include <map>
#include <vector>
#include <memory>
#include "../commands_zoo/eth_cmd_base.hpp"
#include "../eth_cmd_common_types.hpp"
#include "../icmdhandler.hpp"

namespace eth_cmd_processing
{
    /*
        Объект класса аккумулирует функторы обработчиков команд в согласии с их id;
        Распределение команд по обработчикам (маршрутиризация) производится на уровне application (а не на уровне simplex_receiver'а)
    */
    class CmdHandlersDistributor
    {
        public:
            CmdHandlersDistributor();
            bool apply(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd);

            /*
            Регистрация обработчика команды cmd_id функтором handler (через std::bind)
            */
            void register_cmd_handler(EthCommands cmd_id, 
               const std::function<void(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd)> &handler);
        private:
            std::map<
                EthCommands, //keys
                std::vector<
                    std::function<void(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd)>>> handlers_data; //vals
    };
} // namespace eth_cmd_processing

#endif // CMD_HANDLERS_DISTRIBUTOR_HPP
