#ifndef SYS_TIME_EXTRACT_HPP
#define SYS_TIME_EXTRACT_HPP

#include <string>

namespace syst_time_extract
{
    struct DateTime
    {
        int year        = 0;
        int month       = 0;
        int day         = 0;
        int hour        = 0;
        int min         = 0;
        double sec      = 0;
        void log();
        std::string to_str();
    };

    DateTime getCurrentDateTime();
} // -- syst_time_extract

#endif //SYS_TIME_EXTRACT_HPP
