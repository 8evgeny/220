#ifndef GYRO_PLATFORM_HANDLER_H
#define GYRO_PLATFORM_HANDLER_H


#include <QObject>
#include <QMutex>


#define YAW_ACCEL_LIMIT     (20.0f)
#define PITCH_ACCEL_LIMIT   (20.0f)


class Goen220GyroPlatformHost;
class Pid;

class QTimer;



class GyroPlatformHandler : public QObject
{
    Q_OBJECT

public:
    explicit GyroPlatformHandler( Goen220GyroPlatformHost *const host, QObject *parent = nullptr);
    ~GyroPlatformHandler();

signals:
    void positionModeChanged( const bool zero, const bool pohod, const bool pilot, const bool park, const bool stabilization);

    void motorAngleChanged( const float z, const float x);
    void motorSpeedChanged( const float z, const float x);


public slots:
    void setDrivePositionMode( const bool zero, const bool pohod, const bool pilot, const bool park);
    void setStabilization( const bool enable);
    void setControlSpeed( const float z, const float x);
    void setControlPositionRel( const float z, const float x);
    void setControlPositionAbs( const float z, const float x);

    void turnOffMotors();


    void setActiveLockTracking( bool active, bool tracking);
    void updateTrackerObject( uint32_t cx_out, uint32_t cy_out, uint32_t w_out, uint32_t h_out);

    void setTvField( const float tvFieldWGrad, const float tvFieldHGrad);

    void enableCompensateGyroDrift( const bool enable);
    void setGyroDrift( const float yaw, const float pitch);

private slots:
    void updateSpeed();

private:
    void initGyroPlatform();

private:
    Goen220GyroPlatformHost     *const  m_pHost;

    bool    	m_zero;
    bool    	m_pohod;
    bool    	m_pilot;
    bool    	m_park;
    bool    	m_stabilization;

    float   	m_yawAngle;
    float   	m_pitchAngle;

    float   	m_yawSpeed;
    float   	m_pitchSpeed;

    bool    	m_tracking;
    float   	m_tvFieldWGrad;
    float   	m_tvFieldHGrad;

    bool        m_enableStabAfterZero;

    QMutex  	m_tvFieldMutex;

    Pid	*const	m_pPidW;
    Pid	*const	m_pPidH;

    QTimer      *m_pTimerForUpdateSpeed;
 //   bool        m_updateSpeedEnable;

    bool        m_compensateGyroDrift;
    bool        m_lockCompensateGyroDrift;

    float       m_yawCorr;
    float       m_pitchCorr;
};



#endif
