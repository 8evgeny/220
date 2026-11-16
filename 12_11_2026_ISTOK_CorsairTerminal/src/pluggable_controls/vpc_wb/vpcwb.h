#pragma once

#include "vpcwb_joystick.h"

namespace VpcWB
{
bool mapFromJsonString(VpcWB::Joystick *joystick, const QString &jsonString);
bool mapFromJsonFileName(VpcWB::Joystick *joystick, const QString &fileNamePath);
};
