
#include "SetControlPositionCommand.h"


#include "GyroPlatformProtocol.h"
#include "CommandExecutor.h"

#include <QDebug>




SetControlPositionCommand::SetControlPositionCommand(   const float yaw, const float pitch,
                                                        const uint8_t srcDevice, const uint8_t dstDevice,
                                                        const bool requiresResponse,
                                                        CommandExecutor *const commandExecutor):
    WriteRequestCommand( srcDevice, dstDevice, GOEN_220__GYRO_PLATFORM__CMD__CONTROL_POSITION, requiresResponse, QByteArray(),
                         commandExecutor)
{
    QByteArray payload                          = QByteArray( (int)(sizeof(ControlPosition)), (char)0x00);
    ControlPosition *const controlPositionCmd   = (ControlPosition *)payload.data();
    controlPositionCmd->yaw                     = yaw;
    controlPositionCmd->pitch                   = pitch;

    setPayload( payload);
}


SetControlPositionCommand::SetControlPositionCommand( const QByteArray &headerAndPayload,
                                                      CommandExecutor *const commandExecutor):
    WriteRequestCommand( headerAndPayload, commandExecutor)
{

}


void SetControlPositionCommand::execute()
{
    if( m_pCommandExecutor == nullptr)
        return;

    if( isValid() == false)
        return;

    if( m_pCommandExecutor->getDeviceId() != getDstDeviceId())
        return;

    const QByteArray &payload                           = getPayload();
    const ControlPosition *const controlPositionCmd     = (ControlPosition *)payload.constData();

    m_pCommandExecutor->setRequiresResponse( isRequiresResponse());
    m_pCommandExecutor->setControlPosition( controlPositionCmd->yaw, controlPositionCmd->pitch);
}
