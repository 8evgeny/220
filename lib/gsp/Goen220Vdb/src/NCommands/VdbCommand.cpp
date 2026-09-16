
#include "VdbCommand.h"

#include "VdbProtocol.h"


VdbCommand::VdbCommand( const Cmd cmd, const bool requiresResponse,
                        const Error error,
                        const uint8_t srcDevice, const uint8_t srcFunction,
                        const uint8_t dstDevice, const uint8_t dstFunction,
                        const QByteArray &payload,
                        CommandExecutor *const commandExecutor):
    Command( commandExecutor)
{
    m_header    = QByteArray( (int)(sizeof(VdbHeader)), (char)0x00);

    VdbHeader *const header = (VdbHeader *)m_header.data();
    header->cmd             = cmd;
    header->status          = (requiresResponse) ? RequestWithResponse : RequestWithoutResponse;
    if( error != NoError)
        header->status += error;

    header->src.function    = srcFunction;
    header->src.device      = srcDevice;
    header->dst.function    = dstFunction;
    header->dst.device      = dstDevice;

    setPayload( payload);
}


VdbCommand::VdbCommand( const QByteArray &headerAndPayload, CommandExecutor *const commandExecutor):
    Command( commandExecutor)
{
    if( headerAndPayload.size() < sizeof(VdbHeader))
        return;

    m_header = headerAndPayload.mid( 0, sizeof(VdbHeader));


    if( headerAndPayload.size() > sizeof(VdbHeader))
    {
        m_payload = headerAndPayload.mid( sizeof(VdbHeader));
    }
}


QByteArray VdbCommand::serialize() const
{
    return (m_header + m_payload);
}


bool VdbCommand::isValid() const
{
    if( m_header.size() < sizeof(VdbHeader))
        return false;

    const VdbHeader *const header = (VdbHeader *)m_header.constData();
    if( header->cmd != WriteRequest && header->cmd != ReadRequest &&
        header->cmd != WriteResponse && header->cmd != ReadResponse)
    {
        return false;
    }



    return true;
}


bool VdbCommand::isRequiresResponse() const
{
    if( m_header.size() < sizeof(VdbHeader))
        return false;

    const VdbHeader *const header = (VdbHeader *)m_header.constData();
    return (header->status == RequestWithResponse);
}


uint8_t VdbCommand::getErrorCode()
{
    if( m_header.size() < sizeof(VdbHeader))
        return NoError;

    const VdbHeader *const header = (VdbHeader *)m_header.constData();
    if( header->status >= RequestWithResponse)
        return (header->status - RequestWithResponse);

    return header->status;
}


uint8_t VdbCommand::getSrcDeviceId() const
{
    if( m_header.size() < sizeof(VdbHeader))
        return 0;

    const VdbHeader *const header = (VdbHeader *)m_header.constData();
    return header->src.device;
}


uint8_t VdbCommand::getSrcFunction() const
{
    if( m_header.size() < sizeof(VdbHeader))
        return 0;

    const VdbHeader *const header = (VdbHeader *)m_header.constData();
    return header->src.function;
}


uint8_t VdbCommand::getDstDeviceId() const
{
    if( m_header.size() < sizeof(VdbHeader))
        return 0;

    const VdbHeader *const header = (VdbHeader *)m_header.constData();
    return header->dst.device;
}


uint8_t VdbCommand::getDstFunction() const
{
    if( m_header.size() < sizeof(VdbHeader))
        return 0;

    const VdbHeader *const header = (VdbHeader *)m_header.constData();
    return header->dst.function;
}


bool VdbCommand::isRequest() const
{
    if( m_header.size() < sizeof(VdbHeader))
        return false;

    const VdbHeader *const header = (VdbHeader *)m_header.constData();

    return (header->cmd == WriteRequest || header->cmd == ReadRequest);
}


void VdbCommand::setPayload( const QByteArray &payload)
{
    m_payload               = payload;

    if( m_header.size() < sizeof(VdbHeader))
        return;

    VdbHeader *const header = (VdbHeader *)m_header.data();
    header->byteCount       = (uint16_t)payload.size();
}

const QByteArray &VdbCommand::getPayload() const
{
    return m_payload;
}
