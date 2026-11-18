
#ifndef CALCULATOR_H
#define CALCULATOR_H


#include "CommandExecutor.h"


class Calculator : public CommandExecutor
{
    Q_OBJECT

public:
    explicit Calculator( const uint8_t hostId, QObject *parent = nullptr);



public slots:
    uint8_t getHostId() const override;
    void setHostId( const uint8_t hostId) override;

    uint8_t getDeviceId() const override;
    void setDeviceId( const uint8_t deviceId) override;


    void setMode( const Mode mode) override;

    void getVersion() override;
    void getStatus() override;
    void setStatus( const uint8_t mode) override;

    void setVersion(    const uint32_t versionProtocolVdb, const uint32_t versionProtocol,
                        const uint32_t versionDevice)   override;

    void setMemsMode( const uint8_t mode) override;
    void setErrorAllMems( const bool error) override;

//    void setMemsNotConfigured( const uint32_t memsNumber, const bool value) override;
    void setMemsNotConfigured( const uint8_t flags) override;
    void setMemsIllegalData( const uint8_t flags) override;
    void setMemsNoResponseOnSpi( const uint8_t flags) override;
    void setMemsReserve( const uint8_t flags) override;
    void setMemsAxisSwitchMode( const uint8_t mode) override;
    void setMemsProcessMode( const uint8_t mode) override;

    void setMemsBoardStatusCommon(  const uint8_t mode, const bool errorAllMems,
                                    const uint8_t memsNotConfiguredFlags,
                                    const uint8_t memsIllegalDataFlags,
                                    const uint8_t memsNoResponseOnSpiFlags,
                                    const uint8_t reserveFlags,
                                    const uint8_t memsAxisSwitchMode,
                                    const uint8_t memsProcessMode) override;

    void setMemsBoardStatusSpeedAndAngle(   const float speedYaw,
                                            const float speedPitch,
                                            const float angleYaw,
                                            const float anglePitch) override;

    void setMotorYawMode( const uint8_t mode) override;
    void setMotorYawErrors( const uint8_t flags) override;
    void setMotorYawFlags( const uint8_t flags) override;
    void setMotorYawAngle( const float angle) override;
    void setMotorYawSpeed( const float speed) override;

    void setYawMotorStatus(	const uint8_t mode, const uint8_t errors, const uint8_t flags,
                            const float angle, const float speed) override;

    void setMotorPitchMode( const uint8_t mode) override;
    void setMotorPitchErrors( const uint8_t flags) override;
    void setMotorPitchFlags( const uint8_t flags) override;
    void setMotorPitchAngle( const float angle) override;
    void setMotorPitchSpeed( const float speed) override;

    void setPitchMotorStatus(	const uint8_t mode, const uint8_t errors, const uint8_t flags,
                                const float angle, const float speed) override;

    void setSpeedYaw( const float speed) override;
    void setSpeedPitch( const float speed) override;
    void setAngleYaw( const float angle) override;
    void setAnglePitch( const float angle) override;

//    void setControlPositionCourse( const float course) override;
//    void setControlPositionPitch( const float pitch) override;

    void setControlPosition( const float yaw, const float pitch) override;
    void setControlSpeed( const float yaw, const float pitch) override;

    void setControlTracking(    const float yawMisalignment, const float pitchMisalignment,
                                const float yawAccelLimit, const float pitchAccelLimit) override;

    void setUnknownCommand( const uint8_t dstFunction) override;

    void setStatusWithErrorCommand( const uint8_t errorCode) override;

    void setAirConditionerCmd(  const uint8_t fanSpeed,
                                const uint8_t heaterOn,
                                const int8_t peltierI) override;

    void setAirConditionerState(    const uint8_t fanSpeed,
                                    const uint8_t heaterOn,
                                    const int8_t peltierI,
                                    const int8_t t1,
                                    const int8_t t2,
                                    const int8_t t3,
                                    const uint32_t errors) override;

private:
    uint8_t             m_hostId;
    uint8_t             m_deviceId;

    Mode                m_mode;

    uint8_t             m_memsMode;
    bool                m_memsErrorAll;
    uint8_t             m_memsNotConfiguredFlags;
    uint8_t             m_memsIllegalDataFlags;
    uint8_t             m_memsNoResponseOnSpiFlags;
    uint8_t             m_memsReserveFlags;
    uint8_t             m_memsAxisSwitchMode;
    uint8_t             m_memsProcessMode;

    float               m_speedYaw;
    float               m_speedPitch;
    float               m_angleYaw;
    float               m_anglePitch;

    uint8_t             m_motorYawMode;
    uint8_t             m_motorYawErrors;
    uint8_t             m_motorYawFlags;
    float 				m_motorYawAngle;
    float 				m_motorYawSpeed;

    uint8_t             m_motorPitchMode;
    uint8_t             m_motorPitchErrors;
    uint8_t             m_motorPitchFlags;
    float 				m_motorPitchAngle;
    float 				m_motorPitchSpeed;

    int32_t 			m_unknownCommands;
    int32_t 			m_statusWithErrorCommands;
};


#endif

