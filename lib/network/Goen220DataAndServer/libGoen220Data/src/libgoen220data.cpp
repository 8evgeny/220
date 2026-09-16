#include "libgoen220data.h"

QString Goen220::Data::version()
{
    return LIB_GOEN220_DATA_VERSION;
}

unsigned int Goen220::Data::versionMajor()
{
    return LIB_GOEN220_DATA_VERSION_MAJOR;
}

unsigned int Goen220::Data::versionMinor()
{
    return LIB_GOEN220_DATA_VERSION_MINOR;
}

unsigned int Goen220::Data::versionPatch()
{
    return LIB_GOEN220_DATA_VERSION_PATCH;
}
