#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <chrono>

class WatchDog
{
public:
    WatchDog(int msec) : maxSec(msec * 1e-3)
    {
        start = std::chrono::high_resolution_clock::now();
    }

    bool check()
    {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> dt = (now - start);
        if(dt.count() > maxSec)
            return false;
        return true;
    }

    void reset()
    {
        start = std::chrono::high_resolution_clock::now();
    }

private:
    std::chrono::time_point<std::chrono::system_clock> start;
    int maxSec = 0;
};

#endif // WATCHDOG_H
