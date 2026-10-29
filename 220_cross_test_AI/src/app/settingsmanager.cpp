#include "settingsmanager.h"

SettingsManager::SettingsManager(QString settings_file_name , QObject *parent)
    : QObject(parent)
    , settings_file_name {settings_file_name}
{
}
void SettingsManager::loadSettings()
{
    QSettings settings(settings_file_name, QSettings::IniFormat);
    qDebug() << settings.fileName();

    // ================================================================
    // Network
    // ================================================================
    settings_store.ip_dev           = settings.value("Network.IP_Edge2", "192.168.1.96").toString();
    settings_store.ip_host          = settings.value("Network.IP_HostPC", "192.168.1.54").toString();
    settings_store.udp_port_host    = settings.value("Network.UDP_Port_Edge2", 52515).toInt();
    settings_store.udp_port_dev     = settings.value("Network.UDP_Port_HostPC", 52516).toInt();

    // ================================================================
    // MPEG-TS (общие)
    // ================================================================
    settings_store.mpeg_ts_en            = settings.value("MPEG-TS.Enable", true).toBool();
    settings_store.mpeg_ts_telemetry_en  = settings.value("MPEG-TS.Telemetry_Enable", true).toBool();
    settings_store.mpeg_ts_port          = settings.value("MPEG-TS.UDP_Port", 5016).toInt();

    // ================================================================
    // MPEG-TS SRT (включая FEC — только для SRT)
    // ================================================================
    settings_store.mpeg_ts_use_srt           = settings.value("MPEG-TS.SRT.Use_SRT", false).toBool();
    settings_store.mpeg_ts_srt_latency_ms    = settings.value("MPEG-TS.SRT.Latency_MS", 300).toInt();
    settings_store.mpeg_ts_srt_tlpktdrop     = settings.value("MPEG-TS.SRT.TLPKTDROP", true).toBool();

    // Кол-во TS-пакетов (188 байт) в одной SRT payload-посылке.
    // Диапазон: 1..7. Итоговый payload = count * 188.
    const int tsPktCount = settings.value("MPEG-TS.SRT.TS_Packet_Count", 4).toInt();
    settings_store.mpeg_ts_srt_ts_packet_count  = qBound(1, tsPktCount, 7);

    // FEC (вложен в SRT)
    settings_store.mpeg_ts_srt_fec_enabled = settings.value("MPEG-TS.SRT.FEC.Enabled", true).toBool();
    settings_store.mpeg_ts_srt_fec_cols    = qBound(2,   settings.value("MPEG-TS.SRT.FEC.Cols", 5).toInt(), 255);
    settings_store.mpeg_ts_srt_fec_rows    = qBound(1,   settings.value("MPEG-TS.SRT.FEC.Rows", 36).toInt(), 255);

    QString fecLayout = settings.value("MPEG-TS.SRT.FEC.Layout", "staircase").toString().trimmed().toLower();
    if (fecLayout != "staircase" && fecLayout != "even") {
        fecLayout = "staircase";
    }
    settings_store.mpeg_ts_srt_fec_layout = fecLayout;

    QString fecArq = settings.value("MPEG-TS.SRT.FEC.ARQ", "always").toString().trimmed().toLower();
    if (fecArq != "always" && fecArq != "onreq" && fecArq != "never") {
        fecArq = "always";
    }
    settings_store.mpeg_ts_srt_fec_arq = fecArq;

    // ================================================================
    // TV Cam
    // ================================================================
    settings_store.tv_cam_en             = settings.value("TVCam.TV_Cam_Enable", true).toBool();
    settings_store.tv_cam_width          = settings.value("TVCam.TV_Cam_Width", 1920).toInt();
    settings_store.tv_cam_height         = settings.value("TVCam.TV_Cam_Height", 1080).toInt();
    settings_store.tv_cam_timeout        = settings.value("TVCam.TV_Cam_Timeout", 15000).toInt();
    settings_store.tv_cam_video_device   = settings.value("TVCam.TV_Cam_Device", "/dev/video0").toString();
    settings_store.tv_cam_bpp            = settings.value("TVCam.TV_Cam_BytesPerPixel", 3).toInt();
    settings_store.tv_cam_flip_180       = settings.value("TVCam.TV_Cam_Flip_180", false).toBool();

    settings_store.tv_cam_uart_port      = settings.value("TVCam.TV_UART_Device", "/dev/ttyS9").toString();
    settings_store.tv_cam_uart_baudrate  = settings.value("TVCam.TV_UART_Baudrate", 115200).toInt();

    settings_store.tv_cam_enc_mode       = settings.value("TVCam.TV_Cam_Enc_Mode", "cbr").toString();
    settings_store.tv_cam_enc_target_bps = settings.value("TVCam.TV_Cam_Enc_BPS", 2000000).toInt();
    settings_store.tv_cam_target_fps     = settings.value("TVCam.TV_Cam_Target_FPS", 25).toInt();

    // ================================================================
    // IR Cam
    // ================================================================
    settings_store.ir_cam_en             = settings.value("IRCam.IR_Cam_Enable", true).toBool();
    settings_store.ir_cam_width          = settings.value("IRCam.IR_Cam_Width", 640).toInt();
    settings_store.ir_cam_height         = settings.value("IRCam.IR_Cam_Height", 512).toInt();
    settings_store.ir_cam_timeout        = settings.value("IRCam.IR_Cam_Timeout", 15000).toInt();
    settings_store.ir_cam_video_device   = settings.value("IRCam.IR_Cam_Device", "/dev/video11").toString();
    settings_store.ir_cam_isp_video_device = settings.value("IRCam.IR_Cam_ISP_Device", "/dev/video31").toString();
    settings_store.ir_cam_bpp            = settings.value("IRCam.IR_Cam_BytesPerPixel", 2).toInt();
    settings_store.ir_cam_flip_180       = settings.value("IRCam.IR_Cam_Flip_180", false).toBool();

    settings_store.ir_cam_uart_port      = settings.value("IRCam.IR_UART_Device", "/dev/ttyS8").toString();
    settings_store.ir_cam_uart_baudrate  = settings.value("IRCam.IR_UART_Baudrate", 115200).toInt();
    settings_store.ir_cam_period         = settings.value("IRCam.IR_ExpPeriod", 92600).toInt();
    settings_store.ir_cam_exp            = settings.value("IRCam.IR_Exp", 70000).toInt();
    settings_store.ir_cam_gpol           = settings.value("IRCam.IR_GPOL", 870.0).toDouble();

    settings_store.ir_cam_enc_mode       = settings.value("IRCam.IR_Cam_Enc_Mode", "cbr").toString();
    settings_store.ir_cam_enc_target_bps = settings.value("IRCam.IR_Cam_Enc_BPS", 5000000).toInt();
    settings_store.ir_cam_target_fps     = settings.value("IRCam.IR_Cam_Target_FPS", 25).toInt();

    // ================================================================
    // Camera watchdog / first-frame check
    // ================================================================
    settings_store.tv_cam_first_frame_timeout_ms = qBound(1000,  settings.value("CameraCheck.TV_First_Frame_Timeout_MS", 7000).toInt(), 60000);
    settings_store.ir_cam_first_frame_timeout_ms = qBound(1000,  settings.value("CameraCheck.IR_First_Frame_Timeout_MS", 2000).toInt(), 60000);
    settings_store.cam_frame_retry_timeout_ms    = qBound(500,   settings.value("CameraCheck.Frame_Retry_Timeout_MS", 2000).toInt(), 30000);
    settings_store.cam_frame_restart_count       = qBound(0,     settings.value("CameraCheck.Restart_Count", 3).toInt(), 10);

    // ================================================================
    // IR Lens
    // ================================================================
    settings_store.ir_lens_uart_port     = settings.value("IRLens.UART_Device", "/dev/ttyS0").toString();
    settings_store.ir_lens_uart_baudrate = settings.value("IRLens.UART_Baudrate", 19200).toInt();

    // ================================================================
    // LDC20I
    // ================================================================
    settings_store.ldc_uart_port         = settings.value("LDC20I.UART_Device", "/dev/ttyS7").toString();
    settings_store.ldc_uart_baudrate     = settings.value("LDC20I.UART_Baudrate", 115200).toInt();

    // ================================================================
    // GyroPlatform
    // ================================================================
    settings_store.m_gyroPlatform_uartPort             = settings.value("GyroPlatform.UART_Device", "/dev/ttyS1").toString();
    settings_store.m_gyroPlatform_uartDebug            = settings.value("GyroPlatform.UART_Debug", false).toBool();
    settings_store.m_gyroPlatform_deviceId             = settings.value("GyroPlatform.DeviceId", 1u).toUInt();
    settings_store.m_gyroPlatform_compensateGyroDrift  = settings.value("GyroPlatform.CompensateGyroDrift", false).toBool();
    settings_store.m_gyroPlatform_yawGyroDrift         = settings.value("GyroPlatform.YawGyroDrift", -0.355f).toFloat();
    settings_store.m_gyroPlatform_pitchGyroDrift       = settings.value("GyroPlatform.PitchGyroDrift", -0.530f).toFloat();
}

void SettingsManager::writeAllSettings()
{
    // --- Network ---
    writeSetting("Network.IP_Edge2",          settings_store.ip_dev);
    writeSetting("Network.IP_HostPC",         settings_store.ip_host);
    writeSetting("Network.UDP_Port_Edge2",    settings_store.udp_port_dev);
    writeSetting("Network.UDP_Port_HostPC",   settings_store.udp_port_host);

    // --- MPEG-TS general ---
    writeSetting("MPEG-TS.Enable",            settings_store.mpeg_ts_en);
    writeSetting("MPEG-TS.Telemetry_Enable",  settings_store.mpeg_ts_telemetry_en);
    writeSetting("MPEG-TS.UDP_Port",          settings_store.mpeg_ts_port);

    // --- MPEG-TS SRT ---
    writeSetting("MPEG-TS.SRT.Use_SRT",           settings_store.mpeg_ts_use_srt);
    writeSetting("MPEG-TS.SRT.Latency_MS",        settings_store.mpeg_ts_srt_latency_ms);
    writeSetting("MPEG-TS.SRT.TLPKTDROP",         settings_store.mpeg_ts_srt_tlpktdrop);
    writeSetting("MPEG-TS.SRT.TS_Packet_Count",   settings_store.mpeg_ts_srt_ts_packet_count);

    // --- MPEG-TS SRT FEC ---
    writeSetting("1",       settings_store.mpeg_ts_srt_fec_enabled);
    writeSetting("MPEG-TS.SRT.FEC.Cols",          settings_store.mpeg_ts_srt_fec_cols);
    writeSetting("MPEG-TS.SRT.FEC.Rows",          settings_store.mpeg_ts_srt_fec_rows);
    writeSetting("MPEG-TS.SRT.FEC.Layout",        settings_store.mpeg_ts_srt_fec_layout);
    writeSetting("MPEG-TS.SRT.FEC.ARQ",           settings_store.mpeg_ts_srt_fec_arq);

    // --- TV Cam ---
    writeSetting("TVCam.TV_Cam_Enable",           settings_store.tv_cam_en);
    writeSetting("TVCam.TV_Cam_Width",            settings_store.tv_cam_width);
    writeSetting("TVCam.TV_Cam_Height",           settings_store.tv_cam_height);
    writeSetting("TVCam.TV_Cam_Timeout",          settings_store.tv_cam_timeout);
    writeSetting("TVCam.TV_Cam_Device",           settings_store.tv_cam_video_device);
    writeSetting("TVCam.TV_Cam_BytesPerPixel",    settings_store.tv_cam_bpp);
    writeSetting("TVCam.TV_Cam_Flip_180",         settings_store.tv_cam_flip_180);

    writeSetting("TVCam.TV_UART_Device",          settings_store.tv_cam_uart_port);
    writeSetting("TVCam.TV_UART_Baudrate",        settings_store.tv_cam_uart_baudrate);

    // ИСПРАВЛЕНО: было "IRCam.TV_Cam_Enc_*"
    writeSetting("TVCam.TV_Cam_Enc_Mode",         settings_store.tv_cam_enc_mode);
    writeSetting("TVCam.TV_Cam_Enc_BPS",          settings_store.tv_cam_enc_target_bps);
    writeSetting("TVCam.TV_Cam_Target_FPS",       settings_store.tv_cam_target_fps);

    // --- IR Cam ---
    writeSetting("IRCam.IR_Cam_Enable",           settings_store.ir_cam_en);
    writeSetting("IRCam.IR_Cam_Width",            settings_store.ir_cam_width);
    writeSetting("IRCam.IR_Cam_Height",           settings_store.ir_cam_height);
    writeSetting("IRCam.IR_Cam_Timeout",          settings_store.ir_cam_timeout);
    writeSetting("IRCam.IR_Cam_Device",           settings_store.ir_cam_video_device);
    writeSetting("IRCam.IR_Cam_ISP_Device",       settings_store.ir_cam_isp_video_device);
    writeSetting("IRCam.IR_Cam_BytesPerPixel",    settings_store.ir_cam_bpp);
    writeSetting("IRCam.IR_Cam_Flip_180",         settings_store.ir_cam_flip_180);

    writeSetting("IRCam.IR_UART_Device",          settings_store.ir_cam_uart_port);
    writeSetting("IRCam.IR_UART_Baudrate",        settings_store.ir_cam_uart_baudrate);
    writeSetting("IRCam.IR_ExpPeriod",            settings_store.ir_cam_period);
    writeSetting("IRCam.IR_Exp",                  settings_store.ir_cam_exp);
    writeSetting("IRCam.IR_GPOL",                 settings_store.ir_cam_gpol);

    writeSetting("IRCam.IR_Cam_Enc_Mode",         settings_store.ir_cam_enc_mode);
    writeSetting("IRCam.IR_Cam_Enc_BPS",          settings_store.ir_cam_enc_target_bps);
    writeSetting("IRCam.IR_Cam_Target_FPS",       settings_store.ir_cam_target_fps);

    // --- Camera watchdog / check ---
    writeSetting("CameraCheck.TV_First_Frame_Timeout_MS", settings_store.tv_cam_first_frame_timeout_ms);
    writeSetting("CameraCheck.IR_First_Frame_Timeout_MS", settings_store.ir_cam_first_frame_timeout_ms);
    writeSetting("CameraCheck.Frame_Retry_Timeout_MS",    settings_store.cam_frame_retry_timeout_ms);
    writeSetting("CameraCheck.Restart_Count",             settings_store.cam_frame_restart_count);

    // --- IR Lens ---
    writeSetting("IRLens.UART_Device",            settings_store.ir_lens_uart_port);
    writeSetting("IRLens.UART_Baudrate",          settings_store.ir_lens_uart_baudrate);

    // --- LDC20I ---
    writeSetting("LDC20I.UART_Device",            settings_store.ldc_uart_port);
    writeSetting("LDC20I.UART_Baudrate",          settings_store.ldc_uart_baudrate);

    // --- GyroPlatform ---
    writeSetting("GyroPlatform.UART_Device",              settings_store.m_gyroPlatform_uartPort);
    writeSetting("GyroPlatform.UART_Debug",               settings_store.m_gyroPlatform_uartDebug);
    writeSetting("GyroPlatform.DeviceId",                 settings_store.m_gyroPlatform_deviceId);
    writeSetting("GyroPlatform.CompensateGyroDrift",      settings_store.m_gyroPlatform_compensateGyroDrift);
    writeSetting("GyroPlatform.YawGyroDrift",             settings_store.m_gyroPlatform_yawGyroDrift);
    writeSetting("GyroPlatform.PitchGyroDrift",           settings_store.m_gyroPlatform_pitchGyroDrift);

    // Force flush to disk
    QSettings s(settings_file_name, QSettings::IniFormat);
    s.sync();
}




void SettingsManager::writeSetting(const QString &key, const QVariant &variant)
{
    QSettings settings(settings_file_name, QSettings::IniFormat);

    settings.setValue(key, variant);
}
