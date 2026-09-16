
#include "GyroPlatformHandler.h"

#include "Goen220GyroPlatformHost.h"
#include "Pid.h"


#include <QDebug>
#include <QTimer>



GyroPlatformHandler::GyroPlatformHandler( Goen220GyroPlatformHost *const host, QObject *parent)
    : QObject( parent)
    , m_pHost( host)
    , m_zero( false)
    , m_pohod( false)
    , m_pilot( false)
    , m_park( false)
    , m_stabilization( false)
    , m_yawAngle( 0.0f)
    , m_pitchAngle( 0.0f)
    , m_yawSpeed( 0.0f)
    , m_pitchSpeed( 0.0f)
    , m_tracking( false)
    , m_tvFieldWGrad( 0.0f)
    , m_tvFieldHGrad( 0.0f)
    , m_enableStabAfterZero( false)
    , m_pPidW( new Pid( 0.05f, 0.0f, 0.0f))
    , m_pPidH( new Pid( 0.05f, 0.0f, 0.0f))
    , m_pTimerForUpdateSpeed( new QTimer())
//    , m_updateSpeedEnable( true)
    , m_compensateGyroDrift( false)
    , m_lockCompensateGyroDrift( false)
    , m_yawCorr( -0.355f)                   //  -0.3f - L, -0.39f - R
    , m_pitchCorr( -0.530f)                 //  -0.5f - T, -0.6f  - B
{
    initGyroPlatform();
}


GyroPlatformHandler::~GyroPlatformHandler()
{
    if( m_pTimerForUpdateSpeed)
        delete m_pTimerForUpdateSpeed;

    if( m_pPidH)
        delete m_pPidH;

    if( m_pPidW)
        delete m_pPidW;
}


void GyroPlatformHandler::setDrivePositionMode( const bool zero, const bool pohod, const bool pilot, const bool park)
{
    if( m_pHost == nullptr)
        return;

    m_pTimerForUpdateSpeed->stop();

    if( zero)
    {
        m_pHost->setMode( Goen220GyroPlatformHost::RotaryPlatform);
        m_pHost->setControlPosition( 0.0f, 0.0f);
        // if( m_stabilization)
        // {
        //     m_enableStabAfterZero = true;
        // }
    }

    if( pohod)
    {
        m_pHost->setControlPosition( -170.0f, 0.0f);
    }

    if( pilot)
    {
        m_pHost->setControlPosition( 0.0f, 90.0f);
    }

    if( park)
    {
//        m_pHost->setControlPosition( 0.0f, 90.0f);
        m_pHost->setMode( Goen220GyroPlatformHost::Parkin);
    }
    else
        m_pHost->setMode( Goen220GyroPlatformHost::RotaryPlatform);

    m_zero          = zero;
    m_pohod         = pohod;
    m_pilot         = pilot;
    m_park          = park;
    m_stabilization = false;

    emit positionModeChanged( m_zero, m_pohod, m_pilot, m_park, m_stabilization);
}


void GyroPlatformHandler::setStabilization( const bool enable)
{
    if( m_pHost == nullptr)
        return;

    if( enable)
        m_pHost->setMode( Goen220GyroPlatformHost::Stabilization);
    else
        m_pHost->setMode( Goen220GyroPlatformHost::RotaryPlatform);

    //m_pHost->setMode( Goen220GyroPlatformHost::RotaryPlatform);

    m_zero          = false;
    m_pohod         = false;
    m_pilot         = false;
    m_park          = false;
    m_stabilization = enable;

    if( enable)
    {
        m_lockCompensateGyroDrift = false;
        updateSpeed();
        m_pTimerForUpdateSpeed->start();
    }
    else
    {
        m_pTimerForUpdateSpeed->stop();
    }

    emit positionModeChanged( m_zero, m_pohod, m_pilot, m_park, m_stabilization);
}


void GyroPlatformHandler::setControlSpeed( const float z, const float x)
{
    if( m_pHost == nullptr)
        return;

    qDebug() << Q_FUNC_INFO << z << x;

    if( m_stabilization)
    {
        // if( z == 0.0f && x == 0.0f)
        //     m_pTimerForUpdateSpeed->start();
        // else
        //     m_pTimerForUpdateSpeed->stop();

        if( m_compensateGyroDrift)
        {
            m_lockCompensateGyroDrift = (z > -0.01f && z < 0.01f && x > -0.01f && x < 0.01f) ? false : true;
            m_pHost->setControlSpeed( (-x * m_tvFieldWGrad) + m_yawCorr, (z * m_tvFieldWGrad) + m_pitchCorr);
        }
        else
            m_pHost->setControlSpeed( (-x * m_tvFieldWGrad), (z * m_tvFieldWGrad));
    }
    else
    {
       if( x != 0.0f || z != 0.0f)
       {
           m_yawAngle = (m_yawAngle - x);
           m_pitchAngle = (m_pitchAngle + z);
           m_pHost->setControlPosition( m_yawAngle, m_pitchAngle);
       }
    }
}


void GyroPlatformHandler::setControlPositionRel( const float z, const float x)
{
    if( m_pHost == nullptr)
        return;

    qDebug() << Q_FUNC_INFO << z << x;
    m_pHost->setControlTracking( z, x, YAW_ACCEL_LIMIT, PITCH_ACCEL_LIMIT);
}


void GyroPlatformHandler::setControlPositionAbs( const float z, const float x)
{
    if( m_pHost == nullptr)
        return;

    qDebug() << Q_FUNC_INFO << z << x;
    m_pHost->setControlPosition( z, x);
}


void GyroPlatformHandler::turnOffMotors()
{
    qDebug() << Q_FUNC_INFO;
    m_pTimerForUpdateSpeed->stop();
    m_pHost->setMode( Goen220GyroPlatformHost::Off);
}


void GyroPlatformHandler::setActiveLockTracking( bool active, bool tracking)
{
    if( m_tracking != tracking)
    {
        m_tracking = tracking;
        // qDebug() << Q_FUNC_INFO << tracking;
        if( tracking == false)
            m_pHost->setControlSpeed( 0.0f, 0.0f);
    }
}


void GyroPlatformHandler::updateTrackerObject( uint32_t cx_out, uint32_t cy_out, uint32_t w_out, uint32_t h_out)
{
    if( m_tracking == false)
        return;

    const int32_t w = 1920U;
    const int32_t h = 1080U;

    const int32_t wCenter = w / 2;
    const int32_t hCenter = h / 2;

    const int32_t errorWPix = (cx_out  - wCenter);
    const int32_t errorHPix = (cy_out - hCenter);

    const float kW = -50.0f;
    const float kH = -50.0f;

    m_tvFieldMutex.lock();
        const float errorWGrad = ((m_tvFieldWGrad / w) * errorWPix) * kW;
        const float errorHGrad = ((m_tvFieldHGrad / h) * errorHPix) * kH;
    m_tvFieldMutex.unlock();

//    qDebug() << Q_FUNC_INFO << errorWGrad << errorHGrad << w_out << h_out;

    const float pidW = m_pPidW->getPidOutput( errorWGrad);
    const float pidH = m_pPidH->getPidOutput( errorHGrad);

    m_pHost->setControlSpeed( pidW, pidH);
    //m_pHost->setControlSpeed( pidH, 0.0f);

/*
    static uint32_t cx_prev = 0.0f;
    if( qAbs( cx_prev - cx_out) > 50)
    {
        if( cx_prev > cx_out)
            cx_prev -= 10;
        else
            cx_prev += 10;

        return;
    }

    cx_prev = cx_out;
    // qDebug() << cx_out;



    const int32_t wCenter = w / 2;

    const int32_t wDelta = wCenter - cx_out;
    if( qFuzzyCompare( wDelta, 0.0f) == true)
        return;

    // qDebug() << wDelta;
    float z = 0.0f;
    float threshold = 0.1f;
    if( wDelta < 0)
    {
        // m_pHost->setControlTracking( z, x, YAW_ACCEL_LIMIT, PITCH_ACCEL_LIMIT);
        z = ((float)wDelta < -threshold) ? -threshold : ((float)wDelta);
    }
    else
    {
        z = ((float)wDelta > threshold) ? threshold : ((float)wDelta);
    }

    m_pHost->setControlSpeed( z, 0.0f);
*/
}


void GyroPlatformHandler::setTvField( const float tvFieldWGrad, const float tvFieldHGrad)
{
    const float k = 2.0f;

    {
        QMutexLocker mutex( &m_tvFieldMutex);
        if( qFuzzyCompare( tvFieldWGrad, m_tvFieldWGrad) == false)
            m_tvFieldWGrad = (tvFieldWGrad / k);

        if( qFuzzyCompare( tvFieldHGrad, m_tvFieldHGrad) == false)
            m_tvFieldHGrad = tvFieldHGrad / k;
    }

    // qDebug() << Q_FUNC_INFO << tvFieldHGrad << tvFieldWGrad;
}


void GyroPlatformHandler::enableCompensateGyroDrift( const bool enable)
{
    m_compensateGyroDrift = enable;
}


void GyroPlatformHandler::setGyroDrift( const float yaw, const float pitch)
{
    m_yawCorr   = yaw;
    m_pitchCorr = pitch;
}


void GyroPlatformHandler::updateSpeed()
{
    if( m_lockCompensateGyroDrift)
        return;

//    qDebug() << Q_FUNC_INFO;
    //  курс (Yaw) Z, тангаж (Pitch) X
    if( m_compensateGyroDrift)
        m_pHost->setControlSpeed( m_yawCorr, m_pitchCorr);
}


void GyroPlatformHandler::initGyroPlatform()
{
    if( m_pHost == nullptr)
        return;

    connect( m_pHost, &Goen220GyroPlatformHost::stateConnectToSerialPortChanged, this,
        [this]( const bool con)
        {
            qDebug() << "GSP:  Подключение к TTY: " << (con ? QString::fromUtf8( "Установлено") : QString::fromUtf8( "Отсутствует"));
        }
    );

    connect( m_pHost, &Goen220GyroPlatformHost::stateConnectToRemoteDeviceChanged, this,
        [this]( const bool con)
        {
            qDebug() << "GSP: Связь с ГироПлатформой: " << (con ? QString::fromUtf8( "Установлена") : QString::fromUtf8( "Потеряна"));
            if( con)
            {
                m_park = true;
                m_pHost->setMode( Goen220GyroPlatformHost::Parkin);
            }
        }
    );

    connect( m_pHost, &Goen220GyroPlatformHost::serialPortErrorStringChanged, this,
        [this]()
        {
            const QString error = m_pHost->getSerialPortErrorString();
            if( error.isEmpty() == true || error.isNull() == true)
                return;

            qWarning() << "************************************************************************ SERIAL PORT ERROR: " << error;
            m_pHost->connect( false);
        }
    );


    connect( m_pHost, &Goen220GyroPlatformHost::yawMotorStatusChanged, this,
        [this]( const MotorStatus &motorStatus)
        {
            if( qFuzzyCompare( m_yawAngle, motorStatus.angle) == false)
            {
                m_yawAngle      = motorStatus.angle;
                // if( m_enableStabAfterZero &&
                //     m_yawAngle > -4.0f && m_yawAngle < 4.0f &&
                //     m_pitchAngle > -4.0f && m_pitchAngle < 4.0f)
                // {
                //     m_pHost->setMode( Goen220GyroPlatformHost::Stabilization);
                //     m_enableStabAfterZero = false;
                // }

//                emit motorAngleChanged( m_yawAngle, m_pitchAngle);
                emit motorAngleChanged( -m_pitchAngle, m_yawAngle);
            }

            if( qFuzzyCompare( m_yawSpeed, motorStatus.speed) == false)
            {
                m_yawSpeed      = motorStatus.speed;
                emit motorSpeedChanged( m_yawSpeed, m_pitchSpeed);
            }
        }
    );

    connect( m_pHost, &Goen220GyroPlatformHost::pitchMotorStatusChanged, this,
        [this]( const MotorStatus &motorStatus)
        {
            if( qFuzzyCompare( m_pitchAngle, motorStatus.angle) == false)
            {
                m_pitchAngle    = motorStatus.angle;
                //emit motorAngleChanged( m_yawAngle, m_pitchAngle);
                emit motorAngleChanged( -m_pitchAngle, m_yawAngle);
            }

            if( qFuzzyCompare( m_pitchSpeed, motorStatus.speed) == false)
            {
                m_pitchSpeed    = motorStatus.speed;
                // qDebug() << m_pitchSpeed;
                emit motorSpeedChanged( m_yawSpeed, m_pitchSpeed);
            }
        }
    );

    connect( m_pTimerForUpdateSpeed, &QTimer::timeout, this, &GyroPlatformHandler::updateSpeed);
    m_pTimerForUpdateSpeed->setSingleShot( false);
    m_pTimerForUpdateSpeed->setInterval( 2000);
}
