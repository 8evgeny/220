#include "temperature_monitor.hpp"

#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"

#include <fstream>
#include <string>

static double baseTemperatureReader(const std::string &path)
{
    std::string data;
    double devTemperature = 0;
    std::ifstream tempFile(path);
    if(tempFile.is_open())
    {
        tempFile >> data;
        devTemperature = std::stod(data)/1000;
        tempFile.close();
    }
    return devTemperature;
}

double temperature::cpu()
{
    return(baseTemperatureReader("/sys/devices/virtual/thermal/thermal_zone1/temp"));
}

double temperature::gpu()
{
    return(baseTemperatureReader("/sys/devices/virtual/thermal/thermal_zone2/temp"));
}
