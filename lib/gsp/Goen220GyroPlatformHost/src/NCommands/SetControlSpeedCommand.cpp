
#include "SetControlSpeedCommand.h"


#include "GyroPlatformProtocol.h"
#include "CommandExecutor.h"

#include <QDebug>




SetControlSpeedCommand::SetControlSpeedCommand(     const float yaw, const float pitch,
                                                    const uint8_t srcDevice, const uint8_t dstDevice,
                                                    const bool requiresResponse,
                                                    CommandExecutor *const commandExecutor):
    WriteRequestCommand( srcDevice, dstDevice, GOEN_220__GYRO_PLATFORM__CMD__CONTROL_SPEED, requiresResponse, QByteArray(),
                         commandExecutor)
{
    QByteArray payload                          = QByteArray( (int)(sizeof(ControlSpeed)), (char)0x00);
    ControlSpeed *const controlSpeedCmd         = (ControlSpeed *)payload.data();
    controlSpeedCmd->yaw                        = yaw;
    controlSpeedCmd->pitch                      = pitch;

    setPayload( payload);
}


SetControlSpeedCommand::SetControlSpeedCommand( const QByteArray &headerAndPayload,
                                                CommandExecutor *const commandExecutor):
    WriteRequestCommand( headerAndPayload, commandExecutor)
{

}


void SetControlSpeedCommand::execute()
{
    if( m_pCommandExecutor == nullptr)
        return;

    if( isValid() == false)
        return;

    if( m_pCommandExecutor->getDeviceId() != getDstDeviceId())
        return;

    const QByteArray &payload                           = getPayload();
    const ControlSpeed *const controlSpeedCmd           = (ControlSpeed *)payload.constData();

    m_pCommandExecutor->setRequiresResponse( isRequiresResponse());
    m_pCommandExecutor->setControlSpeed( controlSpeedCmd->yaw, controlSpeedCmd->pitch);
}
