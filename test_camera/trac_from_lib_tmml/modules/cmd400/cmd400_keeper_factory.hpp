#ifndef CMD400_KEEPER_FACTORY_HPP
#define CMD400_KEEPER_FACTORY_HPP

#include <memory>
#include <string>

#include "cmd400_keeper.hpp"

namespace cmd400_keeper
{
std::shared_ptr<Cmd400Keeper> create(
        const std::string &path2ini,
        const std::string &section);
} // -- namespace cmd400_keeper

#endif //CMD400_KEEPER_FACTORY_HPP
