#ifndef LOGGERFACTORY_H
#define LOGGERFACTORY_H

#include "logger.hpp"

namespace logger_artem
{
    LoggerArtem& create(const std::string &pathToSettings, bool &status);
}

#endif // LOGGERFACTORY_H
