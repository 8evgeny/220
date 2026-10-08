#ifndef VTRACK_DATA_SENDER_FACTORY_HPP
#define VTRACK_DATA_SENDER_FACTORY_HPP

#include "vtrack_data_sender.hpp"
#include "tools/ExtendINIReader.hpp"
#include "tools/toolbox_common_utils/toolbox_common_utils.hpp"

#include <memory>

namespace track_pos_sender
{
std::shared_ptr<vtrackDataSender> create(std::string & path2ini, std::string & section);
}; // END track_pos_sender

#endif // VTRACK_DATA_SENDER_FACTORY_HPP
