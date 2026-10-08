#ifndef EXTEND_INI_READER_HPP
#define EXTEND_INI_READER_HPP

#include "INIReader.h"
#include <cmath>
#include <iostream>
#include <sstream>

namespace ext_ini
{
	inline bool isDoubleEquals(double a, double b)
	{
		return std::fabs(a - b) < __DBL_EPSILON__;
	}
	
    inline std::string getMessageParseIniFieldError(
        const std::string &section,
        const std::string &fieldName)
    {
        std::ostringstream os;
        os << "Error: INIReader parse [" + section + "] : " << fieldName << " error";
        return os.str();
    } // -- END getMessageParseIniFieldError
        
    inline bool Get(
            const INIReader &reader,
            const std::string &section,
            const std::string &fieldName, 
            std::string &dst,
            const std::string defaultValue = "oops")
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

    inline bool GetInteger(
        const INIReader &reader,
        const std::string &section,
        const std::string &fieldName,
        int &dst,
        int defaultValue = -50500505)
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

    inline bool GetReal(
        const INIReader &reader,
        const std::string &section,
        const std::string &fieldName,
        double &dst,
        double defaultValue = -5050.12345678)
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
} // -- namespace ext_ini

#endif //EXTEND_INI_READER_HPP
