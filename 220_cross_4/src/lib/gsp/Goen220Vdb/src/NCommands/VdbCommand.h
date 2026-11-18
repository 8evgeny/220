
#ifndef VDB_COMMAND_H
#define VDB_COMMAND_H


#include "Command.h"

#include <QByteArray>



class VdbCommand : public Command
{
public:
    enum Cmd : uint8_t
    {
         WriteRequest	= 0x00
        ,WriteResponse	= 0x80
        ,ReadRequest	= 0x01
        ,ReadResponse	= 0x81
    };

    enum Error : uint8_t
    {
         NoError 				= 0x00

        ,MessageDeliveryError	= 0x01
        ,FunctionNotExist       = 0x04
        //  ...
    };

    enum RequestType : uint8_t
    {
         RequestWithoutResponse = 0x00
        ,RequestWithResponse    = 0x80
    };

    VdbCommand( const Cmd cmd, const bool requiresResponse,
                const Error error,
                const uint8_t srcDevice, const uint8_t srcFunction,
                const uint8_t dstDevice, const uint8_t dstFunction,
                const QByteArray &payload,
                CommandExecutor *const commandExecutor = nullptr);


    VdbCommand( const QByteArray &headerAndPayload, CommandExecutor *const commandExecutor = nullptr);


    QByteArray serialize() const override;


    bool isValid() const override;


    bool isRequiresResponse() const override;

    uint8_t getErrorCode() override;

    uint8_t getSrcDeviceId() const override;
    uint8_t getSrcFunction() const override;

    uint8_t getDstDeviceId() const override;
    uint8_t getDstFunction() const override;

    bool isRequest() const override;


protected:
    void setPayload( const QByteArray &payload);

    const QByteArray &getPayload() const;


private:
    QByteArray  m_header;
    QByteArray  m_payload;
};


#endif

