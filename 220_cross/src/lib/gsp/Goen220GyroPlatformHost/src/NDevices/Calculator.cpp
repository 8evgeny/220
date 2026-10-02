
#include "Calculator.h"


#include "SetModeCommand.h"
#include "DelimiterAndCrc8CommandDecorator.h"
#include "SetControlPositionCommand.h"
#include "SetControlSpeedCommand.h"
#include "SetControlTrackingCommand.h"



#include <QDebug>


Calculator::Calculator( const uint8_t hostId, QObject *parent):
    CommandExecutor( parent)
  , m_hostId( hostId)
  , m_deviceId( 0x01)
  , m_mode( Unknown)
  , m_memsMode( 0x00)
  , m_memsErrorAll( false)
  , m_memsNotConfiguredFlags( 0x00)
  , m_memsIllegalDataFlags( 0x00)
  , m_memsNoResponseOnSpiFlags( 0x00)
  , m_memsReserveFlags( 0x00)
  , m_memsAxisSwitchMode( 0x00)
  , m_memsProcessMode( 0x00)
  , m_speedYaw( 0.0f)
  , m_speedPitch( 0.0f)
  , m_angleYaw( 0.0f)
  , m_anglePitch( 0.0f)
  , m_motorYawMode( 0x00)
  , m_motorYawErrors( 0x00)
  , m_motorYawFlags( 0x00)
  , m_motorYawAngle( 0.0f)
  , m_motorYawSpeed( 0.0f)
  , m_motorPitchMode( 0x00)
  , m_motorPitchErrors( 0x00)
  , m_motorPitchFlags( 0x00)
  , m_motorPitchAngle( 0.0f)
  , m_motorPitchSpeed( 0.0f)
  , m_unknownCommands( 0)
  , m_statusWithErrorCommands( 0)
{

}


uint8_t Calculator::getHostId() const
{
    return m_hostId;
}


void Calculator::setHostId( const uint8_t hostId)
{
    m_hostId = hostId;
}


uint8_t Calculator::getDeviceId() const
{
    return m_deviceId;
}


void Calculator::setDeviceId( const uint8_t deviceId)
{
    m_deviceId = deviceId;
}


void Calculator::setMode( const Mode mode)
{
    SetModeCommand::Mode cmdMode = SetModeCommand::Unknown;
    switch( mode)
    {
    case Dummy:             cmdMode = SetModeCommand::Dummy;            break;
    case Stabilization:     cmdMode = SetModeCommand::Stabilization;    break;
    case RotaryPlatform:    cmdMode = SetModeCommand::RotaryPlatform;   break;
    case Parkin:            cmdMode = SetModeCommand::Parkin;           break;
    case Off:               cmdMode = SetModeCommand::Off;              break;
    default:                                                            break;
    }

    if( cmdMode == SetModeCommand::Unknown)
        return;

    // Command *const cmd = new SetModeCommand( cmdMode, m_hostId, m_deviceId, true);
    Command *const cmd = new SetModeCommand( cmdMode, m_hostId, m_deviceId, false);
    Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator( cmd);

    if( cmdMode == SetModeCommand::Off)
        qDebug() << Q_FUNC_INFO << "OFF" << decoratedCmd->serialize().toHex( ' ');

    if( cmdMode == SetModeCommand::Stabilization)
        qDebug() << Q_FUNC_INFO << "Stabilization" << decoratedCmd->serialize().toHex( ' ');

    if( cmdMode == SetModeCommand::RotaryPlatform)
        qDebug() << Q_FUNC_INFO << "RotaryPlatform" << decoratedCmd->serialize().toHex( ' ');

    if( cmdMode == SetModeCommand::Parkin)
        qDebug() << Q_FUNC_INFO << "Parkin" << decoratedCmd->serialize().toHex( ' ');

    emit sendCmd( decoratedCmd);
}


void Calculator::getVersion()
{

}


void Calculator::getStatus()
{

}


void Calculator::setStatus( const uint8_t mode)
{
    Mode newMode = Unknown;
    switch( mode)
    {
    case SetModeCommand::Dummy:             newMode = Dummy;            break;
    case SetModeCommand::Stabilization:     newMode = Stabilization;    break;
    case SetModeCommand::RotaryPlatform:    newMode = RotaryPlatform;   break;
    case SetModeCommand::Parkin:            newMode = Parkin;           break;
    case SetModeCommand::Off:               newMode = Off;              break;
    default:
        qWarning() << Q_FUNC_INFO << "Unknow mode" << mode;
        return;
    }

    if( m_mode == newMode)
        return;

    m_mode = newMode;
    emit modeChanged( newMode);
}


void Calculator::setVersion( const uint32_t versionProtocolVdb, const uint32_t versionProtocol,
                             const uint32_t versionDevice)
{
//    qDebug() << Q_FUNC_INFO << versionProtocolVdb << versionProtocol << versionDevice;

    emit versionChanged( versionProtocolVdb, versionProtocol, versionDevice);
}


void Calculator::setMemsMode( const uint8_t mode)
{
    emit memsModeChanged( mode);
}


void Calculator::setErrorAllMems( const bool error)
{
    emit errorAllMemsChanged( error);
}


void Calculator::setMemsNotConfigured( const uint8_t flags)
{
//    if( m_memsNotConfiguredFlags == flags)
//        return;

//    m_memsNotConfiguredFlags = flags;

    emit memsNotConfiguredChanged( flags);
}


void Calculator::setMemsIllegalData( const uint8_t flags)
{
//    if( m_memsIllegalDataFlags == flags)
//        return;

//    m_memsIllegalDataFlags = flags;

    emit  memsIllegalDataChanged( flags);
}


void Calculator::setMemsNoResponseOnSpi( const uint8_t flags)
{
//    if( m_memsNoResponseOnSpiFlags == flags)
//        return;

//    m_memsNoResponseOnSpiFlags = flags;

    emit  memsNoResponseOnSpiChanged( flags);
}

void Calculator::setMemsReserve( const uint8_t flags)
{
//    if( m_memsReserveFlags == flags)
//        return;

//    m_memsReserveFlags = flags;

    emit  memsReserveChanged( flags);
}


void Calculator::setMemsAxisSwitchMode( const uint8_t mode)
{
    emit memsAxisSwitchModeChanged( mode);
}


void Calculator::setMemsProcessMode( const uint8_t mode)
{
    emit memsProcessModeChanged( mode);
}


void Calculator::setMemsBoardStatusCommon( const uint8_t mode, const bool errorAllMems,
                                           const uint8_t memsNotConfiguredFlags,
                                           const uint8_t memsIllegalDataFlags,
                                           const uint8_t memsNoResponseOnSpiFlags,
                                           const uint8_t reserveFlags,
                                           const uint8_t memsAxisSwitchMode,
                                           const uint8_t memsProcessMode)
{
    bool changed = false;

    if( m_memsMode != mode)             {   changed = true;     m_memsMode = mode;                  }
    if( m_memsErrorAll != errorAllMems) {   changed = true;     m_memsErrorAll = errorAllMems;      }

    if( m_memsNotConfiguredFlags != memsNotConfiguredFlags)
    {
        changed = true;
        m_memsNotConfiguredFlags = memsNotConfiguredFlags;
    }

    if( m_memsIllegalDataFlags != memsIllegalDataFlags)
    {
        changed = true;
        m_memsIllegalDataFlags = memsIllegalDataFlags;
    }

    if( m_memsNoResponseOnSpiFlags != memsNoResponseOnSpiFlags)
    {
        changed = true;
        m_memsNoResponseOnSpiFlags = memsNoResponseOnSpiFlags;
    }

    if( m_memsReserveFlags != reserveFlags)     {   changed = true;     m_memsReserveFlags = reserveFlags;      }
    if( m_memsAxisSwitchMode != memsAxisSwitchMode)
    {
        changed = true;
        m_memsAxisSwitchMode = memsAxisSwitchMode;
    }

    if( m_memsProcessMode != memsProcessMode)   {   changed = true;     m_memsProcessMode = memsProcessMode;    }

    if( changed)
    {
        emit memsBoardStatusCommonChanged( mode, errorAllMems, memsNotConfiguredFlags, memsIllegalDataFlags,
                                           memsNoResponseOnSpiFlags, reserveFlags,
                                           memsAxisSwitchMode, memsProcessMode);
    }
}


void Calculator::setMemsBoardStatusSpeedAndAngle( const float speedYaw, const float speedPitch,
                                                  const float angleYaw, const float anglePitch)
{
    if( qFuzzyCompare( m_speedYaw, speedYaw) && qFuzzyCompare( m_speedPitch, speedPitch) &&
        qFuzzyCompare( m_angleYaw, angleYaw) && qFuzzyCompare( m_anglePitch, anglePitch))
    {
        return;
    }

    m_speedYaw = speedYaw;
    m_speedPitch = speedPitch;
    m_angleYaw = angleYaw;
    m_anglePitch = anglePitch;

    emit memsBoardStatusSpeedAndAngleChanged( speedYaw, speedPitch, angleYaw, anglePitch);
}


void Calculator::setMotorYawMode( const uint8_t mode)
{
    emit motorYawModeChanged( mode);
}


void Calculator::setMotorYawErrors( const uint8_t flags)
{
    // if( m_motorYawErrors == flags)
    //     return;

    // m_motorYawErrors = flags;
    emit motorYawErrorsChanged( flags);
}


void Calculator::setMotorYawFlags( const uint8_t flags)
{
    // if( m_motorYawFlags == flags)
    //     return;

    // m_motorYawFlags = flags;
    emit motorYawFlagsChanged( flags);
}


void Calculator::setMotorYawAngle( const float angle)
{
    emit motorYawAngleChanged( angle);
}


void Calculator::setMotorYawSpeed( const float speed)
{
    emit motorYawSpeedChanged( speed);
}


void Calculator::setYawMotorStatus( const uint8_t mode, const uint8_t errors, const uint8_t flags,
                                    const float angle, const float speed)
{
    if( m_motorYawMode == mode && m_motorYawErrors == errors && m_motorYawFlags == flags &&
        qFuzzyCompare( m_motorYawAngle, angle) && qFuzzyCompare( m_motorYawSpeed, speed))
    {
        return;
    }

    m_motorYawMode = mode;
    m_motorYawErrors = errors;
    m_motorYawFlags = flags;
    m_motorYawAngle = angle;
    m_motorYawSpeed = speed;

    // qDebug() << Q_FUNC_INFO << mode << errors << flags << angle << speed;
    emit yawMotorStatusChanged( mode, errors, flags, angle, speed);
}


void Calculator::setMotorPitchMode( const uint8_t mode)
{
    emit motorPitchModeChanged( mode);
}


void Calculator::setMotorPitchErrors( const uint8_t flags)
{
    // if( m_motorPitchErrors == flags)
    //     return;

    // m_motorPitchErrors = flags;
    emit motorPitchErrorsChanged( flags);
}


void Calculator::setMotorPitchFlags( const uint8_t flags)
{
    // if( m_motorPitchFlags == flags)
    //     return;

    // m_motorPitchFlags = flags;
    emit motorPitchFlagsChanged( flags);
}


void Calculator::setMotorPitchAngle( const float angle)
{
    emit motorPitchAngleChanged( angle);
}


void Calculator::setMotorPitchSpeed( const float speed)
{
    emit motorPitchSpeedChanged( speed);
}


void Calculator::setPitchMotorStatus( 	const uint8_t mode, const uint8_t errors, const uint8_t flags,
                                        const float angle, const float speed)
{
    if( m_motorPitchMode == mode && m_motorPitchErrors == errors && m_motorPitchFlags == flags &&
        qFuzzyCompare( m_motorPitchAngle, angle) && qFuzzyCompare( m_motorPitchSpeed, speed))
    {
        return;
    }

    m_motorPitchMode = mode;
    m_motorPitchErrors = errors;
    m_motorPitchFlags = flags;
    m_motorPitchAngle = angle;
    m_motorPitchSpeed = speed;

    // qDebug() << Q_FUNC_INFO << mode << errors << flags << angle << speed;
    emit pitchMotorStatusChanged( mode, errors, flags, angle, speed);

}


//void Calculator::setMemsNotConfigured( const uint32_t memsNumber, const bool value)
//{

//}


void Calculator::setSpeedYaw( const float speed)
{
    emit speedYawChanged( speed);
}


void Calculator::setSpeedPitch( const float speed)
{
    emit speedPitchChanged( speed);
}


void Calculator::setAngleYaw( const float angle)
{
    emit angleYawChanged( angle);
}


void Calculator::setAnglePitch( const float angle)
{
    emit anglePitchChanged( angle);
}


void Calculator::setControlPosition( const float yaw, const float pitch)
{
    Command *const cmd = new SetControlPositionCommand( yaw, pitch,
                                                        m_hostId, m_deviceId, false);
    Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator( cmd);
    // qDebug()    << "setControlPosition" << yaw << pitch
    //             << decoratedCmd->serialize().size()
    //             << decoratedCmd->serialize().toHex( ' ');

    emit sendCmd( decoratedCmd);
}


void Calculator::setControlSpeed( const float yaw, const float pitch)
{
    Command *const cmd = new SetControlSpeedCommand( yaw, pitch,
                                                     m_hostId, m_deviceId, false);
    Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator( cmd);

    emit sendCmd( decoratedCmd);
}


void Calculator::setControlTracking( const float yawMisalignment, const float pitchMisalignment,
                                     const float yawAccelLimit, const float pitchAccelLimit)
{
    Command *const cmd = new SetControlTrackingCommand( yawMisalignment, pitchMisalignment,
                                                        yawAccelLimit, pitchAccelLimit,
                                                        m_hostId, m_deviceId, false);
    Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator( cmd);

    emit sendCmd( decoratedCmd);

}


void Calculator::setUnknownCommand( const uint8_t dstFunction)
{
    Q_UNUSED( dstFunction);

    m_unknownCommands ++;
    if( m_unknownCommands < 0)
        m_unknownCommands = 0;

    emit unknownCommandCounterChanged( m_unknownCommands);
}


void Calculator::setStatusWithErrorCommand( const uint8_t errorCode)
{
    Q_UNUSED( errorCode);

    m_statusWithErrorCommands ++;
    if( m_statusWithErrorCommands < 0)
        m_statusWithErrorCommands = 0;

    emit statusWithErrorCommandCounterChanged( m_statusWithErrorCommands);
}


void Calculator::setAirConditionerCmd(  const uint8_t fanSpeed,
                                        const uint8_t heaterOn,
                                        const int8_t peltierI)
{

}


void Calculator::setAirConditionerState(    const uint8_t fanSpeed,
                                            const uint8_t heaterOn,
                                            const int8_t peltierI,
                                            const int8_t t1,
                                            const int8_t t2,
                                            const int8_t t3,
                                            const uint32_t errors)
{

}


//void Calculator::setControlPositionCourse( const float course)
//{
//
//}


//void Calculator::setControlPositionPitch( const float pitch)
//{

//}

