#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QVariant>
#include <QMetaType>
#include <qsettings.h>

class SettingsManager : public QObject
{
  private:
    QString settings_file_name;

    typedef struct
    {
        QString ip_host;
        qint32 udp_port_host;

        QString ip_dev;
        qint32 udp_port_dev;
        qint32 mpeg_ts_port;

        bool mpeg_ts_en;
        bool mpeg_ts_telemetry_en;

        // --- SRT transport (only when mpeg_ts_use_srt == true) ---
        bool mpeg_ts_use_srt;
        qint32 mpeg_ts_srt_latency_ms; // 20..5000 ms
        bool mpeg_ts_srt_tlpktdrop;
        qint32 mpeg_ts_srt_ts_packet_count; // 1..7, payload = count * 188

        // --- SRT FEC (only when mpeg_ts_use_srt == true) ---
        bool mpeg_ts_srt_fec_enabled;
        qint32 mpeg_ts_srt_fec_cols; // 2..255
        qint32 mpeg_ts_srt_fec_rows; // 1..255
        QString mpeg_ts_srt_fec_layout; // "staircase" | "even"
        QString mpeg_ts_srt_fec_arq; // "always" | "onreq" | "never"

        // --- Camera watchdog / check ---
        qint32 tv_cam_first_frame_timeout_ms; // 1000..60000, wait for first TV frame (ms)
        qint32 ir_cam_first_frame_timeout_ms; // 1000..60000, wait for first IR frame (ms)
        qint32 cam_frame_retry_timeout_ms; // 500..30000, per-attempt timeout in v4l2ctl check (ms)
        qint32 cam_frame_restart_count; // 0..10, number of retries before giving up

        // --- TV Cam ---
        QString tv_cam_video_device;
        qint32 tv_cam_width;
        qint32 tv_cam_height;
        qint32 tv_cam_timeout;
        qint32 tv_cam_bpp;
        bool tv_cam_en;
        bool tv_cam_flip_180;

        QString tv_cam_uart_port;
        qint32 tv_cam_uart_baudrate;
        QString tv_cam_enc_mode;
        qint32 tv_cam_enc_target_bps;
        qint32 tv_cam_target_fps;

        // --- IR Cam ---
        QString ir_cam_video_device;
        QString ir_cam_isp_video_device;
        qint32 ir_cam_width;
        qint32 ir_cam_height;
        qint32 ir_cam_timeout;
        qint32 ir_cam_bpp;
        bool ir_cam_en;
        bool ir_cam_flip_180;

        QString ir_cam_uart_port;
        qint32 ir_cam_uart_baudrate;
        qint32 ir_cam_period;
        qint32 ir_cam_exp;
        double ir_cam_gpol;

        QString ir_cam_enc_mode;
        qint32 ir_cam_enc_target_bps;
        qint32 ir_cam_target_fps;

        // --- IR Lens ---
        QString ir_lens_uart_port;
        qint32 ir_lens_uart_baudrate;

        // --- LDC20I ---
        QString ldc_uart_port;
        qint32 ldc_uart_baudrate;

        // --- GyroPlatform ---
        QString m_gyroPlatform_uartPort;
        bool m_gyroPlatform_uartDebug;
        quint32 m_gyroPlatform_deviceId;
        bool m_gyroPlatform_compensateGyroDrift;
        float m_gyroPlatform_yawGyroDrift;
        float m_gyroPlatform_pitchGyroDrift;

    } /* struct */ SettingsStore;

  public:
    SettingsManager(QString settings_file_name, QObject *parent = nullptr);
    void loadSettings();
    void writeAllSettings();
    void writeSetting(const QString &key, const QVariant &variant);
    SettingsStore settings_store;
} /* class SettingsManager */;

#endif // SETTINGSMANAGER_H
