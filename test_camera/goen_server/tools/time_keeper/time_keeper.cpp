#include "time_keeper.hpp"

#include <iomanip> // put_time
#include <ctime>   // localtime
#include <sstream> // stringstream

using namespace time_keeper;

std::string time_keeper::time()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%X");
    return ss.str();
}

std::string time_keeper::date()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
    return ss.str();
}

std::string time_keeper::timeAndDate()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

TimeKeeper::TimeKeeper()
{
    start = std::chrono::system_clock::now();
}

TimeKeeper &TimeKeeper::inst()
{
    static TimeKeeper instance;
    return instance;
}

std::string TimeKeeper::executionTime()
{
    auto currentTime = std::chrono::system_clock::now();;

    int seconds = std::chrono::duration_cast<std::chrono::seconds>
            (currentTime - start).count();
    int hours = seconds / 3600;
    seconds -= hours * 3600;
    int minutes = seconds / 60;
    seconds -= minutes * 60;

    std::stringstream ss;
    if(hours < 10)
    {
        ss << "0";
    }
    ss << hours << ":";
    if(minutes < 10)
    {
        ss << "0";
    }
    ss << minutes << ":";
    if(seconds < 10)
    {
        ss << "0";
    }
    ss << seconds;
    return ss.str();
}
