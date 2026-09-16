#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QVariant>
#include <QMetaType>
#include <qsettings.h>


class SettingsManager: public QObject
{
private:
    QString settings_file_name;
    typedef struct {
        QString ip_host;
        qint32 udp_port_host;

        QString ip_dev;
        qint32 udp_port_dev;

        QString tv_cam_video_device;

        qint32 tv_cam_width;
        qint32 tv_cam_height;
        qint32 tv_cam_timeout;
        qint32 tv_cam_bpp;
        QString tv_cam_uart_port;
        qint32  tv_cam_uart_baudrate;
        QString tv_cam_enc_mode;
        qint32 tv_cam_enc_target_bps;
        qint32 tv_cam_target_fps;


        QString ir_cam_video_device;
        QString ir_cam_isp_video_device;


        qint32 ir_cam_width;
        qint32 ir_cam_height;
        qint32 ir_cam_timeout;
        qint32 ir_cam_bpp;
        QString ir_cam_uart_port;
        qint32  ir_cam_uart_baudrate;
        qint32 ir_cam_period;
        qint32 ir_cam_exp;
        double ir_cam_gpol;

        QString ir_cam_enc_mode;
        qint32 ir_cam_enc_target_bps;
        qint32 ir_cam_target_fps;

        QString ir_lens_uart_port;
        qint32  ir_lens_uart_baudrate;

        QString ldc_uart_port;
        qint32  ldc_uart_baudrate;


        QString     m_gyroPlatform_uartPort;
        bool        m_gyroPlatform_uartDebug;
        quint32     m_gyroPlatform_deviceId;

        bool        m_gyroPlatform_compensateGyroDrift;
        float       m_gyroPlatform_yawGyroDrift;
        float       m_gyroPlatform_pitchGyroDrift;

    } SettingsStore;



public:
    SettingsManager(QString settings_file_name, QObject *parent  = nullptr);
    void loadSettings();
    void writeAllSettings();
    void writeSetting(const QString &key, const QVariant &variant);
    SettingsStore settings_store;
};

#endif // SETTINGSMANAGER_H
