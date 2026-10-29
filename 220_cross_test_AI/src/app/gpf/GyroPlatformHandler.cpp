
#include "GyroPlatformHandler.h"

#include "Goen220GyroPlatformHost.h"
#include "Pid.h"

#include <QDebug>
#include <QTimer>
#include <QCoreApplication>
#include <QDateTime>

GyroPlatformHandler::GyroPlatformHandler(Goen220GyroPlatformHost *const host, QObject *parent)
    : QObject(parent), m_pHost(host), m_zero(false), m_pohod(false), m_pilot(false), m_park(false),
      m_stabilization(false), m_yawAngle(0.0f), m_pitchAngle(0.0f), m_yawSpeed(0.0f),
      m_pitchSpeed(0.0f), m_tracking(false), m_tvFieldWGrad(0.0f), m_tvFieldHGrad(0.0f),
      m_enableStabAfterZero(false), m_pPidW(new Pid(0.05f, 0.0f, 0.0f)),
      m_pPidH(new Pid(0.025f, 0.0f, 0.0f)), m_pTimerForUpdateSpeed(new QTimer()),
      m_compensateGyroDrift(false), m_lockCompensateGyroDrift(false), m_yawCorr(-0.355f),
      m_pitchCorr(-0.530f)
{
    initGyroPlatform();

    // Таймер калибровки
    m_pCalWarmupTimer = new QTimer(this);
    m_pCalWarmupTimer->setSingleShot(true);

    connect(m_pCalWarmupTimer, &QTimer::timeout, this, &GyroPlatformHandler::onCalWarmupFinished);

    m_pCalDurationTimer = new QTimer(this);
    m_pCalDurationTimer->setSingleShot(true);

    connect(m_pCalDurationTimer, &QTimer::timeout, this,
            &GyroPlatformHandler::onCalMeasureFinished);

    // Путь к конфигурационному файлу по умолчанию
    m_settingsFilePath = QCoreApplication::applicationDirPath() + "/config.ini";
} /* GyroPlatformHandler::GyroPlatformHandler */

GyroPlatformHandler::~GyroPlatformHandler()
{
    if (m_pTimerForUpdateSpeed)
        delete m_pTimerForUpdateSpeed;

    if (m_pPidH)
        delete m_pPidH;

    if (m_pPidW)
        delete m_pPidW;
} /* GyroPlatformHandler::~GyroPlatformHandler */

void GyroPlatformHandler::setDrivePositionMode(const bool zero, const bool pohod, const bool pilot,
                                               const bool park)
{
    if (m_pHost == nullptr)
        return;

    m_pTimerForUpdateSpeed->stop();

    if (zero)
    {
        m_pHost->setMode(Goen220GyroPlatformHost::RotaryPlatform);
        m_pHost->setControlPosition(0.0f, 0.0f);
        // if( m_stabilization)
        // {
        //     m_enableStabAfterZero = true;
        // }
    } /* if */

    if (pohod)
    {
        m_pHost->setControlPosition(-170.0f, 0.0f);
    } /* if */

    if (pilot)
    {
        m_pHost->setControlPosition(0.0f, 90.0f);
    } /* if */

    if (park)
    {
        //        m_pHost->setControlPosition( 0.0f, 90.0f);
        m_pHost->setMode(Goen220GyroPlatformHost::Parkin);
    } /* if */
    else
        m_pHost->setMode(Goen220GyroPlatformHost::RotaryPlatform);

    m_zero = zero;
    m_pohod = pohod;
    m_pilot = pilot;
    m_park = park;
    m_stabilization = false;

    emit positionModeChanged(m_zero, m_pohod, m_pilot, m_park, m_stabilization);
} /* GyroPlatformHandler::setDrivePositionMode */

void GyroPlatformHandler::setStabilization(const bool enable)
{
    if (m_pHost == nullptr)
        return;

    if (enable)
        m_pHost->setMode(Goen220GyroPlatformHost::Stabilization);
    else
        m_pHost->setMode(Goen220GyroPlatformHost::RotaryPlatform);

    // m_pHost->setMode( Goen220GyroPlatformHost::RotaryPlatform);

    m_zero = false;
    m_pohod = false;
    m_pilot = false;
    m_park = false;
    m_stabilization = enable;

    if (enable)
    {
        m_lockCompensateGyroDrift = false;
        updateSpeed();
        m_pTimerForUpdateSpeed->start();
    } /* if */
    else
    {
        m_pTimerForUpdateSpeed->stop();
    } /* else */

    emit positionModeChanged(m_zero, m_pohod, m_pilot, m_park, m_stabilization);
} /* GyroPlatformHandler::setStabilization */

void GyroPlatformHandler::procSetTrackerChannel(quint8 ch_id)
{
    m_ChannelId = ch_id;
} /* GyroPlatformHandler::procSetTrackerChannel */

void GyroPlatformHandler::setControlSpeed(const float z, const float x)
{
    if (m_pHost == nullptr)
        return;

    if (m_stabilization)
    {
        // ★ Выбор FOV по активному каналу ★
        float fieldWGrad = 58.1f;
        {
            QMutexLocker lock(&m_tvFieldMutex);
            switch (m_ChannelId)
            {
            case 0x80: // TV
                fieldWGrad = m_tvFieldWGrad;
                break;
            case 0x81: // IR
                fieldWGrad = m_irFieldWGrad;
                break;
            default:
                fieldWGrad = m_tvFieldWGrad;
                break;
            } /* switch */
        } /* block */

        if (m_compensateGyroDrift)
        {
            const bool isZero = (z > -0.01f && z < 0.01f && x > -0.01f && x < 0.01f);
            m_lockCompensateGyroDrift = !isZero;

            m_pHost->setControlSpeed((-x * fieldWGrad) + m_yawCorr, (z * fieldWGrad) + m_pitchCorr);
        } /* if */
        else
        {
            m_lockCompensateGyroDrift = false;
            m_pHost->setControlSpeed((-x * fieldWGrad), (z * fieldWGrad));
        } /* else */
    } /* if */
    else
    {
        if (x != 0.0f || z != 0.0f)
        {
            m_yawAngle -= x;
            m_pitchAngle += z;
            m_pHost->setControlPosition(m_yawAngle, m_pitchAngle);
        } /* if */
    } /* else */
} /* GyroPlatformHandler::setControlSpeed */

void GyroPlatformHandler::setControlPositionRel(const float z, const float x)
{
    if (m_pHost == nullptr)
        return;

    qDebug() << Q_FUNC_INFO << z << x;
    m_pHost->setControlTracking(z, x, YAW_ACCEL_LIMIT, PITCH_ACCEL_LIMIT);
} /* GyroPlatformHandler::setControlPositionRel */

void GyroPlatformHandler::setControlPositionAbs(const float z, const float x)
{
    if (m_pHost == nullptr)
        return;

    qDebug() << Q_FUNC_INFO << z << x;
    m_pHost->setControlPosition(z, x);
} /* GyroPlatformHandler::setControlPositionAbs */

void GyroPlatformHandler::turnOffMotors()
{
    qDebug() << Q_FUNC_INFO;
    m_pTimerForUpdateSpeed->stop();
    m_pHost->setMode(Goen220GyroPlatformHost::Off);
} /* GyroPlatformHandler::turnOffMotors */
void GyroPlatformHandler::setActiveLockTracking(bool active, bool tracking)
{
    if (m_tracking != tracking)
    {
        m_tracking = tracking;

        if (tracking == false)
        {
            // Снимаем lock — updateSpeed() снова активен
            m_lockCompensateGyroDrift = false;

            // ПОДАЁМ КОМПЕНСАЦИЮ
            if (m_compensateGyroDrift)
                m_pHost->setControlSpeed(m_yawCorr, m_pitchCorr);
            else
                m_pHost->setControlSpeed(0.0f, 0.0f);

        } /* if */
        else
        {
            // Трекинг начался — блокируем updateSpeed()
            m_lockCompensateGyroDrift = true;
        } /* else */
    } /* if */
} /* GyroPlatformHandler::setActiveLockTracking */
void GyroPlatformHandler::updateTrackerObject(uint32_t cx_out, uint32_t cy_out, uint32_t w_out,
                                              uint32_t h_out)
{
    if (m_tracking == false)
        return;

    // Потеря объекта — держим компенсацию
    if (cx_out == 0 && cy_out == 0)
    {
        if (m_compensateGyroDrift)
            m_pHost->setControlSpeed(m_yawCorr, m_pitchCorr);
        else
            m_pHost->setControlSpeed(0.0f, 0.0f);
        return;
    } /* if */

    int32_t frameW = 1920;
    int32_t frameH = 1080;
    float fieldWGrad = 0.0f;
    float fieldHGrad = 0.0f;

    switch (m_ChannelId)
    {
    case 0x80: // TV
        frameW = 1920;
        frameH = 1080;
        {
            QMutexLocker lock(&m_tvFieldMutex);
            fieldWGrad = m_tvFieldWGrad;
            fieldHGrad = m_tvFieldHGrad;
        } /* case */
        break;
    case 0x81: // IR
        frameW = 640;
        frameH = 512;
        {
            QMutexLocker lock(&m_tvFieldMutex);
            fieldWGrad = m_irFieldWGrad;
            fieldHGrad = m_irFieldHGrad;
        } /* case */
        break;
    default:
        return;
    } /* switch */

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

    if (m_compensateGyroDrift)
    {
        m_pHost->setControlSpeed(pidW + m_yawCorr, pidH + m_pitchCorr);
    } /* if */
    else
    {
        m_pHost->setControlSpeed(pidW, pidH);
    } /* else */
} /* GyroPlatformHandler::updateTrackerObject */

void GyroPlatformHandler::setTvField(const float tvFieldWGrad, const float tvFieldHGrad)
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
    m_tvFieldWGrad = tvFieldWGrad; // 58.1° ... 0.19°
    m_tvFieldHGrad = tvFieldHGrad; // 45.1° ... 0.14°

    // qDebug() << Q_FUNC_INFO << tvFieldHGrad << tvFieldWGrad;
} /* GyroPlatformHandler::setTvField */

void GyroPlatformHandler::setIrField(const float irFieldWGrad, const float irFieldHGrad)
{
    // const float k = 2.0f;

    // QMutexLocker mutex(&m_tvFieldMutex);
    // if (!qFuzzyCompare(irFieldWGrad, m_irFieldWGrad))
    //     m_irFieldWGrad = irFieldWGrad / k;

    // if (!qFuzzyCompare(irFieldHGrad, m_irFieldHGrad))
    //     m_irFieldHGrad = irFieldHGrad / k;

    QMutexLocker mutex(&m_tvFieldMutex);
    m_irFieldWGrad = irFieldWGrad; // 26.99° ... 2.75°
    m_irFieldHGrad = irFieldHGrad; // 21.73° ... 2.20°

} /* GyroPlatformHandler::setIrField */

void GyroPlatformHandler::enableCompensateGyroDrift(const bool enable)
{
    m_compensateGyroDrift = enable;
} /* GyroPlatformHandler::enableCompensateGyroDrift */

void GyroPlatformHandler::setGyroDrift(const float yaw, const float pitch)
{
    m_yawCorr = yaw;
    m_pitchCorr = pitch;
} /* GyroPlatformHandler::setGyroDrift */

void GyroPlatformHandler::updateSpeed()
{
    if (m_lockCompensateGyroDrift)
        return;

    //    qDebug() << Q_FUNC_INFO;
    //  курс (Yaw) Z, тангаж (Pitch) X
    if (m_compensateGyroDrift)
        m_pHost->setControlSpeed(m_yawCorr, m_pitchCorr);
} /* GyroPlatformHandler::updateSpeed */

void GyroPlatformHandler::initGyroPlatform()
{
    if (m_pHost == nullptr)
        return;

    connect(m_pHost, &Goen220GyroPlatformHost::stateConnectToSerialPortChanged, this,
            [this](const bool con)
            {
                qDebug() << "GSP:  Подключение к TTY: "
                         << (con ? QString::fromUtf8("Установлено")
                                 : QString::fromUtf8("Отсутствует"));
            } /* lambda */
    );

    connect(m_pHost, &Goen220GyroPlatformHost::stateConnectToRemoteDeviceChanged, this,
            [this](const bool con)
            {
                qDebug() << "GSP: Связь с ГироПлатформой: "
                         << (con ? QString::fromUtf8("Установлена")
                                 : QString::fromUtf8("Потеряна"));
                if (con)
                {

                    m_pHost->setMode(Goen220GyroPlatformHost::Off);

                    m_park = true;
                    m_pHost->setMode(Goen220GyroPlatformHost::Parkin);
                } /* if */
            } /* lambda */
    );

    connect(m_pHost, &Goen220GyroPlatformHost::serialPortErrorStringChanged, this,
            [this]()
            {
                const QString error = m_pHost->getSerialPortErrorString();
                if (error.isEmpty() == true || error.isNull() == true)
                    return;

                qWarning() << "********************************************************************"
                              "**** SERIAL PORT ERROR: "
                           << error;
                m_pHost->connect(false);
            } /* lambda */
    );

    if (m_pHost == nullptr)
        return;

    connect(m_pHost, &Goen220GyroPlatformHost::stateConnectToSerialPortChanged, this,
            [this](const bool con)
            {
                qDebug() << "GSP:  Подключение к TTY: "
                         << (con ? QString::fromUtf8("Установлено")
                                 : QString::fromUtf8("Отсутствует"));
            } /* lambda */);

    connect(m_pHost, &Goen220GyroPlatformHost::stateConnectToRemoteDeviceChanged, this,
            [this](const bool con)
            {
                qDebug() << "GSP: Связь с ГироПлатформой: "
                         << (con ? QString::fromUtf8("Установлена")
                                 : QString::fromUtf8("Потеряна"));
                if (con)
                {
                    m_pHost->setMode(Goen220GyroPlatformHost::Off);
                    m_park = true;
                    m_pHost->setMode(Goen220GyroPlatformHost::Parkin);
                } /* if */
            } /* lambda */);

    connect(m_pHost, &Goen220GyroPlatformHost::serialPortErrorStringChanged, this, [this]()
            {
                const QString error = m_pHost->getSerialPortErrorString();
                if (error.isEmpty() || error.isNull())
                    return;
                qWarning() << "SERIAL PORT ERROR: " << error;
                m_pHost->connect(false);
            } /* lambda */);

    connect(m_pHost, &Goen220GyroPlatformHost::yawMotorStatusChanged, this,
            [this](const MotorStatus &motorStatus)
            {
                if (qFuzzyCompare(m_yawAngle, motorStatus.angle) == false)
                {
                    m_yawAngle = motorStatus.angle;

                    // ★ Фиксация текущего угла для калибровки ★
                    if (m_calState == CalMeasuring)
                    {
                        m_calCurrentYawAngle = motorStatus.angle;
                    } /* if */

                    emit motorAngleChanged(-m_pitchAngle, m_yawAngle);
                } /* if */

                if (qFuzzyCompare(m_yawSpeed, motorStatus.speed) == false)
                {
                    m_yawSpeed = motorStatus.speed;

                    // ★ Сбор выборок скорости для калибровки ★
                    if (m_calState == CalMeasuring)
                    {
                        m_yawSpeedSamples.append(motorStatus.speed);
                    } /* if */

                    emit motorSpeedChanged(m_yawSpeed, m_pitchSpeed);
                } /* if */
            } /* lambda */);

    // Внутри connect(pitchMotorStatusChanged):

    connect(m_pHost, &Goen220GyroPlatformHost::pitchMotorStatusChanged, this,
            [this](const MotorStatus &motorStatus)
            {
                if (qFuzzyCompare(m_pitchAngle, motorStatus.angle) == false)
                {
                    m_pitchAngle = motorStatus.angle;

                    // ★ Фиксация текущего угла для калибровки ★
                    if (m_calState == CalMeasuring)
                    {
                        m_calCurrentPitchAngle = motorStatus.angle;
                    } /* if */

                    emit motorAngleChanged(-m_pitchAngle, m_yawAngle);
                } /* if */

                if (qFuzzyCompare(m_pitchSpeed, motorStatus.speed) == false)
                {
                    m_pitchSpeed = motorStatus.speed;

                    // ★ Сбор выборок скорости для калибровки ★
                    if (m_calState == CalMeasuring)
                    {
                        m_pitchSpeedSamples.append(motorStatus.speed);
                    } /* if */

                    emit motorSpeedChanged(m_yawSpeed, m_pitchSpeed);
                } /* if */
            } /* lambda */);

    connect(m_pTimerForUpdateSpeed, &QTimer::timeout, this, &GyroPlatformHandler::updateSpeed);

    m_pTimerForUpdateSpeed->setSingleShot(false);
    m_pTimerForUpdateSpeed->setInterval(2000);
} /* GyroPlatformHandler::initGyroPlatform */

// ─────────────────────────────────────────────────────────────────────────────
// СТАРТ КАЛИБРОВКИ — фаза 1: Warmup
// ─────────────────────────────────────────────────────────────────────────────
void GyroPlatformHandler::startGyroDriftCalibration()
{
    if (m_calState != CalIdle)
    {
        qWarning() << Q_FUNC_INFO << "Calibration already in progress!";
        return;
    } /* if */

    qDebug() << Q_FUNC_INFO << "=== GYRO CALIBRATION START ===";

    // ═══ КРИТИЧНО: обнулить целевую скорость ДО переключения режима ═══
    m_pHost->setControlSpeed(0.0f, 0.0f);

    // Останавливаем всё активное управление
    if (m_tracking)
    {
        setActiveLockTracking(false, false);
    } /* if */
    m_pTimerForUpdateSpeed->stop();

    // Переводим в стабилизацию
    m_pHost->setMode(Goen220GyroPlatformHost::Stabilization);
    m_stabilization = true;
    m_zero = false;
    m_pohod = false;
    m_pilot = false;
    m_park = false;

    // Отключаем компенсацию
    m_compensateGyroDrift = false;
    m_lockCompensateGyroDrift = false;

    emit positionModeChanged(m_zero, m_pohod, m_pilot, m_park, m_stabilization);

    // Фаза 1: Warmup — ждём завершения переходного процесса
    m_calState = CalWarmup;
    m_yawSpeedSamples.clear();
    m_pitchSpeedSamples.clear();

    qDebug() << Q_FUNC_INFO << "Phase 1: warmup" << CAL_WARMUP_MS << "ms";
    m_pCalWarmupTimer->start(CAL_WARMUP_MS);
} /* GyroPlatformHandler::startGyroDriftCalibration */

// ─────────────────────────────────────────────────────────────────────────────
// ФАЗА 2: Начало измерения (после warmup)
// ─────────────────────────────────────────────────────────────────────────────
void GyroPlatformHandler::onCalWarmupFinished()
{
    if (m_calState != CalWarmup)
        return;

    // ═══ Ещё раз обнуляем целевую скорость на случай если что-то ушло ═══
    m_pHost->setControlSpeed(0.0f, 0.0f);

    qDebug() << Q_FUNC_INFO << "Phase 2: measuring" << CAL_MEASURE_MS << "ms";

    // Фиксируем НАЧАЛЬНЫЙ угол — уже после переходного процесса!
    m_calStartYawAngle = m_yawAngle;
    m_calStartPitchAngle = m_pitchAngle;
    m_calCurrentYawAngle = m_yawAngle;
    m_calCurrentPitchAngle = m_pitchAngle;

    qDebug() << Q_FUNC_INFO << "Start angles: yaw=" << m_calStartYawAngle
             << "pitch=" << m_calStartPitchAngle;

    m_calStartTimeMs = QDateTime::currentMSecsSinceEpoch();
    m_calState = CalMeasuring;

    // Запускаем таймер окончания измерения
    m_pCalDurationTimer->start(CAL_MEASURE_MS);
} /* GyroPlatformHandler::onCalWarmupFinished */

// ─────────────────────────────────────────────────────────────────────────────
// ФАЗА 3: Вычисление результата
// ─────────────────────────────────────────────────────────────────────────────
void GyroPlatformHandler::onCalMeasureFinished()
{
    if (m_calState != CalMeasuring)
        return;

    m_calState = CalIdle;

    // ═══ Сбрасываем целевую скорость — измерение окончено ═══
    m_pHost->setControlSpeed(0.0f, 0.0f);

    const qint64 elapsedMs = QDateTime::currentMSecsSinceEpoch() - m_calStartTimeMs;
    const float durationS = static_cast<float>(elapsedMs) / 1000.0f;

    if (durationS < 1.0f)
    {
        qWarning() << Q_FUNC_INFO << "Duration too short:" << elapsedMs << "ms";
        emit calibrationCompleted(0.0f, 0.0f, false);
        return;
    } /* if */

    float deltaYaw = m_calCurrentYawAngle - m_calStartYawAngle;
    float deltaPitch = m_calCurrentPitchAngle - m_calStartPitchAngle;

    // Wrap-around ±180°
    if (deltaYaw > 180.0f)
        deltaYaw -= 360.0f;
    if (deltaYaw < -180.0f)
        deltaYaw += 360.0f;
    if (deltaPitch > 180.0f)
        deltaPitch -= 360.0f;
    if (deltaPitch < -180.0f)
        deltaPitch += 360.0f;

    const float yawDrift = deltaYaw / durationS;
    const float pitchDrift = deltaPitch / durationS;

    qDebug() << Q_FUNC_INFO << "=== CALIBRATION RESULT ==="
             << "\n  Elapsed:      " << elapsedMs << "ms"
             << "\n  Δyaw:         " << deltaYaw << "° → drift=" << yawDrift << "°/s"
             << "\n  Δpitch:       " << deltaPitch << "° → drift=" << pitchDrift << "°/s";

    // Sanity check
    const float MAX_VALID_DRIFT = 5.0f;
    if (std::fabs(yawDrift) > MAX_VALID_DRIFT || std::fabs(pitchDrift) > MAX_VALID_DRIFT)
    {
        qWarning() << Q_FUNC_INFO << "Drift too large — platform moved! Aborting.";
        emit calibrationCompleted(0.0f, 0.0f, false);
        return;
    } /* if */

    // ★ ИНВЕРТИРУЕМ: drift>0 (уходит в+) → corr<0 (тянем в-) ★
    m_yawCorr = -yawDrift;
    m_pitchCorr = -pitchDrift;
    m_compensateGyroDrift = true;

    qDebug() << Q_FUNC_INFO << "=== APPLIED ==="
             << "yawCorr=" << m_yawCorr << "pitchCorr=" << m_pitchCorr;

    saveCalibrationToFile(m_yawCorr, m_pitchCorr); // ← сохраняем С ИНВЕРСИЕЙ
    m_pTimerForUpdateSpeed->start();

    emit calibrationCompleted(m_yawCorr, m_pitchCorr, true);
} /* GyroPlatformHandler::onCalMeasureFinished */

// ─────────────────────────────────────────────────────────────────────────────
// СОХРАНЕНИЕ В ФАЙЛ
// ─────────────────────────────────────────────────────────────────────────────
void GyroPlatformHandler::saveCalibrationToFile(const float yaw, const float pitch)
{
    QSettings settings(m_settingsFilePath, QSettings::IniFormat);
    settings.setValue("GyroPlatform.YawGyroDrift", yaw);
    settings.setValue("GyroPlatform.PitchGyroDrift", pitch);
    settings.sync();

    qDebug() << Q_FUNC_INFO << "Saved to" << m_settingsFilePath << "YawGyroDrift=" << yaw
             << "PitchGyroDrift=" << pitch;
} /* GyroPlatformHandler::saveCalibrationToFile */
