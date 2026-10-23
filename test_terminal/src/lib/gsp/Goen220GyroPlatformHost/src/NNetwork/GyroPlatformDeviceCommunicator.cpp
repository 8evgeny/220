
#include "GyroPlatformDeviceCommunicator.h"


#include "GetStatusCommand.h"
#include "DelimiterAndCrc8CommandDecorator.h"

#include "Command.h"

#include <QDebug>






GyroPlatformDeviceCommunicator::GyroPlatformDeviceCommunicator( const uint8_t hostId, const uint8_t deviceId, QObject *parent):
	DeviceCommunicator( hostId, deviceId, parent)
{

}


GyroPlatformDeviceCommunicator::~GyroPlatformDeviceCommunicator()
{

}


Command *GyroPlatformDeviceCommunicator::createGetStatusCommand( const uint8_t hostId, const uint8_t deviceId)
{
    Command *const cmd = new GetStatusCommand( hostId, deviceId);
    Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator( cmd);

    return decoratedCmd;
}


