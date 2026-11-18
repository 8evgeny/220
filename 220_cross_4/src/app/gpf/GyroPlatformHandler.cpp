
#include "GyroPlatformHandler.h"

#include "Goen220GyroPlatformHost.h"
#include "Pid.h"
#include "../settingsmanager.h"


#include <QDebug>
#include <QTimer>
#include <QCoreApplication>
#include <QDateTime>
#include <cmath>
#include <algorithm>
#include <limits>


GyroPlatformHandler::GyroPlatformHandler(Goen220GyroPlatformHost *const host, QObject *parent)
    : QObject(parent)
    , m_pHost(host)
    , m_zero(false)
    , m_pohod(false)
    , m_pilot(false)
    , m_park(false)
    , m_stabilization(false)
    , m_yawAngle(0.0f)
    , m_pitchAngle(0.0f)
    , m_yawSpeed(0.0f)
    , m_pitchSpeed(0.0f)
    , m_tracking(false)
    , m_tvFieldWGrad(0.0f)
    , m_tvFieldHGrad(0.0f)
    , m_enableStabAfterZero(false)
    , m_pPidW(new Pid(0.05f, 0.0f, 0.0f))
    , m_pPidH(new Pid(0.025f, 0.0f, 0.0f))
    , m_pTimerForUpdateSpeed(new QTimer())
    , m_compensateGyroDrift(false)
    , m_lockCompensateGyroDrift(false)
    , m_yawCorr(-0.355f)
    , m_pitchCorr(-0.530f)
{
    initGyroPlatform();

    // Таймер калибровки
    m_pCalWarmupTimer = new QTimer(this);
    m_pCalWarmupTimer->setSingleShot(true);
    connect(m_pCalWarmupTimer, &QTimer::timeout,
            this, &GyroPlatformHandler::onCalWarmupFinished);

    m_pCalDurationTimer = new QTimer(this);
    m_pCalDurationTimer->setSingleShot(true);
    connect(m_pCalDurationTimer, &QTimer::timeout,
            this, &GyroPlatformHandler::onCalMeasureFinished);


    // Путь к конфигурационному файлу по умолчанию
    m_settingsFilePath = QCoreApplication::applicationDirPath() + "/config.ini";

    // Таймер TrackerAdapter: периодический step(). Сам адаптер создаётся лениво
    // в initTrackerAdapter() при первом захвате — к этому моменту main_app уже
    // вызвал setSettingsManager()/setSettingsFilePath().
    m_pTimerAdapter = new QTimer(this);
    m_pTimerAdapter->setSingleShot(false);
    connect(m_pTimerAdapter, &QTimer::timeout,
            this, &GyroPlatformHandler::adapterStep);
}



GyroPlatformHandler::~GyroPlatformHandler()
{
    if( m_pTimerForUpdateSpeed)
        delete m_pTimerForUpdateSpeed;

    if( m_pPidH)
        delete m_pPidH;

    if( m_pPidW)
        delete m_pPidW;

    if( m_pTrackerAdapter)
        delete m_pTrackerAdapter;
}


void GyroPlatformHandler::setDrivePositionMode( const bool zero, const bool pohod, const bool pilot, const bool park)
{
    if( m_pHost == nullptr)
        return;

    m_pTimerForUpdateSpeed->stop();
    stopTrackerAdapter();

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

void GyroPlatformHandler::procSetTrackerChannel(quint8 ch_id)
{
    m_ChannelId = ch_id;
}

void GyroPlatformHandler::setControlSpeed(const float z, const float x)
{
    if (m_pHost == nullptr)
        return;

    if (m_stabilization)
    {
        // ★ Выбор FOV по активному каналу ★
        float fieldWGrad = 58.1f;
        float fieldHGrad = 45.1f;
        {
            QMutexLocker lock(&m_tvFieldMutex);
            switch (m_ChannelId) {
            case 0x80:  // TV
                fieldWGrad = m_tvFieldWGrad;
                fieldHGrad = m_tvFieldHGrad;
                break;
            case 0x81:  // IR
                fieldWGrad = m_irFieldWGrad;
                fieldHGrad = m_irFieldHGrad;
                break;
            default:
                fieldWGrad = m_tvFieldWGrad;
                fieldHGrad = m_tvFieldHGrad;
                break;
            }
        }

        if (m_compensateGyroDrift)
        {
            const bool isZero = (z > -0.01f && z < 0.01f && x > -0.01f && x < 0.01f);
            m_lockCompensateGyroDrift = !isZero;

            m_pHost->setControlSpeed((-x * fieldWGrad) + m_yawCorr,
                                     ( z * fieldHGrad) + m_pitchCorr);
        }
        else
        {
            m_lockCompensateGyroDrift = false;
            m_pHost->setControlSpeed((-x * fieldWGrad),
                                     ( z * fieldHGrad));
        }
    }
    else
    {
        if (x != 0.0f || z != 0.0f)
        {
            m_yawAngle   -= x;
            m_pitchAngle += z;
            m_pHost->setControlPosition(m_yawAngle, m_pitchAngle);
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
    stopTrackerAdapter();
    m_pHost->setMode( Goen220GyroPlatformHost::Off);
}
void GyroPlatformHandler::setActiveLockTracking(bool active, bool tracking)
{
    if (m_tracking != tracking) {
        m_tracking = tracking;

        if (tracking == false) {
            // Останавливаем адаптер ПЕРВЫМ: stopTracking() шлёт (0,0) через
            // onTrackerSpeed(), который устанавливает m_lockCompensateGyroDrift
            stopTrackerAdapter();

            // Снимаем lock — updateSpeed() снова активен
            m_lockCompensateGyroDrift = false;

            // ПОДАЁМ КОМПЕНСАЦИЮ
            if (m_compensateGyroDrift)
                m_pHost->setControlSpeed(m_yawCorr, m_pitchCorr);
            else
                m_pHost->setControlSpeed(0.0f, 0.0f);

        } else {
            // Трекинг начался — блокируем updateSpeed()
            m_lockCompensateGyroDrift = true;

            // Новый контур: адаптер + таймер step() (старый Pid-контур при этом
            // отключён ранним return в updateTrackerObject)
            initTrackerAdapter();
            if (m_useAdapter && m_pTrackerAdapter) {
                m_pTrackerAdapter->startTracking();
                m_pTimerAdapter->start(m_adapterStepMs);
            }
        }
    }
}
void GyroPlatformHandler::updateTrackerObject(uint32_t cx_out, uint32_t cy_out,
                                              uint32_t w_out, uint32_t h_out)
{
    if (m_tracking == false)
        return;

    // Потеря объекта — держим компенсацию
    if (cx_out == 0 && cy_out == 0) {
        // Адаптер: стоп (шлёт (0,0) → onTrackerSpeed удержит дрейф).
        // При появлении объекта ветка ниже возобновит startTracking().
        if (m_useAdapter && m_pTrackerAdapter)
            m_pTrackerAdapter->stopTracking();

        if (m_compensateGyroDrift)
            m_pHost->setControlSpeed(m_yawCorr, m_pitchCorr);
        else
            m_pHost->setControlSpeed(0.0f, 0.0f);
        return;
    }

    int32_t frameW = 1920;
    int32_t frameH = 1080;
    float fieldWGrad = 0.0f;
    float fieldHGrad = 0.0f;

    switch (m_ChannelId) {
    case 0x80:  // TV
        frameW = 1920;
        frameH = 1080;
        { QMutexLocker lock(&m_tvFieldMutex);
            fieldWGrad = m_tvFieldWGrad;
            fieldHGrad = m_tvFieldHGrad; }
        break;
    case 0x81:  // IR
        frameW = 640;
        frameH = 512;
        { QMutexLocker lock(&m_tvFieldMutex);
            fieldWGrad = m_irFieldWGrad;
            fieldHGrad = m_irFieldHGrad; }
        break;
    default:
        return;
    }

    // ═══ Новый контур: vdb_vrt_tracker_adapter ═══
    // Кормим рамкой и выходим — СТАРЫЙ Pid-контур ниже не выполняется.
    // Расчёт и отправка скорости — в adapterStep() (таймер m_adapterStepMs).
    if (m_useAdapter && m_pTrackerAdapter) {
        TrackerAdapter::PldTrackerStatus st{};
        // Конвенция адаптера: dist = ЛЕВЫЙ ВЕРХНИЙ угол рамки в [0;1]
        st.distX = (static_cast<float>(cx_out) - 0.5f * static_cast<float>(w_out))
                   / static_cast<float>(frameW);
        st.distY = (static_cast<float>(cy_out) - 0.5f * static_cast<float>(h_out))
                   / static_cast<float>(frameH);
        st.sizeX = static_cast<float>(w_out) / static_cast<float>(frameW);
        st.sizeY = static_cast<float>(h_out) / static_cast<float>(frameH);
        st.fov   = fieldWGrad;
        st.fovY  = fieldHGrad;   // вертикальный FOV — корректный масштаб ошибки тангажа
        st.curChannel = (m_ChannelId == 0x81) ? Channel::TPV : Channel::TV;
        st.trackerPrepareScoreNorm      = 0.0f;   // метрики готовности у нашего трекера нет
        st.enableSlowdownByPrepareScore = 0;

        m_pTrackerAdapter->startTracking();       // идемпотентно: рестарт после потери
        m_pTrackerAdapter->setTrackerStatusUpdate(st);
        return;
    }

    // ═══ СТАРЫЙ контур (m_useAdapter == false) ═══
    const int32_t wCenter = frameW / 2;
    const int32_t hCenter = frameH / 2;

    const int32_t errorWPix = (static_cast<int32_t>(cx_out) - wCenter);
    const int32_t errorHPix = (static_cast<int32_t>(cy_out) - hCenter);

    // ★ СТАРОЕ масштабирование — НЕ МЕНЯТЬ ★
    const float kW = -50.0f;
    const float kH = -50.0f;

    const float errorWGrad = ((fieldWGrad / frameW) * errorWPix) * kW;
    const float errorHGrad = ((fieldHGrad / frameH) * errorHPix) * kH;

    const float pidW = m_pPidW->getPidOutput(errorWGrad);
    const float pidH = m_pPidH->getPidOutput(errorHGrad);

    // ★ Компенсация дрейфа как bias ★
    m_lockCompensateGyroDrift = true;

    if (m_compensateGyroDrift) {
        m_pHost->setControlSpeed(pidW + m_yawCorr, pidH + m_pitchCorr);
    } else {
        m_pHost->setControlSpeed(pidW, pidH);
    }
}

void GyroPlatformHandler::setTvField( const float tvFieldWGrad, const float tvFieldHGrad)
{
    // const float k = 2.0f;

    // {
    //     QMutexLocker mutex( &m_tvFieldMutex);
    //     if( qFuzzyCompare( tvFieldWGrad, m_tvFieldWGrad) == false)
    //         m_tvFieldWGrad = (tvFieldWGrad / k);

    //     if( qFuzzyCompare( tvFieldHGrad, m_tvFieldHGrad) == false)
    //         m_tvFieldHGrad = tvFieldHGrad / k;
    // }


    QMutexLocker mutex(&m_tvFieldMutex);
    m_tvFieldWGrad = tvFieldWGrad;   // 58.1° ... 0.19°
    m_tvFieldHGrad = tvFieldHGrad;   // 45.1° ... 0.14°

    // qDebug() << Q_FUNC_INFO << tvFieldHGrad << tvFieldWGrad;
}

void GyroPlatformHandler::setIrField(const float irFieldWGrad, const float irFieldHGrad)
{
    // const float k = 2.0f;

    // QMutexLocker mutex(&m_tvFieldMutex);
    // if (!qFuzzyCompare(irFieldWGrad, m_irFieldWGrad))
    //     m_irFieldWGrad = irFieldWGrad / k;

    // if (!qFuzzyCompare(irFieldHGrad, m_irFieldHGrad))
    //     m_irFieldHGrad = irFieldHGrad / k;

    QMutexLocker mutex(&m_tvFieldMutex);
    m_irFieldWGrad = irFieldWGrad;   // 26.99° ... 2.75°
    m_irFieldHGrad = irFieldHGrad;   // 21.73° ... 2.20°

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


// ═══════════════════════════════════════════════════════════════════
//  TRACKER ADAPTER (vdb_vrt_tracker_adapter)
//  Конфигурация — ТОЛЬКО через SettingsManager (settings_store.trackerCtl).
// ═══════════════════════════════════════════════════════════════════
void GyroPlatformHandler::initTrackerAdapter()
{
    if (m_pTrackerAdapter != nullptr)
        return;
    if (m_pSettings == nullptr) {
        qWarning() << Q_FUNC_INFO << "SettingsManager not set — adapter disabled";
        return;
    }

    m_useAdapter    = m_pSettings->settings_store.trackerCtl.useAdapter;
    m_adapterStepMs = static_cast<int>(m_pSettings->settings_store.trackerCtl.stepMs);
    if (!m_useAdapter)
        return;

    m_pTrackerAdapter = new TrackerAdapter(buildMultiplexors());
    m_pTrackerAdapter->setSpeedSink(this);

    qDebug() << Q_FUNC_INFO << "TrackerAdapter enabled: step" << m_adapterStepMs << "ms";
}


void GyroPlatformHandler::stopTrackerAdapter()
{
    if (m_pTimerAdapter != nullptr)
        m_pTimerAdapter->stop();

    if (m_pTrackerAdapter != nullptr)
        m_pTrackerAdapter->stopTracking();
}


// Конвертация настроек SettingsManager → структуры адаптера.
// Индексы идентичны: Channel TV=0/TPV=1, Axis X=0/Y=1, FOV wide=0/tele=1.
TrackerAdapter::ChannelsPidMultiplexors GyroPlatformHandler::buildMultiplexors() const
{
    const SettingsManager::TrackerCtlCfg &tc = m_pSettings->settings_store.trackerCtl;

    TrackerAdapter::ChannelsPidMultiplexors k{};
    for (int ch = 0; ch < int(Channel::channelsCounter); ++ch) {
        for (int ax = 0; ax < int(Axis::counterAxis); ++ax) {
            k.oneChannelK[ch].basePidK[ax].Kp     = tc.Channel[ch].Axis[ax].Kp;
            k.oneChannelK[ch].basePidK[ax].Ki     = tc.Channel[ch].Axis[ax].Ki;
            k.oneChannelK[ch].basePidK[ax].Kd     = tc.Channel[ch].Axis[ax].Kd;
            k.oneChannelK[ch].basePidK[ax].iLim   = tc.Channel[ch].Axis[ax].ILim;
            k.oneChannelK[ch].basePidK[ax].outLim = tc.Channel[ch].Axis[ax].OutLim;
        }

        const SettingsManager::FovPointCfg *fovPts[int(FOVType::fovCounter)] =
            { &tc.Channel[ch].Wide, &tc.Channel[ch].Tele };

        for (int f = 0; f < int(FOVType::fovCounter); ++f) {
            k.oneChannelK[ch].multiAxisK[f].fovVal = fovPts[f]->FovVal;
            for (int ax = 0; ax < int(Axis::counterAxis); ++ax) {
                k.oneChannelK[ch].multiAxisK[f].oneAxisPIDFov[ax].kKp    = fovPts[f]->kKp;
                k.oneChannelK[ch].multiAxisK[f].oneAxisPIDFov[ax].kKi    = fovPts[f]->kKi;
                k.oneChannelK[ch].multiAxisK[f].oneAxisPIDFov[ax].kKd    = fovPts[f]->kKd;
                k.oneChannelK[ch].multiAxisK[f].oneAxisPIDFov[ax].kKiLim = fovPts[f]->kKiLim;
            }
        }
    }
    return k;
}


// Слот таймера: периодический шаг адаптера (20 мс по умолчанию).
void GyroPlatformHandler::adapterStep()
{
    if (!m_useAdapter || m_pTrackerAdapter == nullptr)
        return;
    if (!m_tracking || m_pHost == nullptr)
        return;

    m_pTrackerAdapter->step();
}


// ITrackerSpeedSink: адаптер → гироплатформа.
// Аргументы: pitch (тангаж), yaw (курс), °/с. Ось X адаптера = yaw, Y = pitch.
// Компенсация дрейфа — аддитивным bias, как в старом контуре.
void GyroPlatformHandler::onTrackerSpeed(const float pitch, const float yaw)
{
    if (m_pHost == nullptr)
        return;

    // Пока трекер активен — обновляем lock (держит updateSpeed() выключенным)
    if (m_tracking)
        m_lockCompensateGyroDrift = true;

    if (m_compensateGyroDrift) {
        m_pHost->setControlSpeed(yaw + m_yawCorr, pitch + m_pitchCorr);
    } else {
        m_pHost->setControlSpeed(yaw, pitch);
    }
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

                m_pHost->setMode( Goen220GyroPlatformHost::Off);

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


    // ВНИМАНИЕ: connect'ы stateConnectToSerialPortChanged / stateConnectToRemoteDeviceChanged /
    // serialPortErrorStringChanged уже выполнены выше (строки ~389-421). Ранее здесь шёл
    // повторяющийся блок на те же сигналы — из-за этого каждое событие (подключение TTY,
    // потеря связи, переходы setMode(Off)/setMode(Parkin)) выполнялось ДВАЖДЫ. Ниже остаются
    // только уникальные моторные обработчики, которых в первом блоке нет.

    connect(m_pHost, &Goen220GyroPlatformHost::yawMotorStatusChanged, this,
            [this](const MotorStatus &motorStatus) {
                if (qFuzzyCompare(m_yawAngle, motorStatus.angle) == false) {
                    m_yawAngle = motorStatus.angle;

                    // ★ Сбор развёрнутых отсчётов угла для LS-регрессии ★
                    if (m_calState == CalMeasuring) {
                        calAddYawAngle(QDateTime::currentMSecsSinceEpoch(), motorStatus.angle);
                    }

                    emit motorAngleChanged(-m_pitchAngle, m_yawAngle);
                }

                if (qFuzzyCompare(m_yawSpeed, motorStatus.speed) == false) {
                    m_yawSpeed = motorStatus.speed;

                    // ★ Сбор выборок скорости для калибровки ★
                    if (m_calState == CalMeasuring) {
                        m_yawSpeedSamples.append(motorStatus.speed);
                    }

                    emit motorSpeedChanged(m_yawSpeed, m_pitchSpeed);
                }
            });

    // Внутри connect(pitchMotorStatusChanged):
    connect(m_pHost, &Goen220GyroPlatformHost::pitchMotorStatusChanged, this,
            [this](const MotorStatus &motorStatus) {
                if (qFuzzyCompare(m_pitchAngle, motorStatus.angle) == false) {
                    m_pitchAngle = motorStatus.angle;

                    // ★ Сбор развёрнутых отсчётов угла для LS-регрессии ★
                    if (m_calState == CalMeasuring) {
                        calAddPitchAngle(QDateTime::currentMSecsSinceEpoch(), motorStatus.angle);
                    }

                    emit motorAngleChanged(-m_pitchAngle, m_yawAngle);
                }

                if (qFuzzyCompare(m_pitchSpeed, motorStatus.speed) == false) {
                    m_pitchSpeed = motorStatus.speed;

                    // ★ Сбор выборок скорости для калибровки ★
                    if (m_calState == CalMeasuring) {
                        m_pitchSpeedSamples.append(motorStatus.speed);
                    }

                    emit motorSpeedChanged(m_yawSpeed, m_pitchSpeed);
                }
            });

    connect(m_pTimerForUpdateSpeed, &QTimer::timeout, this, &GyroPlatformHandler::updateSpeed);
    m_pTimerForUpdateSpeed->setSingleShot(false);
    m_pTimerForUpdateSpeed->setInterval(2000);
}

// ─────────────────────────────────────────────────────────────────────────────
// СТАРТ КАЛИБРОВКИ — фаза 1: Warmup
// ─────────────────────────────────────────────────────────────────────────────
void GyroPlatformHandler::startGyroDriftCalibration()
{
    if (m_calState != CalIdle) {
        qWarning() << Q_FUNC_INFO << "Calibration already in progress!";
        return;
    }

    qDebug() << Q_FUNC_INFO << "=== GYRO CALIBRATION START ===";

    // ═══ КРИТИЧНО: обнулить целевую скорость ДО переключения режима ═══
    m_pHost->setControlSpeed(0.0f, 0.0f);

    // Останавливаем всё активное управление
    if (m_tracking) {
        setActiveLockTracking(false, false);
    }
    m_pTimerForUpdateSpeed->stop();
    stopTrackerAdapter();

    // Переводим в стабилизацию
    m_pHost->setMode(Goen220GyroPlatformHost::Stabilization);
    m_stabilization = true;
    m_zero  = false;
    m_pohod = false;
    m_pilot = false;
    m_park  = false;

    // Отключаем компенсацию
    m_compensateGyroDrift     = false;
    m_lockCompensateGyroDrift = false;

    emit positionModeChanged(m_zero, m_pohod, m_pilot, m_park, m_stabilization);

    // Фаза 1: Warmup — ждём завершения переходного процесса
    m_calState = CalWarmup;
    m_yawSpeedSamples.clear();
    m_pitchSpeedSamples.clear();

    // Сброс накоплений многопрогонной калибровки (LS + медиана)
    m_calRunIndex = 0;
    m_calYawDriftRuns.clear();
    m_calPitchDriftRuns.clear();
    m_calYawAngleSamples.clear();
    m_calPitchAngleSamples.clear();

    qDebug() << Q_FUNC_INFO << "Phase 1: warmup" << CAL_WARMUP_MS << "ms"
             << "runs=" << CAL_NUM_RUNS << "measure=" << CAL_MEASURE_MS << "ms";
    m_pCalWarmupTimer->start(CAL_WARMUP_MS);
}


// ─────────────────────────────────────────────────────────────────────────────
// ФАЗА 2: Начало измерения (после warmup)
// ─────────────────────────────────────────────────────────────────────────────
void GyroPlatformHandler::onCalWarmupFinished()
{
    if (m_calState != CalWarmup)
        return;

    // ═══ Ещё раз обнуляем целевую скорость на случай если что-то ушло ═══
    m_pHost->setControlSpeed(0.0f, 0.0f);

    qDebug() << Q_FUNC_INFO << "Phase 2: measuring run" << (m_calRunIndex + 1)
             << "/" << CAL_NUM_RUNS << "" << CAL_MEASURE_MS << "ms";

    // Буферы отсчётов угла — чистим на каждый прогон; unwrap-цепочку стартуем заново
    m_calYawAngleSamples.clear();
    m_calPitchAngleSamples.clear();
    m_calYawPrevRaw   = m_yawAngle;
    m_calPitchPrevRaw = m_pitchAngle;

    // Стартовый угол — только для лога (LS считается по всем точкам окна)
    m_calStartYawAngle   = m_yawAngle;
    m_calStartPitchAngle = m_pitchAngle;
    m_calCurrentYawAngle   = m_yawAngle;
    m_calCurrentPitchAngle = m_pitchAngle;

    qDebug() << Q_FUNC_INFO << "Start angles: yaw=" << m_calStartYawAngle
             << "pitch=" << m_calStartPitchAngle;

    m_calStartTimeMs = QDateTime::currentMSecsSinceEpoch();
    m_calState       = CalMeasuring;

    // Запускаем таймер окончания измерения
    m_pCalDurationTimer->start(CAL_MEASURE_MS);
}


// ─────────────────────────────────────────────────────────────────────────────
// ФАЗА 3: Вычисление результата
// ─────────────────────────────────────────────────────────────────────────────
void GyroPlatformHandler::onCalMeasureFinished()
{
    if (m_calState != CalMeasuring)
        return;

    // ═══ Сбрасываем целевую скорость — измерение прогона окончено ═══
    m_pHost->setControlSpeed(0.0f, 0.0f);

    const qint64 elapsedMs = QDateTime::currentMSecsSinceEpoch() - m_calStartTimeMs;

    // LS-наклон по всем отсчётам угла этого прогона (°/s); NAN — если точек мало.
    const float yawDrift   = calSlopeDegreesPerSec(m_calYawAngleSamples);
    const float pitchDrift = calSlopeDegreesPerSec(m_calPitchAngleSamples);

    const int nYaw   = m_calYawAngleSamples.size();
    const int nPitch = m_calPitchAngleSamples.size();

    qDebug() << Q_FUNC_INFO << QString("=== RUN %1/%2 ===").arg(m_calRunIndex + 1).arg(CAL_NUM_RUNS)
             << "\n  Elapsed:      " << elapsedMs << "ms"
             << "\n  N(yaw/pitch): " << nYaw << "/" << nPitch
             << "\n  yawDrift:     " << yawDrift   << "°/s"
             << "\n  pitchDrift:   " << pitchDrift << "°/s";

    const bool enoughPoints = (nYaw >= CAL_MIN_SAMPLES) && (nPitch >= CAL_MIN_SAMPLES);
    const bool finite       = !std::isnan(yawDrift) && !std::isnan(pitchDrift);
    const bool notMoved     = std::fabs(yawDrift) <= CAL_MAX_VALID_DRIFT
                              && std::fabs(pitchDrift) <= CAL_MAX_VALID_DRIFT;

    if (!enoughPoints || !finite || !notMoved) {
        qWarning() << Q_FUNC_INFO << "Run invalid — aborting calibration."
                   << "enoughPoints=" << enoughPoints
                   << "finite=" << finite
                   << "notMoved=" << notMoved;
        m_calState    = CalIdle;
        m_calRunIndex = 0;
        m_calYawDriftRuns.clear();
        m_calPitchDriftRuns.clear();
        m_pTimerForUpdateSpeed->start();
        emit calibrationCompleted(0.0f, 0.0f, false);
        return;
    }

    // Прогон валиден — копить оценку
    m_calYawDriftRuns.append(yawDrift);
    m_calPitchDriftRuns.append(pitchDrift);
    ++m_calRunIndex;

    // Если прогонов ещё не набрано — следующий warmup → измерение
    if (m_calRunIndex < CAL_NUM_RUNS) {
        m_calState = CalWarmup;
        qDebug() << Q_FUNC_INFO << "Next run: warmup" << CAL_WARMUP_MS << "ms";
        m_pCalWarmupTimer->start(CAL_WARMUP_MS);
        return;
    }

    // ═══ Все прогоны набраны — медиана устойчива к выбросам/покачиванию ═══
    const float yawMed   = medianOf(m_calYawDriftRuns);
    const float pitchMed = medianOf(m_calPitchDriftRuns);

    m_calState    = CalIdle;
    m_calRunIndex = 0;

    // ★ ИНВЕРТИРУЕМ: drift>0 (уходит в+) → corr<0 (тянем в-) ★
    m_yawCorr   = -yawMed;
    m_pitchCorr = -pitchMed;
    m_compensateGyroDrift = true;

    qDebug() << Q_FUNC_INFO << QString("=== CALIBRATION APPLIED (median of %1 runs) ===").arg(CAL_NUM_RUNS)
             << "\n  yawDrift median:   " << yawMed   << "°/s → yawCorr="   << m_yawCorr
             << "\n  pitchDrift median: " << pitchMed << "°/s → pitchCorr=" << m_pitchCorr;

    saveCalibrationToFile(m_yawCorr, m_pitchCorr);  // ← сохраняем С ИНВЕРСИЕЙ
    m_pTimerForUpdateSpeed->start();

    emit calibrationCompleted(m_yawCorr, m_pitchCorr, true);
}




// ─────────────────────────────────────────────────────────────────────────────
// СОХРАНЕНИЕ В ФАЙЛ
// ─────────────────────────────────────────────────────────────────────────────
void GyroPlatformHandler::saveCalibrationToFile(const float yaw, const float pitch)
{
    QSettings settings(m_settingsFilePath, QSettings::IniFormat);
    settings.setValue("GyroPlatform.YawGyroDrift",   yaw);
    settings.setValue("GyroPlatform.PitchGyroDrift",  pitch);
    settings.sync();

    qDebug() << Q_FUNC_INFO << "Saved to" << m_settingsFilePath
             << "YawGyroDrift="   << yaw
             << "PitchGyroDrift=" << pitch;
}

// ─────────────────────────────────────────────────────────────────────────────
// ХЕЛПЕРЫ ОЦЕНКИ ДРЕЙФА: unwrap-накопление + LS-регрессия + медиана
// ─────────────────────────────────────────────────────────────────────────────
void GyroPlatformHandler::calAddYawAngle(qint64 tMs, float angle)
{
    if (m_calYawAngleSamples.isEmpty()) {
        m_calYawPrevRaw = angle;
        m_calYawAngleSamples.append(qMakePair(tMs, angle));
        return;
    }
    float d = angle - m_calYawPrevRaw;          // unwrap через приращение
    if (d >  180.0f) d -= 360.0f;
    if (d < -180.0f) d += 360.0f;
    const float prev = m_calYawAngleSamples.last().second;
    m_calYawAngleSamples.append(qMakePair(tMs, prev + d));
    m_calYawPrevRaw = angle;
}

void GyroPlatformHandler::calAddPitchAngle(qint64 tMs, float angle)
{
    if (m_calPitchAngleSamples.isEmpty()) {
        m_calPitchPrevRaw = angle;
        m_calPitchAngleSamples.append(qMakePair(tMs, angle));
        return;
    }
    float d = angle - m_calPitchPrevRaw;
    if (d >  180.0f) d -= 360.0f;
    if (d < -180.0f) d += 360.0f;
    const float prev = m_calPitchAngleSamples.last().second;
    m_calPitchAngleSamples.append(qMakePair(tMs, prev + d));
    m_calPitchPrevRaw = angle;
}

// МНК: slope = Σ(t−t̄)(a−ā) / Σ(t−t̄)². Оптимальная оценка наклона при шуме угла.
// Возвращает °/s либо NAN, если точек недостаточно / вырожденная выборка.
float GyroPlatformHandler::calSlopeDegreesPerSec(const QVector<QPair<qint64,float>> &s)
{
    const int n = s.size();
    if (n < 2)
        return std::numeric_limits<float>::quiet_NaN();

    const qint64 t0 = s.first().first;
    double sumT = 0.0, sumA = 0.0;
    for (const auto &p : s) {
        sumT += static_cast<double>(p.first - t0) / 1000.0;   // t в секундах
        sumA += static_cast<double>(p.second);
    }
    const double mt = sumT / n;
    const double ma = sumA / n;

    double num = 0.0, den = 0.0;
    for (const auto &p : s) {
        const double dt = static_cast<double>(p.first - t0) / 1000.0 - mt;
        num += dt * (static_cast<double>(p.second) - ma);
        den += dt * dt;
    }
    if (den <= 0.0)
        return std::numeric_limits<float>::quiet_NaN();
    return static_cast<float>(num / den);
}

float GyroPlatformHandler::medianOf(QVector<float> v)
{
    if (v.isEmpty())
        return std::numeric_limits<float>::quiet_NaN();
    std::sort(v.begin(), v.end());
    const int n = v.size();
    if (n % 2 == 1)
        return v[n / 2];
    return 0.5f * (v[n / 2 - 1] + v[n / 2]);
}


