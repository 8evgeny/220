#ifndef CMD_VERTELKA_KEEPER_FACTORY_HPP
#define CMD_VERTELKA_KEEPER_FACTORY_HPP

#include <memory>
#include <string>

#include "cmd_vertelka_keeper.hpp"

namespace cmd_vertelka
{
std::shared_ptr<CmdVertelkaKeeper> create(
        const std::string &path2ini,
        const std::string &section);
} // -- namespace cmd_vertelka

#endif //CMD_VERTELKA_KEEPER_FACTORY_HPP
