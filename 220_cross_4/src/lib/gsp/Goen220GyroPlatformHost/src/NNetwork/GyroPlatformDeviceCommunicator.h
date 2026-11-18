
#ifndef GYRO_PLATFORM_DEVICE_COMMUNICATOR_H
#define GYRO_PLATFORM_DEVICE_COMMUNICATOR_H

#include "DeviceCommunicator.h"


class Q_DECL_EXPORT GyroPlatformDeviceCommunicator : public DeviceCommunicator
{
    Q_OBJECT

public:
    explicit GyroPlatformDeviceCommunicator( const uint8_t hostId, const uint8_t deviceId, QObject *parent = nullptr);
    ~GyroPlatformDeviceCommunicator();

protected:
    Command *createGetStatusCommand( const uint8_t hostId, const uint8_t deviceId) override;

};



#endif

