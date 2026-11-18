
#include "ResultGetStatusCommand.h"

#include "GyroPlatformProtocol.h"
#include "CommandExecutor.h"

#include <QDebug>

ResultGetStatusCommand::ResultGetStatusCommand( const uint8_t srcDevice, const uint8_t dstDevice,
                                                const uint8_t mode,
                                                const ResultGetStatusCommand::MemsBoardStatus *const memsBoardStatus,
                                                const MotorStatus *const motorYaw,
                                                const MotorStatus *const motorPitch,
                                                CommandExecutor *const commandExecutor):
    ReadResponseCommand( srcDevice, dstDevice, GOEN_220__GYRO_PLATFORM__CMD__STATUS, NoError, QByteArray())
{
    QByteArray payload                  = QByteArray( (int)(sizeof(TelemetryUART)), (char)0x00);

    TelemetryUART *const resultCmd      = (TelemetryUART *)payload.data();
    resultCmd->mode                     = mode;

    resultCmd->mems.mode                = memsBoardStatus->mode;
    resultCmd->mems.errors.errorAllMems = memsBoardStatus->errors.errorAllMems;

    resultCmd->mems.flags.notConfigured.flags       = memsBoardStatus->flags.notConfigured.flags;
    resultCmd->mems.flags.illegalData.flags         = memsBoardStatus->flags.illegalData.flags;
    resultCmd->mems.flags.noResponseOnSpi.flags     = memsBoardStatus->flags.noResponseOnSpi.flags;
    resultCmd->mems.flags.reserve.flags             = memsBoardStatus->flags.reserve.flags;

    resultCmd->mems.axisSwitchMode      = memsBoardStatus->axisSwitchMode;
    resultCmd->mems.memsProcessMode     = memsBoardStatus->memsProcessMode;

    resultCmd->mems.speedYaw            = memsBoardStatus->speedYaw;
    resultCmd->mems.speedPitch          = memsBoardStatus->speedPitch;
    resultCmd->mems.angleYaw            = memsBoardStatus->angleYaw;
    resultCmd->mems.anglePitch          = memsBoardStatus->anglePitch;


    resultCmd->motorYaw.mode            = motorYaw->mode;
    resultCmd->motorYaw.errors.flags    = motorYaw->errors.flags;
    resultCmd->motorYaw.flags.flags     = motorYaw->flags.flags;
    resultCmd->motorYaw.angle           = motorYaw->angle;
    resultCmd->motorYaw.speed           = motorYaw->speed;

    resultCmd->motorPitch.mode          = motorPitch->mode;
    resultCmd->motorPitch.errors.flags  = motorPitch->errors.flags;
    resultCmd->motorPitch.flags.flags   = motorPitch->flags.flags;
    resultCmd->motorPitch.angle         = motorPitch->angle;
    resultCmd->motorPitch.speed         = motorPitch->speed;


    setPayload( payload);
}


ResultGetStatusCommand::ResultGetStatusCommand(     const QByteArray &headerAndPayload,
                                                    CommandExecutor *const commandExecutor):
    ReadResponseCommand( headerAndPayload, commandExecutor)
{

}


void ResultGetStatusCommand::execute()
{
    if( m_pCommandExecutor == nullptr)
        return;

    if( isValid() == false)
        return;

    const QByteArray &payload           = getPayload();
    TelemetryUART *const telemetry      = (TelemetryUART *)payload.constData();

//    qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << telemetry->mems.speedPitch;  //   X

    m_pCommandExecutor->setStatus( telemetry->mode);

    m_pCommandExecutor->setMemsMode( telemetry->mems.mode);
    m_pCommandExecutor->setErrorAllMems( telemetry->mems.errors.errorAllMems);

    m_pCommandExecutor->setMemsNotConfigured( telemetry->mems.flags.notConfigured.flags);
    m_pCommandExecutor->setMemsIllegalData( telemetry->mems.flags.illegalData.flags);
    m_pCommandExecutor->setMemsNoResponseOnSpi( telemetry->mems.flags.noResponseOnSpi.flags);
    m_pCommandExecutor->setMemsReserve( telemetry->mems.flags.reserve.flags);
    m_pCommandExecutor->setMemsAxisSwitchMode( telemetry->mems.axisSwitchMode);
    m_pCommandExecutor->setMemsProcessMode( telemetry->mems.memsProcessMode);

    m_pCommandExecutor->setMemsBoardStatusCommon( telemetry->mems.mode, telemetry->mems.errors.errorAllMems,
                                                  telemetry->mems.flags.notConfigured.flags,
                                                  telemetry->mems.flags.illegalData.flags,
                                                  telemetry->mems.flags.noResponseOnSpi.flags,
                                                  telemetry->mems.flags.reserve.flags,
                                                  telemetry->mems.axisSwitchMode,
                                                  telemetry->mems.memsProcessMode);

    m_pCommandExecutor->setMotorYawMode( telemetry->motorYaw.mode);
    m_pCommandExecutor->setMotorYawErrors( telemetry->motorYaw.errors.flags);
    m_pCommandExecutor->setMotorYawFlags( telemetry->motorYaw.flags.flags);
    m_pCommandExecutor->setMotorYawAngle( telemetry->motorYaw.angle);
    m_pCommandExecutor->setMotorYawSpeed( telemetry->motorYaw.speed);

    m_pCommandExecutor->setYawMotorStatus(	telemetry->motorYaw.mode, telemetry->motorYaw.errors.flags,
                                            telemetry->motorYaw.flags.flags, telemetry->motorYaw.angle,
                                            telemetry->motorYaw.speed);


    m_pCommandExecutor->setMotorPitchMode( telemetry->motorPitch.mode);
    m_pCommandExecutor->setMotorPitchErrors( telemetry->motorPitch.errors.flags);
    m_pCommandExecutor->setMotorPitchFlags( telemetry->motorPitch.flags.flags);
    m_pCommandExecutor->setMotorPitchAngle( telemetry->motorPitch.angle);
    m_pCommandExecutor->setMotorPitchSpeed( telemetry->motorPitch.speed);

    m_pCommandExecutor->setPitchMotorStatus(	telemetry->motorPitch.mode, telemetry->motorPitch.errors.flags,
                                                telemetry->motorPitch.flags.flags, telemetry->motorPitch.angle,
                                                telemetry->motorPitch.speed);


    m_pCommandExecutor->setSpeedYaw( telemetry->mems.speedYaw);
    m_pCommandExecutor->setSpeedPitch( telemetry->mems.speedPitch);
    m_pCommandExecutor->setAngleYaw( telemetry->mems.angleYaw);
    m_pCommandExecutor->setAnglePitch( telemetry->mems.anglePitch);

    m_pCommandExecutor->setMemsBoardStatusSpeedAndAngle( telemetry->mems.speedYaw, telemetry->mems.speedPitch,
                                                         telemetry->mems.angleYaw, telemetry->mems.anglePitch);
}
