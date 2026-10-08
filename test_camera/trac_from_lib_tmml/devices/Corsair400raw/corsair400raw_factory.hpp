#ifndef CORSAIR400RAW_FATORY_HPP
#define CORSAIR400RAW_FATORY_HPP

#include "corsair400raw.hpp"

#include <memory>
#include <string>

namespace devices::corsair_400_raw
{
    std::shared_ptr<Corsair400raw> create(
        const std::string &path2ini, const std::string &section);
} // -- namespace devices::corsair_400_raw

#endif // CORSAIR400RAW_FATORY_HPP
