
#ifndef GYRO_PLATFORM_H
#define GYRO_PLATFORM_H


#include "CommandExecutor.h"




class GyroPlatform : public CommandExecutor
{
    Q_OBJECT

public:
    explicit GyroPlatform( const uint8_t deviceId, QObject *parent = nullptr);
    ~GyroPlatform();

public slots:
    uint8_t getHostId() const override;
    void setHostId( const uint8_t hostId) override;

    uint8_t getDeviceId() const override;
    void setDeviceId( const uint8_t deviceId) override;


    void setAllowResponse( bool allow);

    void setMode( const Mode mode) override;
    void getVersion() override;
    void getStatus() override;
    void setStatus( const uint8_t mode) override;

    void setVersion(    const uint32_t versionProtocolVdb, const uint32_t versionProtocol,
                        const uint32_t versionDevice)   override;


    void setModeByUser( const Mode mode);
//    void setSpeedYawByUser( const float speed);

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

signals:
//    void modeChanged( const Mode mode);

private:
    uint8_t             m_deviceId;
    uint8_t             m_hostId;

    uint32_t            m_versionProtocolVdb;
    uint32_t            m_versionProtocol;
    uint32_t            m_versionDevice;

    Mode                m_mode;

    bool                m_allowResponse;

    struct MemsBoardStatus;
    MemsBoardStatus     *const m_pMemsBoardStatus;

    struct MotorStatus;
    MotorStatus         *const m_pMotorYaw;
    MotorStatus         *const m_pMotorPitch;
};




#endif

