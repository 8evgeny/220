#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dev_types.h"
#include "device_parts/cmd/telemetry.h"
#include "device_parts/vlx/vlx_media_parameters_dialog.h"
#include "vlx_display_parameters_dialog.h"
#include "device_parts/vlx/vlx_stream_channel.h"

class EthModel;
class EthView;

class Controller;

class CorsairKeyFilter;
class Joystick;
class Panel;

class TelemetryModel;
class TelemetryView;

class TVCtrlModel;
class SWIRCtrlModel;
class MWIRCtrlModel;
class HighlighterModel;
class GSPCtrlModel;
class FusionCtrlModel;
class terminalsettingsmodel;

class TVCtrlView;
class SWIRCtrlView;
class MWIRCtrlView;
class HighlighterView;
class GSPCtrlView;
class FusionCtrlView;
class vdbMsgSenderView;
class terminalsettingsview;


class QLabel;
class JoystickWidget;

class GSP_Initialization;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget * parent = nullptr);
    virtual ~MainWindow();

    enum MODEL_ID {
        EthModel_id,
        TerminalSettings_id,
        TelemetryModel_id,
        TVCtrlModel_id,
        SWIRCtrlModel_id,
        MWIRCtrlModel_id,
        RangeFinderCtrlModel_id,
        HighlighterModel_id,
        GSPCtrlModel_id,
        FusionCtrlModel_id,
    };

public slots:
    void response(Telemetry t);
    void blockAllInterface(bool block);
    void turnOnImitateJoystick();

private slots:

    void onVlxIdmxStateChanged(Vlx::Av::InputDemuxer::State state);

    void onChannelChanged(uint32_t chanId);

    void on_help_triggered();
    void on_about_triggered();

    void on_telemetry_triggered();
    void settings();
    void on_tvCtrl_triggered();
    void on_swirCtrl_triggered();
    void on_mwirCtrl_triggered();
    void on_highlighter_triggered();
    void on_gspCtrl_triggered();
    void on_fusionCtrl_triggered();
    void on_vdbSendMsg_triggered();
    void on_connection_triggered();
    void on_terminal_settings_triggered();

    void on_channel_tvrgb_clicked();
    void on_channel_tvgray_clicked();
    void on_channel_swir_clicked();
    void on_channel_mwir_clicked();
    void on_channel_fusion_clicked();
    void on_channel_tvgraymwir_clicked();
    void on_channel_test_clicked();
    void on_channel_cpu_clicked();

    void on_arretir_set_clicked();
    void on_arretir_reset_clicked();
    void on_el_arr_coarse_clicked();
    void on_el_arr_fine_clicked();
    void on_park_clicked();
    void on_stabilization_toggled(bool checked);
    void on_motor_off_clicked();
    void on_gsp_self_test_clicked();

    void on_inverted_position_clicked(bool checked);

    void full_screen();
    void next_channel();
    void prev_channel();
    void switch_channel();
    void select_channel(const uint32_t & id);

    void on_host_etld_clicked(bool checked);
    void on_self_test_toggled(bool checked);

    void image_ready();

    void mouse_pos(const int & x, const int & y);

    void on_fine_arretir_set_clicked();

    void on_fine_arretir_reset_clicked();

    void on_setAngle_released();

    void on_el_arr_clicked();

    void on_btn_confirmPassword_clicked();

    void sl_blockInterfaceForArretiresOff();
    void sl_alignJoystikWidget();

    void on_btn_prepareToWork_clicked();

    void on_angleZ_Max_180_clicked();

    void on_angleZ_Max_360_clicked();

    void on_btn_mwirDampEn_clicked();

    void on_fixAngle_clicked();

    void sl_fixAngleONTimer();
    void sl_fixAngleOFFTimer();

    void on_btn_turnOnTVMWIRChannel_clicked();
    void on_btn_turnOnFusionChannel_clicked();

    void on_channel_rtp_clicked();
    void on_channel_rtp_2_clicked();

private:
    void load();
    void save();
    void closeEvent(QCloseEvent * event) override;

    void init_controller();
    void init_tvCtrl();
    void init_swirCtrl();
    void init_mwirCtrl();
    void init_highlighterView();
    void init_gspCtrl();
    void init_fusionCtrl();
    void init_vdbMsgSenderView();
    void init_terminalSettings();
    void init_iterface();
    void init_telemetry();
    void init_keyfilter();
    void init_joystick();
    void init_panel();
    void init_logger();
    void init_gspInitializationModul();

    // Service Commands
    void serviceCmdTurnOn();
    void serviceCmdTurnOff();

        // Pseudo Electrical Arretires
    void fixAngleTurnON();
    void fixAngleTurnOFF();

    void indicateInvertedPos();

private:
    Ui::MainWindow * _ui{nullptr};
    static const DEV_TYPES _dev_type = DEV_TYPES::en_dev_type_350p;
    EthModel *   _interfaceModel{nullptr};
    EthView *    _interfaceView{nullptr};
    Controller * _controller{nullptr};

    CorsairKeyFilter* _keyfilter{nullptr};
    Joystick*         _joystick{nullptr};
    Panel*            _panel{nullptr};

    TelemetryModel* _telemetryModel{nullptr};
    TelemetryView*  _telemetryView{nullptr};

    TVCtrlModel*           _tvCtrlModel{nullptr};
    SWIRCtrlModel*         _swirCtrlModel{nullptr};
    MWIRCtrlModel*         _mwirCtrlModel{nullptr};
    HighlighterModel*      _highlighterModel{nullptr};
    GSPCtrlModel*          _gspCtrlModel{nullptr};
    FusionCtrlModel*       _fusionCtrlModel{nullptr};
    terminalsettingsmodel* _terminalSettingsModel{nullptr};
    
    TVCtrlView*           _tvCtrlView{nullptr};
    SWIRCtrlView*         _swirCtrlView{nullptr};
    MWIRCtrlView*         _mwirCtrlView{nullptr};
    HighlighterView*      _highlighterView{nullptr};
    GSPCtrlView*          _gspCtrlView{nullptr};
    FusionCtrlView*       _fusionCtrlView{nullptr};
    vdbMsgSenderView*     _vdbMsgSenderView{nullptr};
    terminalsettingsview* _terminalSettingsView{nullptr};

    QString _window_title;

    QString _panel_com_port_name = "COM4";
    QString _media_path = "media";

    int _mouse_x = 0;
    int _mouse_y = 0;
    double _fps = 0;

    QLabel * _status_vlx_idmx;
    QLabel * _status_mouse_pos;
    QLabel * _status_fps;
    QLabel * _status_connection;
    QLabel * _status_joystick;
//    QLabel * _status_panel;

    // Service Commands
    bool _serviceCmdsOn { false };
    QString _serviceCmdPassword { "220197" };

    JoystickWidget * m_newJoystik;
    QTimer * m_joystikTimer;

    GSP_Initialization * m_gspInit;
    QString m_gspInitStatus {""};

    typedef struct
    {
        bool enable;
        double angleZ;
        double angleX;
        bool rememberAngles;
        bool angleCtrlOn;
        uint32_t tmrPeriodAnglCtrlON;
        uint32_t tmrPeriodAnglCtrlOFF;
    } t_fixAngle;
    t_fixAngle m_fixAngle;

    VlxMediaParametersDialog *m_vlxDlgRtpParams;
    VlxDisplayParametersDialog *m_vlxDlgDispParams;

};

#endif // MAINWINDOW_H
