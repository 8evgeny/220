
#ifndef COMMAND_EXECUTOR_H
#define COMMAND_EXECUTOR_H


#include <QObject>


class Command;


class CommandExecutor : public QObject
{
    Q_OBJECT

public:
    explicit CommandExecutor( QObject *parent = nullptr);

    enum Mode : uint8_t
    {
         Dummy
        ,Stabilization
        ,RotaryPlatform
        ,Parkin
        ,Off

        ,Unknown        = 0xFF
    };

    void setRequiresResponse( const bool requiresResponse);


public slots:
    virtual uint8_t getHostId() const = 0;
    virtual void setHostId( const uint8_t hostId) = 0;

    virtual uint8_t getDeviceId() const = 0;
    virtual void setDeviceId( const uint8_t deviceId) = 0;

    virtual void setMode( const Mode mode) = 0;
    virtual void getVersion() = 0;
    virtual void getStatus() = 0;
    virtual void setStatus( const uint8_t mode) = 0;
    virtual void setVersion( const uint32_t versionProtocolVdb, const uint32_t versionProtocol,
                             const uint32_t versionDevice) = 0;

    virtual void setMemsMode( const uint8_t mode) = 0;
    virtual void setErrorAllMems( const bool error) = 0;
//    virtual void setMemsNotConfigured( const uint32_t flagIndex, const bool value) = 0;
    virtual void setMemsNotConfigured( const uint8_t flags) = 0;
    virtual void setMemsIllegalData( const uint8_t flags) = 0;
    virtual void setMemsNoResponseOnSpi( const uint8_t flags) = 0;
    virtual void setMemsReserve( const uint8_t flags) = 0;
    virtual void setMemsAxisSwitchMode( const uint8_t mode) = 0;
    virtual void setMemsProcessMode( const uint8_t mode) = 0;

    virtual void setMemsBoardStatusCommon(  const uint8_t mode, const bool errorAllMems,
                                            const uint8_t memsNotConfiguredFlags,
                                            const uint8_t memsIllegalDataFlags,
                                            const uint8_t memsNoResponseOnSpiFlags,
                                            const uint8_t reserveFlags,
                                            const uint8_t memsAxisSwitchMode,
                                            const uint8_t memsProcessMode) = 0;

    virtual void setMemsBoardStatusSpeedAndAngle(   const float speedYaw,
                                                    const float speedPitch,
                                                    const float angleYaw,
                                                    const float anglePitch) = 0;

    virtual void setMotorYawMode( const uint8_t mode) = 0;
    virtual void setMotorYawErrors( const uint8_t flags) = 0;
    virtual void setMotorYawFlags( const uint8_t flags) = 0;
    virtual void setMotorYawAngle( const float angle) = 0;
    virtual void setMotorYawSpeed( const float speed) = 0;

    virtual void setYawMotorStatus( const uint8_t mode, const uint8_t errors, const uint8_t flags,
                                    const float angle, const float speed) = 0;

    virtual void setMotorPitchMode( const uint8_t mode) = 0;
    virtual void setMotorPitchErrors( const uint8_t flags) = 0;
    virtual void setMotorPitchFlags( const uint8_t flags) = 0;
    virtual void setMotorPitchAngle( const float angle) = 0;
    virtual void setMotorPitchSpeed( const float speed) = 0;

    virtual void setPitchMotorStatus(	const uint8_t mode, const uint8_t errors, const uint8_t flags,
                                        const float angle, const float speed) = 0;


    virtual void setSpeedYaw( const float speed) = 0;
    virtual void setSpeedPitch( const float speed) = 0;
    virtual void setAngleYaw( const float angle) = 0;
    virtual void setAnglePitch( const float angle) = 0;

//    virtual void setControlPositionCourse( const float course) = 0;
//    virtual void setControlPositionPitch( const float pitch) = 0;

    virtual void setControlPosition( const float yaw, const float pitch) = 0;

    virtual void setControlSpeed( const float yaw, const float pitch) = 0;

    virtual void setControlTracking(    const float yawMisalignment, const float pitchMisalignment,
                                        const float yawAccelLimit, const float pitchAccelLimit) = 0;

    virtual void setUnknownCommand( const uint8_t dstFunction) = 0;

    virtual void setStatusWithErrorCommand( const uint8_t errorCode) = 0;


    virtual void setAirConditionerCmd(  const uint8_t fanSpeed,
                                        const uint8_t heaterOn,
                                        const int8_t peltierI) = 0;

    virtual void setAirConditionerState(    const uint8_t fanSpeed,
                                            const uint8_t heaterOn,
                                            const int8_t peltierI,
                                            const int8_t t1,
                                            const int8_t t2,
                                            const int8_t t3,
                                            const uint32_t errors) = 0;

signals:
    void modeChanged( const Mode mode);
    void versionChanged( const uint32_t versionProtocolVdb, const uint32_t versionProtocol,
                         const uint32_t versionDevice);

    void setModeCommandCompleted();
    void setControlPositionCommandCompleted();
    void setControlSpeedCommandCompleted();
    void setControlTrackingCommandCompleted();

    void memsModeChanged( const uint8_t mode);
    void errorAllMemsChanged( const bool errors);

    void memsNotConfiguredChanged( const uint8_t flags);
    void memsIllegalDataChanged( const uint8_t flags);
    void memsNoResponseOnSpiChanged( const uint8_t flags);
    void memsReserveChanged( const uint8_t flags);
    void memsAxisSwitchModeChanged( const uint8_t mode);
    void memsProcessModeChanged( const uint8_t mode);

    void memsBoardStatusCommonChanged(  const uint8_t mode, const bool errorAllMems,
                                        const uint8_t memsNotConfiguredFlags,
                                        const uint8_t memsIllegalDataFlags,
                                        const uint8_t memsNoResponseOnSpiFlags,
                                        const uint8_t reserveFlags,
                                        const uint8_t memsAxisSwitchMode,
                                        const uint8_t memsProcessMode);

    void memsBoardStatusSpeedAndAngleChanged(   const float speedYaw,
                                                const float speedPitch,
                                                const float angleYaw,
                                                const float anglePitch);

    void motorYawModeChanged( const uint8_t mode);
    void motorYawErrorsChanged( const uint8_t flags);
    void motorYawFlagsChanged( const uint8_t flags);
    void motorYawAngleChanged( const float angle);
    void motorYawSpeedChanged( const float speed);

    void yawMotorStatusChanged(	const uint8_t mode, const uint8_t errors, const uint8_t flags,
                                const float angle, const float speed);

    void motorPitchModeChanged( const uint8_t mode);
    void motorPitchErrorsChanged( const uint8_t flags);
    void motorPitchFlagsChanged( const uint8_t flags);
    void motorPitchAngleChanged( const float angle);
    void motorPitchSpeedChanged( const float speed);

    void pitchMotorStatusChanged(	const uint8_t mode, const uint8_t errors, const uint8_t flags,
                                    const float angle, const float speed);

    void speedYawChanged( const float speed);
    void speedPitchChanged( const float speed);
    void angleYawChanged( const float angle);
    void anglePitchChanged( const float angle);

    void controlPositionChanged( const float yaw, const float pitch);
    void controlSpeedChanged( const float yaw, const float pitch);
    void controlTrackingChanged(    const float yawMisalignment, const float pitchMisalignment,
                                    const float yawAccelLimit, const float pitchAccelLimit);

    void unknownCommandCounterChanged( const int32_t unknownCommands);

    void statusWithErrorCommandCounterChanged( const int32_t statusWithErrorCommands);

    void airConditionerStateChanged(    const uint8_t fanSpeed,
                                        const uint8_t heaterOn,
                                        const int8_t peltierI,
                                        const int8_t t1,
                                        const int8_t t2,
                                        const int8_t t3,
                                        const uint32_t errors);

    void setAirConditionerCommandCompleted();

signals:
    void sendCmd( Command *const cmd);


protected:
    bool 	m_requiresResponse;
};





#endif

