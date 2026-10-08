#ifndef TIMEKEEPER_HPP
#define TIMEKEEPER_HPP

#include <string>
#include <chrono>

namespace time_keeper {

    std::string time();
    std::string date();
    std::string timeAndDate();

    class TimeKeeper
    {
    public:
        static TimeKeeper& inst();
        std::string executionTime();
    private:
        TimeKeeper();
        TimeKeeper(const TimeKeeper &root) = delete;
        TimeKeeper(TimeKeeper &&root) = delete;
        TimeKeeper& operator=(TimeKeeper &&) = delete;
        ~TimeKeeper() = default;

        decltype(std::chrono::system_clock::now()) start;
    };

}

#endif // TIMEKEEPER_HPP
