#include "settingsmanager.h"

SettingsManager::SettingsManager(QString settings_file_name , QObject *parent)
    : QObject(parent)
    , settings_file_name {settings_file_name}
{
}


void SettingsManager::loadSettings()
{
    // .ini format example
    QSettings settings(settings_file_name, QSettings::IniFormat);

    qDebug()<< settings.fileName();
    bool bOk = false;
    settings_store.ip_dev = settings.value("Network.IP_Edge2").toString();
    settings_store.ip_host = settings.value("Network.IP_HostPC").toString();
    settings_store.udp_port_host = settings.value("Network.UDP_Port_Edge2").toInt();
    settings_store.udp_port_dev = settings.value("Network.UDP_Port_HostPC").toInt();
    settings_store.tv_cam_width = settings.value("TVCam.TV_Cam_Width").toInt();
    settings_store.tv_cam_height= settings.value("TVCam.TV_Cam_Height").toInt();
    settings_store.tv_cam_timeout = settings.value("TVCam.TV_Cam_Timeout").toInt();
    settings_store.tv_cam_video_device = settings.value("TVCam.TV_Cam_Device").toString();
    settings_store.tv_cam_bpp = settings.value("TVCam.TV_Cam_BytesPerPixel").toInt();
    settings_store.tv_cam_uart_port = settings.value("TVCam.TV_UART_Device").toString();
    settings_store.tv_cam_uart_baudrate = settings.value("TVCam.TV_UART_Baudrate").toInt();

    settings_store.tv_cam_enc_mode = settings.value("TVCam.TV_Cam_Enc_Mode", "cbr").toString();
    settings_store.tv_cam_enc_target_bps = settings.value("TVCam.TV_Cam_Enc_BPS", 2000000).toInt();
    settings_store.tv_cam_target_fps = settings.value("TVCam.TV_Cam_Target_FPS", 25).toInt();


    settings_store.ir_cam_width = settings.value("IRCam.IR_Cam_Width").toInt();
    settings_store.ir_cam_height= settings.value("IRCam.IR_Cam_Height").toInt();
    settings_store.ir_cam_timeout = settings.value("IRCam.IR_Cam_Timeout").toInt();
    settings_store.ir_cam_video_device = settings.value("IRCam.IR_Cam_Device").toString();
    settings_store.ir_cam_isp_video_device = settings.value("IRCam.IR_Cam_ISP_Device").toString();
    settings_store.ir_cam_bpp = settings.value("IRCam.IR_Cam_BytesPerPixel").toInt();
    settings_store.ir_cam_uart_port = settings.value("IRCam.IR_UART_Device").toString();
    settings_store.ir_cam_uart_baudrate = settings.value("IRCam.IR_UART_Baudrate").toInt();
    settings_store.ir_cam_period =  settings.value("IRCam.IR_ExpPeriod").toInt();
    settings_store.ir_cam_exp =  settings.value("IRCam.IR_Exp").toInt();
    settings_store.ir_cam_gpol =  settings.value("IRCam.IR_GPOL").toDouble();

    settings_store.ir_lens_uart_port = settings.value("IRLens.UART_Device").toString();
    settings_store.ir_lens_uart_baudrate = settings.value("IRLens.UART_Baudrate").toInt();

    settings_store.ir_cam_enc_mode = settings.value("IRCam.IR_Cam_Enc_Mode", "cbr").toString();
    settings_store.ir_cam_enc_target_bps = settings.value("IRCam.IR_Cam_Enc_BPS", 500000).toInt();
    settings_store.ir_cam_target_fps = settings.value("IRCam.IR_Cam_Target_FPS", 25).toInt();

    settings_store.ldc_uart_port = settings.value("LDC20I.UART_Device").toString();
    settings_store.ldc_uart_baudrate = settings.value("LDC20I.UART_Baudrate").toInt();


    settings_store.m_gyroPlatform_uartPort  = settings.value( "GyroPlatform.UART_Device").toString();
    settings_store.m_gyroPlatform_uartDebug = settings.value( "GyroPlatform.UART_Debug").toBool();
    settings_store.m_gyroPlatform_deviceId = settings.value( "GyroPlatform.DeviceId").toUInt();

    settings_store.m_gyroPlatform_compensateGyroDrift = settings.value( "GyroPlatform.CompensateGyroDrift", false).toBool();
    settings_store.m_gyroPlatform_yawGyroDrift = settings.value( "GyroPlatform.YawGyroDrift", -0.355f).toFloat();
    settings_store.m_gyroPlatform_pitchGyroDrift = settings.value( "GyroPlatform.PitchGyroDrift", -0.530f).toFloat();
}

void SettingsManager::writeAllSettings()
{

   writeSetting("Network.IP_Edge2",  settings_store.ip_dev);
   writeSetting("Network.IP_HostPC",  settings_store.ip_host);
   writeSetting("Network.UDP_Port_Edge2",  settings_store.udp_port_dev);
   writeSetting("Network.UDP_Port_HostPC",  settings_store.udp_port_host);

   writeSetting("TVCam.TV_Cam_Width",  settings_store.tv_cam_width);
   writeSetting("TVCam.TV_Cam_Height",  settings_store.tv_cam_height);
   writeSetting("TVCam.TV_Cam_Timeout",  settings_store.tv_cam_timeout);
   writeSetting("TVCam.TV_Cam_Device", settings_store.tv_cam_video_device);
   writeSetting("TVCam.TV_Cam_BytesPerPixel", settings_store.tv_cam_bpp);
   writeSetting("TVCam.TV_UART_Device", settings_store.tv_cam_uart_port);
   writeSetting("IRCam.IR_Cam_ISP_Device", settings_store.ir_cam_isp_video_device);

   writeSetting("TVCam.TV_UART_Baudrate", settings_store.tv_cam_uart_baudrate);
   writeSetting("IRCam.TV_Cam_Enc_Mode", settings_store.tv_cam_enc_mode);
   writeSetting("IRCam.TV_Cam_Enc_BPS", settings_store.tv_cam_enc_target_bps);
   writeSetting("IRCam.TV_Cam_Target_FPS", settings_store.tv_cam_target_fps);


   writeSetting("IRCam.IR_Cam_Width",  settings_store.ir_cam_width);
   writeSetting("IRCam.IR_Cam_Height",  settings_store.ir_cam_height);
   writeSetting("IRCam.IR_Cam_Timeout",  settings_store.ir_cam_timeout);
   writeSetting("IRCam.IR_Cam_Device", settings_store.ir_cam_video_device);
   writeSetting("IRCam.IR_Cam_BytesPerPixel", settings_store.ir_cam_bpp);
   writeSetting("IRCam.IR_UART_Device", settings_store.ir_cam_uart_port);
   writeSetting("IRCam.IR_UART_Baudrate", settings_store.ir_cam_uart_baudrate);
   writeSetting("IRCam.IR_ExpPeriod", settings_store.ir_cam_period);
   writeSetting("IRCam.IR_Exp", settings_store.ir_cam_exp);
   writeSetting("IRCam.IR_GPOL", settings_store.ir_cam_gpol);
   writeSetting("IRCam.IR_Cam_Enc_Mode", settings_store.ir_cam_enc_mode);
   writeSetting("IRCam.IR_Cam_Enc_BPS", settings_store.ir_cam_enc_target_bps);
   writeSetting("IRCam.IR_Cam_Target_FPS", settings_store.ir_cam_target_fps);


   writeSetting("IRLens.UART_Device", settings_store.ir_lens_uart_port);
   writeSetting("IRLens.UART_Baudrate", settings_store.ir_lens_uart_baudrate);

   writeSetting("LDC20I.UART_Device", settings_store.ldc_uart_port);
   writeSetting("LDC20I.UART_Baudrate", settings_store.ldc_uart_baudrate);


   writeSetting( "GyroPlatform.UART_Device", settings_store.m_gyroPlatform_uartPort);
   writeSetting( "GyroPlatform.UART_Debug", settings_store.m_gyroPlatform_uartDebug);
   writeSetting( "GyroPlatform.DeviceId", settings_store.m_gyroPlatform_deviceId);

   writeSetting( "GyroPlatform.CompensateGyroDrift", settings_store.m_gyroPlatform_compensateGyroDrift);
   writeSetting( "GyroPlatform.YawGyroDrift", settings_store.m_gyroPlatform_yawGyroDrift);
   writeSetting( "GyroPlatform.PitchGyroDrift", settings_store.m_gyroPlatform_pitchGyroDrift);
}


void SettingsManager::writeSetting(const QString &key, const QVariant &variant)
{
    QSettings settings(settings_file_name, QSettings::IniFormat);

    settings.setValue(key, variant);
}
