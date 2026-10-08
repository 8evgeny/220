#include "sys_time_extract.hpp"

#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace syst_time_extract
{

    void DateTime::log()
    {
        std::cout << year << "-" << month << "-" << day 
            << " " << hour << ":" << min << ":" << std::fixed << std::setprecision(3) << sec << std::endl;
    } // -- END log

    std::string DateTime::to_str()
    {
        std::ostringstream os;
        os.str("");
        os << year << "-" << month << "-" << day 
            << " " << hour << ":" << min << ":" << std::fixed << std::setprecision(3) << sec;
        return os.str();
    } // -- END to_str

    //ref: https://stackoverflow.com/questions/15957805/extract-year-month-day-etc-from-stdchronotime-point-in-c
    DateTime getCurrentDateTime()
    {
        DateTime dateTime;
        using namespace std;
        using namespace std::chrono;
        const int gmt_time_zone = +3;

        system_clock::time_point now = system_clock::now();
        now += hours(gmt_time_zone);
        time_t tt = system_clock::to_time_t(now);
        tm utc_tm = *gmtime(&tt);
        // tm local_tm = *localtime(&tt);
        dateTime.year = utc_tm.tm_year + 1900;
        dateTime.month = utc_tm.tm_mon + 1;
        dateTime.day = utc_tm.tm_mday;
        dateTime.hour = utc_tm.tm_hour;
        dateTime.min = utc_tm.tm_min;
        dateTime.sec = utc_tm.tm_sec;

        typedef duration<int, ratio_multiply<hours::period, ratio<24> >::type> days;
        system_clock::duration tp = now.time_since_epoch();
        days d = duration_cast<days>(tp);
        tp -= d;
        hours h = duration_cast<hours>(tp);
        tp -= h;
        minutes m = duration_cast<minutes>(tp);
        tp -= m;
        seconds s = duration_cast<seconds>(tp);
        tp -= s;
        milliseconds ms = duration_cast<milliseconds>(tp);
        tp -= ms;

        // dateTime.hour = h.count();
        // dateTime.min = m.count();
        // dateTime.sec = s.count();
        // dateTime.sec = m.count();

        double msec = ms.count() / 1000.f;
        dateTime.sec += msec;
        return dateTime;
    }
}
