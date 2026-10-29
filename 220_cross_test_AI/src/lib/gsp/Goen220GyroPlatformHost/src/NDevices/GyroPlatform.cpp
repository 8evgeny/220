
#include "GyroPlatform.h"

#include "ResultGetVersionCommand.h"
#include "ResultGetStatusCommand.h"
#include "ResultSetModeCommand.h"
#include "ResultSetControlPositionCommand.h"
#include "ResultSetControlSpeedCommand.h"
#include "ResultSetControlTrackingCommand.h"

#include "ResultSetUnknownCommand.h"

#include "SetModeCommand.h"
#include "DelimiterAndCrc8CommandDecorator.h"

#include <QDebug>

struct GyroPlatform::MemsBoardStatus
{
    ResultGetStatusCommand::MemsBoardStatus memsBoardStatus;
} /* struct GyroPlatform::MemsBoardStatus */;

struct GyroPlatform::MotorStatus
{
    ResultGetStatusCommand::MotorStatus motorStatus;
} /* struct GyroPlatform::MotorStatus */;

GyroPlatform::GyroPlatform(const uint8_t deviceId, QObject *parent)
    : CommandExecutor(parent), m_deviceId(deviceId), m_hostId(0x12), m_versionProtocolVdb(123),
      m_versionProtocol(456), m_versionDevice(789), m_mode(Off), m_allowResponse(true),
      m_pMemsBoardStatus(new MemsBoardStatus()), m_pMotorYaw(new MotorStatus()),
      m_pMotorPitch(new MotorStatus())
{
    m_pMemsBoardStatus->memsBoardStatus.mode = 0x3D;

    m_pMemsBoardStatus->memsBoardStatus.errors.errorAllMems = 0;

    m_pMemsBoardStatus->memsBoardStatus.flags.notConfigured.flags = 0x00;
    m_pMemsBoardStatus->memsBoardStatus.flags.illegalData.flags = 0x00;
    m_pMemsBoardStatus->memsBoardStatus.flags.noResponseOnSpi.flags = 0x00;
    m_pMemsBoardStatus->memsBoardStatus.flags.reserve.flags = 0x00;

    m_pMemsBoardStatus->memsBoardStatus.axisSwitchMode = 0x5f;
    m_pMemsBoardStatus->memsBoardStatus.memsProcessMode = 0x9b;

    m_pMemsBoardStatus->memsBoardStatus.speedYaw = 1234.5678f;
    m_pMemsBoardStatus->memsBoardStatus.speedPitch = 4321.012300f;
    m_pMemsBoardStatus->memsBoardStatus.angleYaw = 1122.334500f;
    m_pMemsBoardStatus->memsBoardStatus.anglePitch = 6655.443300f;

    m_pMotorYaw->motorStatus.mode = 0x7A;
    m_pMotorYaw->motorStatus.errors.flags = 0x00;
    m_pMotorYaw->motorStatus.flags.flags = 0x00;
    m_pMotorYaw->motorStatus.angle = 55.6677f;
    m_pMotorYaw->motorStatus.speed = 88.9900f;

    m_pMotorPitch->motorStatus.mode = 0xFA;
    m_pMotorPitch->motorStatus.errors.flags = 0x00;
    m_pMotorPitch->motorStatus.flags.flags = 0x00;
    m_pMotorPitch->motorStatus.angle = 555.666f;
    m_pMotorPitch->motorStatus.speed = 888.999f;
} /* GyroPlatform::GyroPlatform */

GyroPlatform::~GyroPlatform()
{
    delete m_pMotorPitch;
    delete m_pMotorYaw;
    delete m_pMemsBoardStatus;
} /* GyroPlatform::~GyroPlatform */

uint8_t GyroPlatform::getHostId() const
{
    return m_hostId;
} /* GyroPlatform::getHostId */

void GyroPlatform::setHostId(const uint8_t hostId)
{
    m_hostId = hostId;
} /* GyroPlatform::setHostId */

uint8_t GyroPlatform::getDeviceId() const
{
    return m_deviceId;
} /* GyroPlatform::getDeviceId */

void GyroPlatform::setDeviceId(const uint8_t deviceId)
{
    m_deviceId = deviceId;
} /* GyroPlatform::setDeviceId */

void GyroPlatform::setAllowResponse(bool allow)
{
    //    qDebug() << Q_FUNC_INFO << allow;
    m_allowResponse = allow;
} /* GyroPlatform::setAllowResponse */

void GyroPlatform::setMode(const Mode mode)
{
    //    qDebug() << Q_FUNC_INFO << mode;

    m_mode = mode;

    emit modeChanged(mode);

    if (!m_requiresResponse)
        return;

    Command *const cmd = new ResultSetModeCommand(m_deviceId, m_hostId, VdbCommand::NoError);
    Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator(cmd);

    if (m_allowResponse)
    {
        emit sendCmd(decoratedCmd);
    } /* if */
    else
    {
        delete decoratedCmd;
    } /* else */
} /* GyroPlatform::setMode */

void GyroPlatform::getVersion()
{
    //    qDebug() << Q_FUNC_INFO;

    Command *const cmd = new ResultGetVersionCommand(m_deviceId, m_hostId, m_versionProtocolVdb,
                                                     m_versionProtocol, m_versionDevice);
    Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator(cmd);

    //    qDebug() << Q_FUNC_INFO << decoratedCmd->serialize().toHex( ' ');

    if (m_allowResponse)
    {
        emit sendCmd(decoratedCmd);
    } /* if */
    else
    {
        delete decoratedCmd;
    } /* else */
} /* GyroPlatform::getVersion */

void GyroPlatform::getStatus()
{
    //    qDebug() << Q_FUNC_INFO;
    SetModeCommand::Mode mode = SetModeCommand::Unknown;
    switch (m_mode)
    {
    case Dummy:
        mode = SetModeCommand::Dummy;
        break;
    case Stabilization:
        mode = SetModeCommand::Stabilization;
        break;
    case RotaryPlatform:
        mode = SetModeCommand::RotaryPlatform;
        break;
    case Parkin:
        mode = SetModeCommand::Parkin;
        break;
    case Off:
        mode = SetModeCommand::Off;
        break;
    default:
        break;
    } /* switch */

    Command *const cmd = new ResultGetStatusCommand(
        m_deviceId, m_hostId, (uint8_t)mode, &m_pMemsBoardStatus->memsBoardStatus,
        &m_pMotorYaw->motorStatus, &m_pMotorPitch->motorStatus);
    Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator(cmd);

    //    qDebug() << Q_FUNC_INFO << decoratedCmd->serialize().toHex( ' ');

    if (m_allowResponse)
    {
        emit sendCmd(decoratedCmd);
    } /* if */
    else
    {
        delete decoratedCmd;
    } /* else */
} /* GyroPlatform::getStatus */

void GyroPlatform::setStatus(const uint8_t mode)
{

} /* GyroPlatform::setStatus */

void GyroPlatform::setVersion(const uint32_t versionProtocolVdb, const uint32_t versionProtocol,
                              const uint32_t versionDevice)
{

} /* GyroPlatform::setVersion */

void GyroPlatform::setModeByUser(const Mode mode)
{
    //    qDebug() << Q_FUNC_INFO << mode;

    m_mode = mode;
} /* GyroPlatform::setModeByUser */

void GyroPlatform::setMemsMode(const uint8_t mode)
{
    m_pMemsBoardStatus->memsBoardStatus.mode = mode;
} /* GyroPlatform::setMemsMode */

void GyroPlatform::setErrorAllMems(const bool error)
{
    m_pMemsBoardStatus->memsBoardStatus.errors.errorAllMems = error ? 1 : 0;
} /* GyroPlatform::setErrorAllMems */

void GyroPlatform::setMemsNotConfigured(const uint8_t flags)
{
    m_pMemsBoardStatus->memsBoardStatus.flags.notConfigured.flags = flags;
} /* GyroPlatform::setMemsNotConfigured */

void GyroPlatform::setMemsIllegalData(const uint8_t flags)
{
    m_pMemsBoardStatus->memsBoardStatus.flags.illegalData.flags = flags;
} /* GyroPlatform::setMemsIllegalData */

void GyroPlatform::setMemsNoResponseOnSpi(const uint8_t flags)
{
    m_pMemsBoardStatus->memsBoardStatus.flags.noResponseOnSpi.flags = flags;
} /* GyroPlatform::setMemsNoResponseOnSpi */

void GyroPlatform::setMemsReserve(const uint8_t flags)
{
    m_pMemsBoardStatus->memsBoardStatus.flags.reserve.flags = flags;
} /* GyroPlatform::setMemsReserve */

void GyroPlatform::setMemsAxisSwitchMode(const uint8_t mode)
{
    m_pMemsBoardStatus->memsBoardStatus.axisSwitchMode = mode;
} /* GyroPlatform::setMemsAxisSwitchMode */

void GyroPlatform::setMemsProcessMode(const uint8_t mode)
{
    m_pMemsBoardStatus->memsBoardStatus.memsProcessMode = mode;
} /* GyroPlatform::setMemsProcessMode */

void GyroPlatform::setMemsBoardStatusCommon(
    const uint8_t mode, const bool errorAllMems, const uint8_t memsNotConfiguredFlags,
    const uint8_t memsIllegalDataFlags, const uint8_t memsNoResponseOnSpiFlags,
    const uint8_t reserveFlags, const uint8_t memsAxisSwitchMode, const uint8_t memsProcessMode)
{

} /* GyroPlatform::setMemsBoardStatusCommon */

void GyroPlatform::setMemsBoardStatusSpeedAndAngle(const float speedYaw, const float speedPitch,
                                                   const float angleYaw, const float anglePitch)
{

} /* GyroPlatform::setMemsBoardStatusSpeedAndAngle */

void GyroPlatform::setMotorYawMode(const uint8_t mode)
{
    m_pMotorYaw->motorStatus.mode = mode;
} /* GyroPlatform::setMotorYawMode */

void GyroPlatform::setMotorYawErrors(const uint8_t flags)
{
    m_pMotorYaw->motorStatus.errors.flags = flags;
} /* GyroPlatform::setMotorYawErrors */

void GyroPlatform::setMotorYawFlags(const uint8_t flags)
{
    m_pMotorYaw->motorStatus.flags.flags = flags;
} /* GyroPlatform::setMotorYawFlags */

void GyroPlatform::setMotorYawAngle(const float angle)
{
    m_pMotorYaw->motorStatus.angle = angle;
} /* GyroPlatform::setMotorYawAngle */

void GyroPlatform::setMotorYawSpeed(const float speed)
{
    m_pMotorYaw->motorStatus.speed = speed;
} /* GyroPlatform::setMotorYawSpeed */

void GyroPlatform::setYawMotorStatus(const uint8_t mode, const uint8_t errors, const uint8_t flags,
                                     const float angle, const float speed)
{

} /* GyroPlatform::setYawMotorStatus */

void GyroPlatform::setMotorPitchMode(const uint8_t mode)
{
    m_pMotorPitch->motorStatus.mode = mode;
} /* GyroPlatform::setMotorPitchMode */

void GyroPlatform::setMotorPitchErrors(const uint8_t flags)
{
    m_pMotorPitch->motorStatus.errors.flags = flags;
} /* GyroPlatform::setMotorPitchErrors */

void GyroPlatform::setMotorPitchFlags(const uint8_t flags)
{
    m_pMotorPitch->motorStatus.flags.flags = flags;
} /* GyroPlatform::setMotorPitchFlags */

void GyroPlatform::setMotorPitchAngle(const float angle)
{
    m_pMotorPitch->motorStatus.angle = angle;
} /* GyroPlatform::setMotorPitchAngle */

void GyroPlatform::setMotorPitchSpeed(const float speed)
{
    m_pMotorPitch->motorStatus.speed = speed;
} /* GyroPlatform::setMotorPitchSpeed */

void GyroPlatform::setPitchMotorStatus(const uint8_t mode, const uint8_t errors,
                                       const uint8_t flags, const float angle, const float speed)
{

} /* GyroPlatform::setPitchMotorStatus */

// void GyroPlatform::setMemsNotConfigured( const uint32_t memsNumber, const bool value)
//{
//     if( memsNumber < 1 || memsNumber > 8)
//         return;

//    uint8_t flags = m_pMemsBoardStatus->memsBoardStatus.flags.notConfigured.flags;
//    if( value)
//        flags |= (0x01 << (memsNumber - 1));
//    else
//        flags &= ~(0x01 << (memsNumber - 1));

//    m_pMemsBoardStatus->memsBoardStatus.flags.notConfigured.flags = flags;
//}

// void GyroPlatform::setSpeedYawByUser( const float speed)
//{
//     m_pMemsBoardStatus->memsBoardStatus.speedYaw    = speed;
// }

void GyroPlatform::setSpeedYaw(const float speed)
{
    m_pMemsBoardStatus->memsBoardStatus.speedYaw = speed;
} /* GyroPlatform::setSpeedYaw */

void GyroPlatform::setSpeedPitch(const float speed)
{
    m_pMemsBoardStatus->memsBoardStatus.speedPitch = speed;
} /* GyroPlatform::setSpeedPitch */

void GyroPlatform::setAngleYaw(const float angle)
{
    m_pMemsBoardStatus->memsBoardStatus.angleYaw = angle;
} /* GyroPlatform::setAngleYaw */

void GyroPlatform::setAnglePitch(const float angle)
{
    m_pMemsBoardStatus->memsBoardStatus.anglePitch = angle;
} /* GyroPlatform::setAnglePitch */

void GyroPlatform::setControlPosition(const float yaw, const float pitch)
{
    //    qDebug() << course << pitch;

    emit controlPositionChanged(yaw, pitch);

    if (!m_requiresResponse)
        return;

    //  Test
    //    Command *const cmd = new ResultSetControlPositionCommand( m_deviceId, m_hostId,
    //                                                              (VdbCommand::Error)(VdbCommand::NoError+1));

    Command *const cmd =
        new ResultSetControlPositionCommand(m_deviceId, m_hostId, VdbCommand::NoError);
    Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator(cmd);

    if (m_allowResponse)
    {
        emit sendCmd(decoratedCmd);
    } /* if */
    else
    {
        delete decoratedCmd;
    } /* else */

} /* GyroPlatform::setControlPosition */

void GyroPlatform::setControlSpeed(const float yaw, const float pitch)
{
    emit controlSpeedChanged(yaw, pitch);

    if (!m_requiresResponse)
        return;

    Command *const cmd =
        new ResultSetControlSpeedCommand(m_deviceId, m_hostId, VdbCommand::NoError);
    Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator(cmd);

    if (m_allowResponse)
    {
        emit sendCmd(decoratedCmd);
    } /* if */
    else
    {
        delete decoratedCmd;
    } /* else */
} /* GyroPlatform::setControlSpeed */

void GyroPlatform::setControlTracking(const float yawMisalignment, const float pitchMisalignment,
                                      const float yawAccelLimit, const float pitchAccelLimit)
{
    emit controlTrackingChanged(yawMisalignment, pitchMisalignment, yawAccelLimit, pitchAccelLimit);

    if (!m_requiresResponse)
        return;

    Command *const cmd =
        new ResultSetControlTrackingCommand(m_deviceId, m_hostId, VdbCommand::NoError);
    Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator(cmd);

    if (m_allowResponse)
    {
        emit sendCmd(decoratedCmd);
    } /* if */
    else
    {
        delete decoratedCmd;
    } /* else */
} /* GyroPlatform::setControlTracking */

void GyroPlatform::setUnknownCommand(const uint8_t dstFunction)
{
    if (!m_requiresResponse)
        return;

    Command *const cmd = new ResultSetUnknownCommand(m_deviceId, m_hostId, dstFunction);
    Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator(cmd);

    if (m_allowResponse)
    {
        emit sendCmd(decoratedCmd);
    } /* if */
    else
    {
        delete decoratedCmd;
    } /* else */
} /* GyroPlatform::setUnknownCommand */

void GyroPlatform::setStatusWithErrorCommand(const uint8_t errorCode)
{

} /* GyroPlatform::setStatusWithErrorCommand */

// void GyroPlatform::setControlPositionCourse( const float course)
//{

//}

// void GyroPlatform::setControlPositionPitch( const float pitch)
//{

//}
