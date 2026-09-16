
#include "SetModeCommand.h"


#include "GyroPlatformProtocol.h"
#include "CommandExecutor.h"

#include <QDebug>


SetModeCommand::SetModeCommand( const Mode mode,
                                const uint8_t srcDevice, const uint8_t dstDevice,
                                const bool requiresResponse,
                                CommandExecutor *const commandExecutor):
    WriteRequestCommand( srcDevice, dstDevice, GOEN_220__GYRO_PLATFORM__CMD__MODE, requiresResponse, QByteArray(),
                         commandExecutor)
{
    QByteArray payload      = QByteArray( (int)(sizeof(ModeCmd)), (char)0x00);
    ModeCmd *const modeCmd  = (ModeCmd *)payload.data();
    modeCmd->mode           = mode;

    setPayload( payload);
}


SetModeCommand::SetModeCommand( const QByteArray &headerAndPayload, CommandExecutor *const commandExecutor):
    WriteRequestCommand( headerAndPayload, commandExecutor)
{

}


void SetModeCommand::execute()
{
    if( m_pCommandExecutor == nullptr)
        return;

    if( isValid() == false)
        return;

    if( m_pCommandExecutor->getDeviceId() != getDstDeviceId())
        return;

    const QByteArray &payload   = getPayload();
    ModeCmd *const modeCmd      = (ModeCmd *)payload.constData();

    CommandExecutor::Mode newMode = CommandExecutor::Unknown;
    switch( modeCmd->mode)
    {
    case SetModeCommand::Dummy:             newMode = CommandExecutor::Dummy;            break;
    case SetModeCommand::Stabilization:     newMode = CommandExecutor::Stabilization;    break;
    case SetModeCommand::RotaryPlatform:    newMode = CommandExecutor::RotaryPlatform;   break;
    case SetModeCommand::Parkin:            newMode = CommandExecutor::Parkin;           break;
    case SetModeCommand::Off:               newMode = CommandExecutor::Off;              break;
    default:
        qWarning() << Q_FUNC_INFO << "Unknow mode" << modeCmd->mode;
        return;
    }

    m_pCommandExecutor->setRequiresResponse( isRequiresResponse());
    m_pCommandExecutor->setMode( newMode);
}

