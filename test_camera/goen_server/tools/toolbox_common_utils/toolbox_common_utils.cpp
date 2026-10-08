#include "toolbox_common_utils.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

namespace toolbox_utils
{
    bool isDoubleEquals(double a, double b)
    {
        return std::fabs(a - b) < __DBL_EPSILON__;
    } // isDoubleEquals

    namespace ini_reader_tool
    {
        std::string getMessageParseIniFieldError(
            const std::string &section,
            const std::string &fieldName)
        {
            std::ostringstream os;
            os << "Error: INIReader parse [" + section + "] : " << fieldName << " error";
            return os.str();
        } // -- END getMessageParseIniFieldError

        bool Get(
            const INIReader &reader,
            const std::string &section,
            const std::string &fieldName, 
            std::string &dst,
            const std::string defaultValue)
        {
            dst = reader.Get(
                section,
                fieldName,
                defaultValue);
            if(dst == defaultValue)
            {
                std::cout << getMessageParseIniFieldError(section, fieldName) << std::endl;
                return false;
            }
            else
            {
                std::cout << "\t " << fieldName << " = " << dst << std::endl;
            }
            return true;
        } // -- END Get

        bool GetInteger(
            const INIReader &reader,
            const std::string &section,
            const std::string &fieldName,
            int &dst,
            int defaultValue)
        {
            dst = reader.GetInteger(
                section,
                fieldName,
                defaultValue);
            if(dst == defaultValue)
            {
                std::cout << getMessageParseIniFieldError(section, fieldName) << std::endl;
                return false;
            }
            else
            {
                std::cout << "\t " << fieldName << " = " << dst << std::endl;
            }
            return true;
        } // -- END GetInteger

        bool GetReal(
            const INIReader &reader,
            const std::string &section,
            const std::string &fieldName,
            double &dst,
            double defaultValue)
        {
            dst = reader.GetReal(
                section, 
                fieldName, 
                defaultValue);
            if(isDoubleEquals(dst, defaultValue))
            {
                std::cout << getMessageParseIniFieldError(section, fieldName) << std::endl;
                return false;
            }
            else
            {
                std::cout << "\t " << fieldName << " = " << dst << std::endl;
            }
            return true;
        } // -- END GetReal
    } // -- namespace ini_reader_tool
} // -- namespace projet_utils