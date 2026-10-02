
#include "SetControlTrackingCommand.h"


#include "GyroPlatformProtocol.h"
#include "CommandExecutor.h"

#include <QDebug>




SetControlTrackingCommand::SetControlTrackingCommand(   const float yawMisalignment, const float pitchMisalignment,
                                                        const float yawAccelLimit, const float pitchAccelLimit,
                                                        const uint8_t srcDevice, const uint8_t dstDevice,
                                                        const bool requiresResponse,
                                                        CommandExecutor *const commandExecutor):
    WriteRequestCommand( srcDevice, dstDevice, GOEN_220__GYRO_PLATFORM__CMD__CONTROL_TRACKING, requiresResponse, QByteArray(),
                         commandExecutor)
{
    QByteArray payload                          = QByteArray( (int)(sizeof(ControlTracking)), (char)0x00);
    ControlTracking *const controlTrackingCmd   = (ControlTracking *)payload.data();
    controlTrackingCmd->yawMisalignment         = yawMisalignment;
    controlTrackingCmd->pitchMisalignment       = pitchMisalignment;
    controlTrackingCmd->yawAccelLimit           = yawAccelLimit;
    controlTrackingCmd->pitchAccelLimit         = pitchAccelLimit;

    setPayload( payload);
}


SetControlTrackingCommand::SetControlTrackingCommand(   const QByteArray &headerAndPayload,
                                                        CommandExecutor *const commandExecutor):
    WriteRequestCommand( headerAndPayload, commandExecutor)
{

}


void SetControlTrackingCommand::execute()
{
    if( m_pCommandExecutor == nullptr)
        return;

    if( isValid() == false)
        return;

    if( m_pCommandExecutor->getDeviceId() != getDstDeviceId())
        return;

    const QByteArray &payload                           = getPayload();
    const ControlTracking *const controlTrackingCmd     = (ControlTracking *)payload.constData();

    m_pCommandExecutor->setRequiresResponse( isRequiresResponse());
    m_pCommandExecutor->setControlTracking( controlTrackingCmd->yawMisalignment, controlTrackingCmd->pitchMisalignment,
                                            controlTrackingCmd->yawAccelLimit, controlTrackingCmd->pitchAccelLimit);
}
