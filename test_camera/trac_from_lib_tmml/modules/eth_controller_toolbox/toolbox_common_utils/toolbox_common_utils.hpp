#ifndef PROJECT_COMMON_UTILS
#define PROJECT_COMMON_UTILS

#include <string>
#include "INIReader.h"

namespace toolbox_utils
{
    bool isDoubleEquals(double a, double b);

    namespace ini_reader_tool
    {
        bool Get(
            const INIReader &reader,
            const std::string &section,
            const std::string &fieldName, 
            std::string &dst,
            const std::string defaultValue = "oops");

        bool GetInteger(
            const INIReader &reader,
            const std::string &section,
            const std::string &fieldName,
            int &dst,
            int defaultValue = -1);
            
        bool GetReal(
            const INIReader &reader,
            const std::string &section,
            const std::string &fieldName,
            double &dst,
            double defaultValue = -1);
    } // namespace ini_reader_tool
} // namespace project_utils

#endif //PROJECT_COMMON_UTILS