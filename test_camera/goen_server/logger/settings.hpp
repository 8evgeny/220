#ifndef LOGGER_SETTINGS_HPP
#define LOGGER_SETTINGS_HPP

#include <string>
#include <memory>

namespace logger_artem
{
    enum class Lavel : uint8_t
    {
        OFF = 0,
        SIMPLE = 1,
        TIMED = 2
    };

    enum class Destination : uint8_t
    {
        STD_OUT = 0,
        FILE = 1,
        FULL = 2
    };

    struct Settings
    {
        Lavel lavel = Lavel::SIMPLE;
        Destination dest = Destination::STD_OUT;
        int LogSleepMs = 10000;
        int checkQuitSleepMs = 100; // Временной интервал проверки запроса на закрытие

        // Максимально допустимое значение LogCheckTimeDivider
        long double maxLogCheckTimeDivider = 1e4;

        std::string pathToLogFile = "../log/";
        std::string filename = "../log/nan.txt";
        int maxNumberLogFolder = 365;
        struct LogFeatures
        {
            bool currentTime = false;
            bool executionTime = false;
            bool cpuTemperature = false;
            bool gpuTemperature = false;
        } features;
    }; // -- END Settings

    using SettingsPtr = std::unique_ptr<Settings>;
} // -- namespace logger_artem

#endif // LOGGER_SETTINGS_HPP
