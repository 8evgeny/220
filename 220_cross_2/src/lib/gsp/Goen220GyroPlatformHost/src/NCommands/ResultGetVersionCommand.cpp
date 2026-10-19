
#include "ResultGetVersionCommand.h"

#include "GyroPlatformProtocol.h"
#include "CommandExecutor.h"


ResultGetVersionCommand::ResultGetVersionCommand(   const uint8_t srcDevice, const uint8_t dstDevice,
                                                    const uint32_t versionProtocolVdb, const uint32_t versionProtocol,
                                                    const uint32_t versionDevice,
                                                    CommandExecutor *const commandExecutor):
    ReadResponseCommand( srcDevice, dstDevice, GOEN_220__GYRO_PLATFORM__CMD__VERSION, NoError, QByteArray())
{
    QByteArray payload                  = QByteArray( (int)(sizeof(VersionResultCmd)), (char)0x00);

    VersionResultCmd *const resultCmd   = (VersionResultCmd *)payload.data();
    resultCmd->versionProtocolVdb       = versionProtocolVdb;
    resultCmd->versionProtocol          = versionProtocol;
    resultCmd->versionDevice            = versionDevice;

    setPayload( payload);
}


ResultGetVersionCommand::ResultGetVersionCommand( const QByteArray &headerAndPayload,
                                                  CommandExecutor *const commandExecutor):
    ReadResponseCommand( headerAndPayload, commandExecutor)
{

}


void ResultGetVersionCommand::execute()
{
    if( m_pCommandExecutor == nullptr)
        return;

    if( isValid() == false)
        return;

    const QByteArray &payload           = getPayload();
    VersionResultCmd *const versions    = (VersionResultCmd *)payload.constData();

    m_pCommandExecutor->setVersion( versions->versionProtocolVdb, versions->versionProtocol, versions->versionDevice);
}
