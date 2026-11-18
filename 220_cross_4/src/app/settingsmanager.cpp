#include "settingsmanager.h"
#include <QDebug>

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

    // IP назначения UDP-телеметрии. Если ключ отсутствует (старый конфиг)
    // или пуст — наследуем общий IP_HostPC (обратная совместимость).
    settings_store.ip_telemetry = settings.value("Network.IP_Telemetry", settings_store.ip_host).toString();
    if (settings_store.ip_telemetry.trimmed().isEmpty())
        settings_store.ip_telemetry = settings_store.ip_host;

    // ================================================================
    // MPEG-TS (общие)
    // ================================================================
    settings_store.mpeg_ts_en            = settings.value("MPEG-TS.Enable", true).toBool();
    settings_store.mpeg_ts_telemetry_en  = settings.value("MPEG-TS.Telemetry_Enable", true).toBool();
    settings_store.mpeg_ts_port          = settings.value("MPEG-TS.UDP_Port", 5016).toInt();
    // Периодическая диагностика [Bridge]/[Diag]: по умолчанию ВЫКЛЮЧЕНА (тихо в
    // проде); true — включить периодический дамп моста encode→transport для отладки.
    settings_store.mpeg_ts_diag_logs     = settings.value("MPEG-TS.Diag_Logs", false).toBool();

    // ================================================================
    // MPEG-TS SRT (включая FEC — только для SRT)
    // ================================================================
    settings_store.mpeg_ts_use_srt           = settings.value("MPEG-TS.SRT.Use_SRT", true).toBool();
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
    // TrackerControl (vdb_vrt_tracker_adapter)
    // ================================================================
    settings_store.trackerCtl.useAdapter = settings.value("TrackerControl.Use_Adapter", false).toBool();
    settings_store.trackerCtl.stepMs     = qBound(5, settings.value("TrackerControl.Step_MS", 20).toInt(), 200);

    // Дефолты: TV 58.1°…0.19°, TPV(IR) 26.99°…2.75°.
    // Base Kp X=2.5 / Y=1.25 — эквивалент старого контура (Kp 0.05/0.025 × kW −50).
    struct TrackerCtlDefaults {
        const char *name;
        float wideFov;
        float teleFov;
        float kpX;
        float kpY;
    };
    static const TrackerCtlDefaults tcDef[2] = {
        { "TV",  58.1f,  0.19f, 2.5f, 1.25f },
        { "TPV", 26.99f, 2.75f, 2.5f, 1.25f },
    };
    static const char *tcAxName[2] = { "X", "Y" };

    for (int ch = 0; ch < 2; ++ch) {
        ChannelCtlCfg &c = settings_store.trackerCtl.Channel[ch];
        const QString p = QString("TrackerControl.%1.").arg(tcDef[ch].name);

        c.Wide.FovVal = settings.value(p + "Wide.FovVal", tcDef[ch].wideFov).toFloat();
        c.Tele.FovVal = settings.value(p + "Tele.FovVal", tcDef[ch].teleFov).toFloat();

        for (int ax = 0; ax < 2; ++ax) {
            const QString pa = p + tcAxName[ax] + ".";
            c.Axis[ax].Kp     = settings.value(pa + "Kp",     ax == 0 ? tcDef[ch].kpX : tcDef[ch].kpY).toFloat();
            c.Axis[ax].Ki     = settings.value(pa + "Ki",     0.0f).toFloat();
            c.Axis[ax].Kd     = settings.value(pa + "Kd",     0.0f).toFloat();
            c.Axis[ax].ILim   = settings.value(pa + "ILim",   1.0f).toFloat();
            c.Axis[ax].OutLim = settings.value(pa + "OutLim", 20.0f).toFloat();

            FovPointCfg *fovPts[2] = { &c.Wide, &c.Tele };
            static const char *tcFovName[2] = { "Wide.", "Tele." };
            for (int f = 0; f < 2; ++f) {
                const QString pf = p + tcFovName[f] + tcAxName[ax] + ".";
                fovPts[f]->kKp    = settings.value(pf + "kKp",    1.0f).toFloat();
                fovPts[f]->kKi    = settings.value(pf + "kKi",    1.0f).toFloat();
                fovPts[f]->kKd    = settings.value(pf + "kKd",    1.0f).toFloat();
                fovPts[f]->kKiLim = settings.value(pf + "kKiLim", 1.0f).toFloat();
            }
        }
    }

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
    settings_store.m_gyroPlatform_zero_x_offset        = settings.value("GyroPlatform.ZeroXOffset", 0.0f).toFloat();
    settings_store.m_gyroPlatform_zero_z_offset        = settings.value("GyroPlatform.ZeroZOffset", 0.0f).toFloat();
}

void SettingsManager::saveGyroDrift(float yawGyroDrift, float pitchGyroDrift, bool compensate)
{
    // Единая точка записи дрейфа: пишем в ТОТ ЖЕ файл, что и loadSettings(), и сразу
    // обновляем settings_store, чтобы in-memory не расходился с диском.
    QSettings settings(settings_file_name, QSettings::IniFormat);
    settings.setValue("GyroPlatform.YawGyroDrift",        yawGyroDrift);
    settings.setValue("GyroPlatform.PitchGyroDrift",      pitchGyroDrift);
    settings.setValue("GyroPlatform.CompensateGyroDrift", compensate);
    settings.sync();

    settings_store.m_gyroPlatform_yawGyroDrift        = yawGyroDrift;
    settings_store.m_gyroPlatform_pitchGyroDrift      = pitchGyroDrift;
    settings_store.m_gyroPlatform_compensateGyroDrift = compensate;

    qDebug() << "[GYRO] saveGyroDrift →" << settings_file_name
             << "yaw=" << yawGyroDrift << "pitch=" << pitchGyroDrift
             << "compensate=" << compensate;
}

void SettingsManager::saveGspZeroXOffset(float offset)
{
    QSettings settings(settings_file_name, QSettings::IniFormat);
    settings.setValue("GyroPlatform.ZeroXOffset", offset);
    settings.sync();
    settings_store.m_gyroPlatform_zero_x_offset = offset;
    qDebug() << "[GSP] saveGspZeroXOffset →" << offset;
}

void SettingsManager::saveGspZeroZOffset(float offset)
{
    QSettings settings(settings_file_name, QSettings::IniFormat);
    settings.setValue("GyroPlatform.ZeroZOffset", offset);
    settings.sync();
    settings_store.m_gyroPlatform_zero_z_offset = offset;
    qDebug() << "[GSP] saveGspZeroZOffset →" << offset;
}

void SettingsManager::writeAllSettings()
{
    // --- Network ---
    writeSetting("Network.IP_Edge2",          settings_store.ip_dev);
    writeSetting("Network.IP_HostPC",         settings_store.ip_host);
    writeSetting("Network.IP_Telemetry",      settings_store.ip_telemetry);
    // ВАЖНО: ключи соответствуют loadSettings():
    //   UDP_Port_Edge2 ↔ udp_port_host, UDP_Port_HostPC ↔ udp_port_dev.
    // Ранее стояла перестановка — writeAllSettings() менял порты местами.
    writeSetting("Network.UDP_Port_Edge2",    settings_store.udp_port_host);
    writeSetting("Network.UDP_Port_HostPC",   settings_store.udp_port_dev);

    // --- MPEG-TS general ---
    writeSetting("MPEG-TS.Enable",            settings_store.mpeg_ts_en);
    writeSetting("MPEG-TS.Telemetry_Enable",  settings_store.mpeg_ts_telemetry_en);
    writeSetting("MPEG-TS.UDP_Port",          settings_store.mpeg_ts_port);
    writeSetting("MPEG-TS.Diag_Logs",         settings_store.mpeg_ts_diag_logs);

    // --- MPEG-TS SRT ---
    writeSetting("MPEG-TS.SRT.Use_SRT",           settings_store.mpeg_ts_use_srt);
    writeSetting("MPEG-TS.SRT.Latency_MS",        settings_store.mpeg_ts_srt_latency_ms);
    writeSetting("MPEG-TS.SRT.TLPKTDROP",         settings_store.mpeg_ts_srt_tlpktdrop);
    writeSetting("MPEG-TS.SRT.TS_Packet_Count",   settings_store.mpeg_ts_srt_ts_packet_count);

    // --- MPEG-TS SRT FEC ---
    // Исправлен ключ: было writeSetting("1", …) — Enabled не сохранялся.
    writeSetting("MPEG-TS.SRT.FEC.Enabled",   settings_store.mpeg_ts_srt_fec_enabled);
    writeSetting("MPEG-TS.SRT.FEC.Cols",          settings_store.mpeg_ts_srt_fec_cols);
    writeSetting("MPEG-TS.SRT.FEC.Rows",          settings_store.mpeg_ts_srt_fec_rows);
    writeSetting("MPEG-TS.SRT.FEC.Layout",        settings_store.mpeg_ts_srt_fec_layout);
    writeSetting("MPEG-TS.SRT.FEC.ARQ",           settings_store.mpeg_ts_srt_fec_arq);

    // --- TrackerControl (vdb_vrt_tracker_adapter) ---
    writeSetting("TrackerControl.Use_Adapter",    settings_store.trackerCtl.useAdapter);
    writeSetting("TrackerControl.Step_MS",        settings_store.trackerCtl.stepMs);
    {
        static const char *tcChName[2]  = { "TV", "TPV" };
        static const char *tcAxName[2]  = { "X", "Y" };
        static const char *tcFovName[2] = { "Wide", "Tele" };
        for (int ch = 0; ch < 2; ++ch) {
            const ChannelCtlCfg &c = settings_store.trackerCtl.Channel[ch];
            const QString p = QString("TrackerControl.%1.").arg(tcChName[ch]);
            writeSetting(p + "Wide.FovVal", c.Wide.FovVal);
            writeSetting(p + "Tele.FovVal", c.Tele.FovVal);
            for (int ax = 0; ax < 2; ++ax) {
                const QString pa = p + tcAxName[ax] + ".";
                writeSetting(pa + "Kp",     c.Axis[ax].Kp);
                writeSetting(pa + "Ki",     c.Axis[ax].Ki);
                writeSetting(pa + "Kd",     c.Axis[ax].Kd);
                writeSetting(pa + "ILim",   c.Axis[ax].ILim);
                writeSetting(pa + "OutLim", c.Axis[ax].OutLim);
                const FovPointCfg *fovPts[2] = { &c.Wide, &c.Tele };
                for (int f = 0; f < 2; ++f) {
                    const QString pf = p + tcFovName[f] + "." + tcAxName[ax] + ".";
                    writeSetting(pf + "kKp",    fovPts[f]->kKp);
                    writeSetting(pf + "kKi",    fovPts[f]->kKi);
                    writeSetting(pf + "kKd",    fovPts[f]->kKd);
                    writeSetting(pf + "kKiLim", fovPts[f]->kKiLim);
                }
            }
        }
    }

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
    writeSetting("GyroPlatform.ZeroXOffset",              settings_store.m_gyroPlatform_zero_x_offset);
    writeSetting("GyroPlatform.ZeroZOffset",              settings_store.m_gyroPlatform_zero_z_offset);

    // Force flush to disk
    QSettings s(settings_file_name, QSettings::IniFormat);
    s.sync();
}




void SettingsManager::writeSetting(const QString &key, const QVariant &variant)
{
    QSettings settings(settings_file_name, QSettings::IniFormat);

    settings.setValue(key, variant);
}
