#ifndef GYRO_PLATFORM_HANDLER_H
#define GYRO_PLATFORM_HANDLER_H

#include <QMutex>
#include <QObject>
#include <QPair>
#include <QSettings>
#include <QTimer>
#include <QVector>

#define YAW_ACCEL_LIMIT (20.0f)
#define PITCH_ACCEL_LIMIT (20.0f)

// Параметры калибровки
#define CALIBRATION_DURATION_MS 3000 // Длительность измерения дрейфа, мс
#define CALIBRATION_SAMPLE_MS 50     // Интервал выборки, мс

class Goen220GyroPlatformHost;
class Pid;

class QTimer;

class GyroPlatformHandler : public QObject
{
    Q_OBJECT

  public:
    explicit GyroPlatformHandler(Goen220GyroPlatformHost *const host, QObject *parent = nullptr);
    ~GyroPlatformHandler();

  signals:
    void positionModeChanged(const bool zero, const bool pohod, const bool pilot, const bool park, const bool stabilization);

    void motorAngleChanged(const float z, const float x);
    void motorSpeedChanged(const float z, const float x);

    /// Сигнал завершения калибровки. Параметры: yaw_drift, pitch_drift, success
    void calibrationCompleted(const float yawDrift, const float pitchDrift, const bool success);

  public slots:
    void setDrivePositionMode(const bool zero, const bool pohod, const bool pilot, const bool park);
    void setStabilization(const bool enable);
    void setControlSpeed(const float z, const float x);
    void setControlPositionRel(const float z, const float x);
    void setControlPositionAbs(const float z, const float x);

    void turnOffMotors();

    void setActiveLockTracking(bool active, bool tracking);
    void updateTrackerObject(uint32_t cx_out, uint32_t cy_out, uint32_t w_out, uint32_t h_out);

    void setTvField(const float tvFieldWGrad, const float tvFieldHGrad);
    void setIrField(const float irFieldWGrad, const float irFieldHGrad);
    void enableCompensateGyroDrift(const bool enable);
    void setGyroDrift(const float yaw, const float pitch);
    void procSetTrackerChannel(quint8 ch_id);

    /// Запуск калибровки дрейфа гироплатформы.
    /// Платформа должна быть неподвижна (не управляется оператором).
    void startGyroDriftCalibration();

    void setSettingsFilePath(const QString &path)
    {
        m_settingsFilePath = path;
    } // setSettingsFilePath

  private slots:
    void updateSpeed();
    void onCalWarmupFinished();
    void onCalMeasureFinished();

  private:
    void initGyroPlatform();
    void saveCalibrationToFile(const float yaw, const float pitch);

  private:
    Goen220GyroPlatformHost *const m_pHost;

    bool m_zero;
    bool m_pohod;
    bool m_pilot;
    bool m_park;
    bool m_stabilization;

    float m_yawAngle;
    float m_pitchAngle;

    float m_yawSpeed;
    float m_pitchSpeed;

    bool m_tracking;
    float m_tvFieldWGrad;
    float m_tvFieldHGrad;
    float m_irFieldWGrad;
    float m_irFieldHGrad;

    bool m_enableStabAfterZero;

    QMutex m_tvFieldMutex;

    Pid *const m_pPidW;
    Pid *const m_pPidH;

    QTimer *m_pTimerForUpdateSpeed;

    bool m_compensateGyroDrift;
    bool m_lockCompensateGyroDrift;

    int m_ChannelId = 0;

    float m_yawCorr;
    float m_pitchCorr;

    // ─── Калибровка дрейфа ────────────────────────────────────────
    enum CalibrationState
    {
        CalIdle,
        CalWarmup,    // Ждём завершения переходного процесса
        CalMeasuring, // Реальное измерение
    }; // enum CalibrationState

    CalibrationState m_calState = CalIdle;
    QTimer *m_pCalWarmupTimer = nullptr;   // Пауза 2 сек
    QTimer *m_pCalDurationTimer = nullptr; // Измерение

    float m_calStartYawAngle = 0.0f;
    float m_calStartPitchAngle = 0.0f;
    float m_calCurrentYawAngle = 0.0f;
    float m_calCurrentPitchAngle = 0.0f;

    qint64 m_calStartTimeMs = 0;

    // Скорости моторов (более устойчивая метрика)
    QVector<float> m_yawSpeedSamples;
    QVector<float> m_pitchSpeedSamples;

    // ── Оценка дрейфа через линейную регрессию (LS) по всем отсчётам угла ──
    // Endpoint-разность (конец−начало)/T выбрасывает всё окно и целиком наследует
    // шум двух крайних точек → остаток «сотые °/s», интегрируемый в десятки градусов.
    // LS-наклон использует все N точек окна: σ_slope ≈ σ_φ·√(12/(N·T²)) — в разы точнее.
    // Пары (tMs, развернутый угол). Разворот (unwrap) убирает скачок на ±180°.
    QVector<QPair<qint64, float>> m_calYawAngleSamples;
    QVector<QPair<qint64, float>> m_calPitchAngleSamples;
    float m_calYawPrevRaw = 0.0f; // для unwrap-цепочки прогона
    float m_calPitchPrevRaw = 0.0f;

    int m_calRunIndex = 0;            // номер текущего прогона
    QVector<float> m_calYawDriftRuns; // оценки дрейфа по прогонам
    QVector<float> m_calPitchDriftRuns;

    static constexpr int CAL_WARMUP_MS = 2000;         // Пауза для стабилизации
    static constexpr int CAL_MEASURE_MS = 5000;        // Длительность измерения одного прогона
    static constexpr int CAL_NUM_RUNS = 2;             // Кол-во прогонов (итог — медиана)
    static constexpr int CAL_MIN_SAMPLES = 20;         // Минимум точек для надёжного LS
    static constexpr float CAL_MAX_VALID_DRIFT = 5.0f; // Порог «платформу сдвинули»

    // Хелперы калибровки
    void calAddYawAngle(qint64 tMs, float angle);                               // unwrap + append (yaw)
    void calAddPitchAngle(qint64 tMs, float angle);                             // unwrap + append (pitch)
    static float calSlopeDegreesPerSec(const QVector<QPair<qint64, float>> &s); // LS-наклон, °/s
    static float medianOf(QVector<float> v);

    QString m_settingsFilePath;
}; // class GyroPlatformHandler

#endif
