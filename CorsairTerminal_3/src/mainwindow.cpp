#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QThread>

#include <QFileDialog>

#include <QLabel>

#include <qmv/tools/accessor.h>
#include <qmv/tools/help.h>
#include <qmv/tools/about.h>

#include "version.h"

#include "eth/ethcontroller.h"

#include "program_settings/model/ethmodel.h"

#include "program_settings/view/ethview.h"

#include "controller.h"
#include "device_parts/channel/abstractchannel.h"

#include "pluggable_controls/corsairkeyfilter.h"
#include "pluggable_controls/joystick.h"
#include "pluggable_controls/panel.h"

#include "device_parts/gsp/gspcontroller.h"

#include "aids/model/telemetrymodel.h"
#include "aids/view/telemetryview.h"

#include "device_parts/model/tvctrlmodel.h"
#include "device_parts/model/swirctrlmodel.h"
#include "device_parts/model/mwirctrlmodel.h"
#include "device_parts/model/highlightermodel.h"
#include "device_parts/model/gspctrlmodel.h"
#include "device_parts/model/fusionctrlmodel.h"
#include "program_settings/model/terminalsettingsmodel.h"

#include "device_parts/view/tvctrlview.h"
#include "device_parts/view/swirctrlview.h"
#include "device_parts/view/mwirctrlview.h"
#include "device_parts/view/highlighterview.h"
#include "device_parts/view/gspctrlview.h"
#include "device_parts/view/fusionctrlview.h"
#include "aids/view/vdbMsgSenderView.h"
#include "program_settings/view/terminalsettingsview.h"

#include "device_parts/vlx/vlx_stream_channel.h"
#include "vlx_common/vlx_settings.h"
#include "vlx_common/vlx_cmd_info_dialog.h"

#include "tools/imgp.h"

#include "device_parts/cmd/vdb_registers.h"
#include "device_parts/cmd/cmdmanager.h"
#include "device_parts/cmd/cmd.h"


#include <QTimer>
#include "tools/JoystickWidget.hpp"
#include "tools/gsp_initialization.h"

#include <QDesktopWidget>
#include <QScreen>

#include "tools/logger.h"
Q_LOGGING_CATEGORY(logMainWindow, "MainWindow")

#include "tools/resource.h"
#define RESOURCE_SECTION    ("mainwindow")
#define RESOURCE_FILENAME   ("rsc/mainwindow.xml")


#define OUT_F(x) (quint64)(&x) << "/" << sizeof(x)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{


/*

    auto &p = *((Telemetry::t_telemetry_struct *)0);

    qDebug() << "====== Telemetry size: " << sizeof(p);
    qDebug() << " <tv>              :" << OUT_F(p.tv);
    qDebug() << " <sw>              :" << OUT_F(p.sw);
    qDebug() << " <mw>              :" << OUT_F(p.mw);
    qDebug() << " <rf>              :" << OUT_F(p.rf);
    qDebug() << " <gsp>             :" << OUT_F(p.gsp);
    qDebug() << " <controller>      :" << OUT_F(p.controller);
    qDebug() << " <tracker>         :" << OUT_F(p.tracker);
    qDebug() << "  <tracker_chan..> :" << OUT_F(p.tracker.tracker_channel);
    qDebug() << "  <target_id>      :" << OUT_F(p.tracker.target_id);
    qDebug() << "  <targets[0]>     :" << OUT_F(p.tracker.targets[0]);
    qDebug() << "  <targets[1]>     :" << OUT_F(p.tracker.targets[1]);
    qDebug() << "  <targets[2]>     :" << OUT_F(p.tracker.targets[2]);
    qDebug() << "  <targets[3]>     :" << OUT_F(p.tracker.targets[3]);
    qDebug() << "   <target>        :" << OUT_F(p.tracker.targets[3].target);
    qDebug() << "   <x_center>      :" << OUT_F(p.tracker.targets[3].x_center);
    qDebug() << "   <y_center>      :" << OUT_F(p.tracker.targets[3].y_center);
    qDebug() << "   <x_size>        :" << OUT_F(p.tracker.targets[3].x_size);
    qDebug() << "   <y_size>        :" << OUT_F(p.tracker.targets[3].y_size);
    qDebug() << "   <z_angle_cen..> :" << OUT_F(p.tracker.targets[3].z_angle_center);
    qDebug() << "   <x_angle_cen..> :" << OUT_F(p.tracker.targets[3].x_angle_center);
    qDebug() << "   <z_angle_size>  :" << OUT_F(p.tracker.targets[3].z_angle_size);
    qDebug() << "   <x_angle_size>  :" << OUT_F(p.tracker.targets[3].x_angle_size);
    qDebug() << " <agc>             :" << OUT_F(p.agc);
    qDebug() << " <inversion_mw>    :" << OUT_F(p.inversion_mw);
    qDebug() << " <dev>             :" << OUT_F(p.dev);
    qDebug() << " <add>             :" << OUT_F(p.add);
*/
    // QTimer::singleShot(0, qApp, &QCoreApplication::quit);

    // return;

    qRegisterMetaType<uint8_t *>("uint8_t *");
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<uint16_t>("uint16_t");
    qRegisterMetaType<Cmd>("Cmd");
    qRegisterMetaType<Telemetry>("Telemetry");

    _ui->setupUi(this);

    QTimer::singleShot(4000, this, [this]{
        on_channel_tvrgb_clicked();
    });

    // _ui->virtualJoystickWidget->setVisible(false);
    m_vlxDlgRtpParams = new VlxMediaParametersDialog(this);
    m_vlxDlgDispParams = new VlxDisplayParametersDialog(this);
    auto vlxDlgCmdMon = new VlxCmdInfoDialog(this);

    connect(_ui->actOpenVlxRtpSettingsDialog, &QAction::triggered, m_vlxDlgRtpParams, &QDialog::open);
    connect(_ui->actOpenVlxDispSettingsDialog, &QAction::triggered, m_vlxDlgDispParams, &QDialog::open);
    connect(_ui->actCmdMon, &QAction::triggered, vlxDlgCmdMon,  &VlxCmdInfoDialog::openMonitor);


    _status_vlx_idmx = new QLabel(this);
    _status_mouse_pos = new QLabel(this);
    _status_mouse_pos->setText(" (0, 0) ");
    _status_fps = new QLabel(this);
    _status_fps->setText(" 25.000 ");
    _status_connection = new QLabel(this);
    _status_connection->setText(" Соединение: Нет ");
    _status_joystick = new QLabel(this);
    _status_joystick->setText(" Джойстик: Нет ");
//    _status_panel = new QLabel(this);
//    _status_panel->setText(" Пульт: Нет ");

    statusBar()->addWidget(_status_mouse_pos);
    statusBar()->addWidget(_status_fps);
    statusBar()->addPermanentWidget(_status_vlx_idmx);
    statusBar()->addPermanentWidget(_status_connection);
    statusBar()->addPermanentWidget(_status_joystick);
//    statusBar()->addPermanentWidget(_status_panel);


    load();

    init_controller();
    vlxDlgCmdMon->connectCmdManger(_controller->cmdManger());
    init_tvCtrl();

    if (_dev_type == DEV_TYPES::en_dev_type_350) {
       init_swirCtrl();
    } else if(_dev_type == DEV_TYPES::en_dev_type_350p) {

    }
    serviceCmdTurnOff();


    init_mwirCtrl();
    init_highlighterView();
    init_gspCtrl();
    init_fusionCtrl();
    init_vdbMsgSenderView();
    init_telemetry();
    init_logger();
    init_iterface();
    init_keyfilter();
    init_joystick();
    init_panel();
    init_terminalSettings();

    _window_title = "Терминал ОЭЦН/ГОЭН-220.";

#if defined(VLX_STREAM_TYPE_RTP_MPEG_TS)
    _window_title += " ";
    _window_title += "[RTP MPEG TS]";
#elif defined(VLX_STREAM_TYPE_SRT_MPEG_TS)
    _window_title += " ";
    _window_title += "[SRT MPEG TS]";
#endif
    _window_title += (" <cборка: " + QString(BUILD_NUMBER) + ">");


    setWindowTitle(_window_title);
    setWindowState(Qt::WindowMaximized);

//    on_host_etld_clicked(_ui->host_etld->isChecked());
    on_inverted_position_clicked(_ui->inverted_position->isChecked());

//     _ui->motor_off->setShortcut(Qt::Key_P);

//    blockAllInterface(true);
//    QTimer::singleShot(15000, this, SLOT(sl_blockInterfaceForArretiresOff()));

    _ui->newJoystik->move(0, 1000);
    _ui->newJoystik->setHidden(true);
    connect(_ui->newJoystik, &JoystickWidget::sg_newExcursion, _controller, &Controller::gsp_speed_set, Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    m_joystikTimer = new QTimer();
//    m_joystikTimer->setInterval(5000);
//    connect(m_joystikTimer, &QTimer::timeout, this, &MainWindow::sl_alignJoystikWidget);
//    m_joystikTimer->start();

    init_gspInitializationModul();

    on_angleZ_Max_180_clicked();

    m_fixAngle.enable = false;
    m_fixAngle.rememberAngles = false;
    m_fixAngle.angleCtrlOn = false;
    m_fixAngle.tmrPeriodAnglCtrlON = 100;
    m_fixAngle.tmrPeriodAnglCtrlOFF = 100;

    QTimer::singleShot(200, this, [this]{

        auto _idmx = VlxStreamChannel::demuxer();
        if (_idmx == nullptr)
            return;
        auto idmx = _idmx.toStrongRef();
        QObject::connect(idmx.get(), &Vlx::Av::InputDemuxer::stateChanged, this, &MainWindow::onVlxIdmxStateChanged);
        onVlxIdmxStateChanged(idmx->state());
    });
}

void MainWindow::init_controller()
{
    _controller = new Controller(this, _dev_type);
    connect(_controller, SIGNAL(image_ready()), SLOT(image_ready()));
    connect(_ui->video_view, SIGNAL(new_object(bool,int,int,int,int)), _controller, SLOT(new_object(bool,int,int,int,int)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_ui->video_view, SIGNAL(video_click(int,int)), _controller, SLOT(video_click(int,int)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_ui->video_view, SIGNAL(stop_object(bool)), _controller, SLOT(stop_object(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_ui->video_view, SIGNAL(mouse_pos(int,int)), this, SLOT(mouse_pos(int,int)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_controller, SIGNAL(response(Telemetry)), this, SLOT(response(Telemetry)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_controller, &Controller::channelChanged, this, &MainWindow::onChannelChanged);
    _controller->start();
}
void MainWindow::init_tvCtrl()
{
    // Model
    _tvCtrlModel = new TVCtrlModel(this, TVCtrlModel_id, "tvCtrlModel");
    // View
    _tvCtrlView = new TVCtrlView();
    _tvCtrlView->setWindowTitle("ТВ");
    _tvCtrlView->setWindowModality(Qt::WindowModal);
    _tvCtrlView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _tvCtrlView->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    _tvCtrlView->resize(600, 200);
    _tvCtrlView->set_model(_tvCtrlModel);

    connect(_controller, SIGNAL(response(Cmd)), _tvCtrlModel, SLOT(response(Cmd)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_controller, SIGNAL(response(Telemetry)), _tvCtrlModel, SLOT(response(Telemetry)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_controller, SIGNAL(set_autofocus_host_pause(bool)), _tvCtrlModel, SLOT(set_autofocus_host_pause(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_tvCtrlModel, SIGNAL(zoom_in(double, uint32_t)), _controller, SLOT(zoom_in(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(zoom_stop(uint32_t)), _controller, SLOT(zoom_stop(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(zoom_out(double, uint32_t)), _controller, SLOT(zoom_out(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(zoom_pos(uint32_t, uint32_t)), _controller, SLOT(zoom_pos(uint32_t, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(focus_near(double, uint32_t)), _controller, SLOT(focus_near(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(focus_stop(uint32_t)), _controller, SLOT(focus_stop(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(focus_far(double, uint32_t)), _controller, SLOT(focus_far(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(focus_pos(uint32_t, uint32_t)), _controller, SLOT(focus_pos(uint32_t, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(iris_close(double, uint32_t)), _controller, SLOT(iris_close(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(iris_stop(uint32_t)), _controller, SLOT(iris_stop(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(iris_open(double, uint32_t)), _controller, SLOT(iris_open(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(field_sync_narrow(uint32_t)), _controller, SLOT(field_sync_narrow(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(field_sync(uint32_t)), _controller, SLOT(field_sync(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(field_sync_reg_switch(uint32_t)), _controller, SLOT(field_sync_reg_switch(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(autofocus(uint32_t)), _controller, SLOT(autofocus(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(autofocus_en(uint32_t)), _controller, SLOT(autofocus_en(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(autofocus_dis(uint32_t)), _controller, SLOT(autofocus_dis(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(agc_switch(uint32_t)), _controller, SLOT(agc_switch(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(thermostat_on(uint32_t)), _controller, SLOT(thermostat_on(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(thermostat_off(uint32_t)), _controller, SLOT(thermostat_off(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(heat_on(uint32_t)), _controller, SLOT(heat_on(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_tvCtrlModel, SIGNAL(heat_off(uint32_t)), _controller, SLOT(heat_off(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
}
void MainWindow::init_swirCtrl()
{
    // Model
    _swirCtrlModel = new SWIRCtrlModel(this, SWIRCtrlModel_id, "swirCtrlModel");
    // View
    _swirCtrlView = new SWIRCtrlView();
    _swirCtrlView->setWindowTitle("КВИК");
    _swirCtrlView->setWindowModality(Qt::WindowModal);
    _swirCtrlView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _swirCtrlView->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    _swirCtrlView->resize(600, 200);
    _swirCtrlView->set_model(_swirCtrlModel);

    connect(_controller, SIGNAL(response(Cmd)), _swirCtrlModel, SLOT(response(Cmd)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_controller, SIGNAL(response(Telemetry)), _swirCtrlModel, SLOT(response(Telemetry)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_controller, SIGNAL(set_autofocus_host_pause(bool)), _swirCtrlModel, SLOT(set_autofocus_host_pause(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_swirCtrlModel, SIGNAL(zoom_in(double, uint32_t)), _controller, SLOT(zoom_in(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(zoom_stop(uint32_t)), _controller, SLOT(zoom_stop(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(zoom_out(double, uint32_t)), _controller, SLOT(zoom_out(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(zoom_pos(uint32_t, uint32_t)), _controller, SLOT(zoom_pos(uint32_t, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(focus_near(double, uint32_t)), _controller, SLOT(focus_near(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(focus_stop(uint32_t)), _controller, SLOT(focus_stop(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(focus_far(double, uint32_t)), _controller, SLOT(focus_far(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(focus_pos(uint32_t, uint32_t)), _controller, SLOT(focus_pos(uint32_t, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(iris_close(double, uint32_t)), _controller, SLOT(iris_close(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(iris_stop(uint32_t)), _controller, SLOT(iris_stop(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(iris_open(double, uint32_t)), _controller, SLOT(iris_open(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(field_sync_narrow(uint32_t)), _controller, SLOT(field_sync_narrow(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(field_sync(uint32_t)), _controller, SLOT(field_sync(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(field_sync_reg_switch(uint32_t)), _controller, SLOT(field_sync_reg_switch(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(autofocus(uint32_t)), _controller, SLOT(autofocus(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(autofocus_en(uint32_t)), _controller, SLOT(autofocus_en(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(autofocus_dis(uint32_t)), _controller, SLOT(autofocus_dis(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(agc_switch(uint32_t)), _controller, SLOT(agc_switch(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(thermostat_on(uint32_t)), _controller, SLOT(thermostat_on(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(thermostat_off(uint32_t)), _controller, SLOT(thermostat_off(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(heat_on(uint32_t)), _controller, SLOT(heat_on(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_swirCtrlModel, SIGNAL(heat_off(uint32_t)), _controller, SLOT(heat_off(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
}
void MainWindow::init_mwirCtrl()
{
    // Model
    _mwirCtrlModel = new MWIRCtrlModel(this, MWIRCtrlModel_id, "mwirCtrlModel");
    // View
    _mwirCtrlView = new MWIRCtrlView();
    _mwirCtrlView->setWindowTitle("ТПВ-С");
    _mwirCtrlView->setWindowModality(Qt::WindowModal);
    _mwirCtrlView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _mwirCtrlView->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    _mwirCtrlView->resize(600, 200);
    _mwirCtrlView->set_model(_mwirCtrlModel);

    connect(_controller, SIGNAL(response(Cmd)), _mwirCtrlModel, SLOT(response(Cmd)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_controller, SIGNAL(response(Telemetry)), _mwirCtrlModel, SLOT(response(Telemetry)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_controller, SIGNAL(set_autofocus_host_pause(bool)), _mwirCtrlModel, SLOT(set_autofocus_host_pause(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_mwirCtrlModel, SIGNAL(zoom_in(double, uint32_t)), _controller, SLOT(zoom_in(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(zoom_stop(uint32_t)), _controller, SLOT(zoom_stop(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(zoom_out(double, uint32_t)), _controller, SLOT(zoom_out(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(zoom_pos(uint32_t, uint32_t)), _controller, SLOT(zoom_pos(uint32_t, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(focus_near(double, uint32_t)), _controller, SLOT(focus_near(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(focus_stop(uint32_t)), _controller, SLOT(focus_stop(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(focus_far(double, uint32_t)), _controller, SLOT(focus_far(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(focus_pos(uint32_t, uint32_t)), _controller, SLOT(focus_pos(uint32_t, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(iris_close(double, uint32_t)), _controller, SLOT(iris_close(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(iris_stop(uint32_t)), _controller, SLOT(iris_stop(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(iris_open(double, uint32_t)), _controller, SLOT(iris_open(double, uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(field_sync_narrow(uint32_t)), _controller, SLOT(field_sync_narrow(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(field_sync(uint32_t)), _controller, SLOT(field_sync(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(field_sync_reg_switch(uint32_t)), _controller, SLOT(field_sync_reg_switch(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(autofocus(uint32_t)), _controller, SLOT(autofocus(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(autofocus_en(uint32_t)), _controller, SLOT(autofocus_en(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(autofocus_dis(uint32_t)), _controller, SLOT(autofocus_dis(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(agc_switch(uint32_t)), _controller, SLOT(agc_switch(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(inversion_mw()), _controller, SLOT(inversion_mw()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(thermostat_on(uint32_t)), _controller, SLOT(thermostat_on(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(thermostat_off(uint32_t)), _controller, SLOT(thermostat_off(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(heat_on(uint32_t)), _controller, SLOT(heat_on(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_mwirCtrlModel, SIGNAL(heat_off(uint32_t)), _controller, SLOT(heat_off(uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_mwirCtrlModel, SIGNAL(vdb_write_request(const uint8_t, const uint8_t, const uint32_t)), _controller, SLOT(vdb_write_request(const uint8_t, const uint8_t, const uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

}
void MainWindow::init_highlighterView()
{
    // Model
    _highlighterModel = new HighlighterModel(this, HighlighterModel_id, "highlighterModel");
    // View
    _highlighterView = new HighlighterView();
    _highlighterView->setWindowTitle("Подсветчик");
    _highlighterView->setWindowModality(Qt::WindowModal);
    _highlighterView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _highlighterView->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    _highlighterView->resize(600, 100);
    _highlighterView->set_model(_highlighterModel);

    connect(_controller, SIGNAL(response(Cmd)), _highlighterModel, SLOT(response(Cmd)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_controller, SIGNAL(response(Telemetry)), _highlighterModel, SLOT(response(Telemetry)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_highlighterView, SIGNAL(vdb_read_request(const uint8_t, const uint8_t, const uint16_t)),
            _controller, SLOT(vdb_read_request(const uint8_t, const uint8_t, const uint16_t)),
            Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_highlighterView, SIGNAL(vdb_write_request(const uint8_t, const uint8_t, const uint32_t)),
            _controller, SLOT(vdb_write_request(const uint8_t, const uint8_t, const uint32_t)),
            Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_highlighterView, SIGNAL(vdb_write_request_array(const uint8_t, const uint8_t, const std::vector<uint8_t>&)),
            _controller, SLOT(vdb_write_request_array(const uint8_t, const uint8_t, const std::vector<uint8_t>&)),
            Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_highlighterView, SIGNAL(signal_stop_radiation()),          _controller,
            SLOT(lrf_stop()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_highlighterView, SIGNAL(signal_start_measurement()), _controller,
            SLOT(lrf_start_mono()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_highlighterView, SIGNAL(signal_start_illumination()), _controller,
            SLOT(lrf_start_5Hz()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_highlighterView, SIGNAL(signal_power_on_switch()), _controller,
            SLOT(lrf_enable_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_highlighterModel, SIGNAL(signal_send_msg(Packet&)), _controller, SLOT(send_msg(Packet&)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_highlighterModel, SIGNAL(signal_set_frequency(uint8_t)), _controller, SLOT(lrf_set_frequency(uint8_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_highlighterModel, SIGNAL(vdb_write_request(const uint8_t, const uint8_t, const uint32_t)), _controller, SLOT(vdb_write_request(const uint8_t, const uint8_t, const uint32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_highlighterView,  SIGNAL(signal_send_msg(Packet&)), _controller, SLOT(send_msg(Packet&)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_highlighterModel, SIGNAL(signal_dangerZone_en(const bool, const t_lrf_danger_zone_params)),
            _controller, SLOT(lrf_dangerZone_en(const bool, const t_lrf_danger_zone_params)),
            Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

}
void MainWindow::init_gspCtrl()
{
    // Model
    _gspCtrlModel = new GSPCtrlModel(_dev_type, this, GSPCtrlModel_id, "gspCtrlModel");
    _gspCtrlModel->set_gsp_controller(_controller->get_gsp_controller());
    // View
    _gspCtrlView = new GSPCtrlView();
    _gspCtrlView->setController(_controller);
    _gspCtrlView->setWindowTitle("Платформа");
    _gspCtrlView->setWindowModality(Qt::WindowModal);
    _gspCtrlView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _gspCtrlView->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    _gspCtrlView->resize(800, 200);
    _gspCtrlView->set_model(_gspCtrlModel);

    connect(_controller, SIGNAL(response(Cmd)), _gspCtrlModel, SLOT(response(Cmd)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_controller, SIGNAL(response(Telemetry)), _gspCtrlModel, SLOT(response(Telemetry)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_gspCtrlModel, SIGNAL(gsp_speed_set(double,double)), _controller, SLOT(gsp_speed_set(double,double)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_gspCtrlModel, SIGNAL(gsp_absolute_angle_set(double,double)), _controller, SLOT(gsp_absolute_angle_set(double,double)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_gspCtrlModel, SIGNAL(gsp_relative_angle_set(double,double)), _controller, SLOT(gsp_relative_angle_set(double,double)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_gspCtrlModel, SIGNAL(gsp_speed_f_set(double,double)), _controller, SLOT(gsp_speed_set_deg(double,double)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_gspCtrlModel, SIGNAL(gsp_arretir_set()), _controller, SLOT(gsp_arretir_set()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_gspCtrlModel, SIGNAL(gsp_arretir_reset()), _controller, SLOT(gsp_arretir_reset()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_gspCtrlModel, SIGNAL(gsp_stabilization_switch()), _controller, SLOT(gsp_stabilization_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_gspCtrlModel, SIGNAL(gsp_park()), _controller, SLOT(gsp_park()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_gspCtrlModel, SIGNAL(gsp_null_position()), _controller, SLOT(gsp_null_position()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_gspCtrlModel, SIGNAL(gsp_pohod_position()), _controller, SLOT(gsp_pohod_position()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_gspCtrlModel, SIGNAL(gsp_polyot_position()), _controller, SLOT(gsp_polyot_position()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_gspCtrlModel, SIGNAL(gsp_alignment()), _controller, SLOT(gsp_alignment()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_gspCtrlModel, SIGNAL(gsp_angle_ctrl_mode()), _controller, SLOT(gsp_angle_ctrl_mode()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_gspCtrlModel, SIGNAL(gsp_velocity_ctrl_mode()), _controller, SLOT(gsp_velocity_ctrl_mode()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_gspCtrlModel, SIGNAL(gsp_drift(int32_t,int32_t)), _controller, SLOT(gsp_drift(int32_t,int32_t)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    // Сигналы от модели ГСП на отправку сообщений vdb
    connect(_gspCtrlModel, SIGNAL(vdb_read_request(uint8_t,uint8_t,uint16_t)),
            _controller, SLOT(vdb_read_request(uint8_t,uint8_t,uint16_t)),
            Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_gspCtrlModel, SIGNAL(vdb_write_request(uint8_t,uint8_t,uint32_t)),
            _controller, SLOT(vdb_write_request(uint8_t,uint8_t,uint32_t)),
            Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_gspCtrlModel, SIGNAL(vdb_write_request_array(uint8_t,uint8_t,std::vector<uint8_t>&)),
            _controller, SLOT(vdb_write_request_array(uint8_t,uint8_t,std::vector<uint8_t>&)),
            Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_controller, SIGNAL(host_etld_state_changed(bool)), _gspCtrlModel, SLOT(gsp_host_etld_info(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_gspCtrlModel, SIGNAL(signal_send_msg(Packet&)), _controller, SLOT(send_msg(Packet&)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    _gspCtrlModel->update();
}
void MainWindow::init_fusionCtrl()
{
    // Model
    _fusionCtrlModel = new FusionCtrlModel(this, FusionCtrlModel_id, "fusionCtrlModel");
    // View
    _fusionCtrlView = new FusionCtrlView();
    _fusionCtrlView->setWindowTitle("Комплексирование");
    _fusionCtrlView->setWindowModality(Qt::WindowModal);
    _fusionCtrlView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _fusionCtrlView->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    _fusionCtrlView->resize(320, 240);
    _fusionCtrlView->set_model(_fusionCtrlModel);

    connect(_controller, SIGNAL(response(Cmd)), _fusionCtrlModel, SLOT(response(Cmd)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_controller, SIGNAL(response(Telemetry)), _fusionCtrlModel, SLOT(response(Telemetry)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_fusionCtrlModel, SIGNAL(enable_changed(bool)), _controller, SLOT(tvgraymwir_enable_changed(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_fusionCtrlModel, SIGNAL(method_changed(int)), _controller, SLOT(tvgraymwir_method_changed(int)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_fusionCtrlModel, SIGNAL(offset_x_changed(int)), _controller, SLOT(tvgraymwir_offset_x_changed(int)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_fusionCtrlModel, SIGNAL(offset_y_changed(int)), _controller, SLOT(tvgraymwir_offset_y_changed(int)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_fusionCtrlModel, SIGNAL(method_agc_en_changed(bool)), _controller, SLOT(tvgraymwir_method_agc_en_changed(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_fusionCtrlModel, SIGNAL(method_c_changed(double)), _controller, SLOT(tvgraymwir_method_c_changed(double)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    _fusionCtrlModel->update();
}
void MainWindow::init_vdbMsgSenderView()
{
    _vdbMsgSenderView = new vdbMsgSenderView();
    _vdbMsgSenderView->setWindowTitle("Отправление произвольных команд");
    _vdbMsgSenderView->setWindowModality(Qt::WindowModal);
    _vdbMsgSenderView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _vdbMsgSenderView->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    connect(_vdbMsgSenderView, SIGNAL(signal_send_msg(Packet&)), _controller, SLOT(send_msg(Packet&)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_controller, SIGNAL(response(Packet)), _vdbMsgSenderView, SLOT(reply_to_msg(Packet)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
}
void MainWindow::init_terminalSettings()
{
    // Interface Model
    _terminalSettingsModel = new terminalsettingsmodel(this, TerminalSettings_id, "terminalSettings");
    // Interface View
    _terminalSettingsView = new terminalsettingsview();
    _terminalSettingsView->setWindowTitle("Настройки терминала");
    _terminalSettingsView->setWindowModality(Qt::WindowModal);
    _terminalSettingsView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _terminalSettingsView->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    _terminalSettingsView->resize(400, 100);
    _terminalSettingsView->set_model(_terminalSettingsModel);

    connect(_terminalSettingsModel, SIGNAL(click_control_changed(bool)),     _controller, SLOT(click_control(bool)),     Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_terminalSettingsModel, SIGNAL(tracker_deviation_changed(bool)), _controller, SLOT(tracker_deviation(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_terminalSettingsModel, SIGNAL(panel_changed(bool)),             _panel,       SLOT(set_enable(bool)),       Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_terminalSettingsModel, SIGNAL(infographics_on_screen_changed(bool)),  _controller, SLOT(infographics_on_screen(bool)),  Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_terminalSettingsModel, SIGNAL(infographics_on_record_changed(bool)),  _controller, SLOT(infographics_on_record(bool)),  Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_terminalSettingsModel, SIGNAL(infographics_on_picture_changed(bool)), _controller, SLOT(infographics_on_picture(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_terminalSettingsModel, SIGNAL(infographics_angleInversion_X_changed(bool)), _controller, SLOT(infographics_angleInversion_X(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_terminalSettingsModel, SIGNAL(infographics_angleInversion_Z_changed(bool)), _controller, SLOT(infographics_angleInversion_Z(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));


    connect(_terminalSettingsModel, SIGNAL(keyboard_inv_x(bool)),   _keyfilter, SLOT(set_inverse_x(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_terminalSettingsModel, SIGNAL(keyboard_inv_z(bool)),   _keyfilter, SLOT(set_inverse_z(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_terminalSettingsModel, SIGNAL(keyboard_inv_x(bool)),   _gspCtrlModel, SLOT(set_inverse_x(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_terminalSettingsModel, SIGNAL(keyboard_inv_z(bool)),   _gspCtrlModel, SLOT(set_inverse_z(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    QObject::connect(_terminalSettingsView, &terminalsettingsview::sg_infographic_setShipPointer, _controller, &Controller::sl_infographic_setShipPointer, Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    QObject::connect(_terminalSettingsView, &terminalsettingsview::sg_infographic_setHelicopterPointer, _controller, &Controller::sl_infographic_setHelicopterPointer, Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    _terminalSettingsModel->update_settings();
    emit _terminalSettingsView->sg_infographic_setHelicopterPointer();
}

void MainWindow::init_iterface()
{
    // Interface Model
    _interfaceModel = new EthModel(this, TerminalSettings_id, "ethModel");
    // Interface View
    _interfaceView = new EthView();
    _interfaceView->setWindowTitle("Ethernet Настройки");
    _interfaceView->setWindowModality(Qt::WindowModal);
    _interfaceView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _interfaceView->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    _interfaceView->resize(400, 100);
    _interfaceView->set_model(_interfaceModel);
}
void MainWindow::init_telemetry()
{
    // Model
    _telemetryModel = new TelemetryModel(_dev_type, this, TelemetryModel_id, "telemetryModel");
    // View
    _telemetryView = new TelemetryView();
    _telemetryView->setWindowTitle("Телеметрия");
    _telemetryView->setWindowModality(Qt::WindowModal);
    _telemetryView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _telemetryView->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    _telemetryView->resize(500, 350);
    _telemetryView->set_model(_telemetryModel);

    connect(_controller, SIGNAL(response(Cmd)), _telemetryModel, SLOT(response(Cmd)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_controller, SIGNAL(response(Telemetry)), _telemetryModel, SLOT(response(Telemetry)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
}
void MainWindow::init_keyfilter()
{
    _keyfilter = new CorsairKeyFilter(this);

    installEventFilter(_keyfilter);

    connect(_keyfilter, SIGNAL(tvrgb_on()),        this, SLOT(on_channel_tvrgb_clicked()),     Qt::DirectConnection);
    connect(_keyfilter, SIGNAL(tvgray_on()),       this, SLOT(on_channel_tvgray_clicked()),    Qt::DirectConnection);
    connect(_keyfilter, SIGNAL(mwir_on()),         this, SLOT(on_channel_mwir_clicked()),      Qt::DirectConnection);
//    connect(_keyfilter, SIGNAL(tvgraymwir_on()),   this, SLOT(on_channel_tvgraymwir_clicked()),Qt::DirectConnection);
//    connect(_keyfilter, SIGNAL(fusion_on()),       this, SLOT(on_channel_fusion_clicked()),    Qt::DirectConnection);
//    connect(_keyfilter, SIGNAL(test_on()),         this, SLOT(on_channel_test_clicked()),      Qt::DirectConnection);
    connect(_keyfilter, SIGNAL(agc_switch()), _controller, SLOT(agc_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    if (_dev_type == DEV_TYPES::en_dev_type_350 || _dev_type == DEV_TYPES::en_dev_type_obzor) {
//        connect(_keyfilter, SIGNAL(swir_on()),         this, SLOT(on_channel_swir_clicked()),      Qt::DirectConnection);
//        connect(_keyfilter, SIGNAL(swir_gain_switch()), _controller, SLOT(swir_gain_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    }
    connect(_keyfilter, SIGNAL(zoom_in()), _controller, SLOT(zoom_in()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(zoom_out()), _controller, SLOT(zoom_out()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(zoom_stop()), _controller, SLOT(zoom_stop()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(focus_near()), _controller, SLOT(focus_near()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(focus_far()), _controller, SLOT(focus_far()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(focus_stop()), _controller, SLOT(focus_stop()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(iris_close()), _controller, SLOT(iris_close()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(iris_open()), _controller, SLOT(iris_open()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(iris_stop()), _controller, SLOT(iris_stop()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(field_sync_narrow()), _controller, SLOT(field_sync_narrow()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(field_sync()), _controller, SLOT(field_sync()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(field_sync_reg_switch()), _controller, SLOT(field_sync_reg_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(autofocus()), _controller, SLOT(autofocus()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_keyfilter, SIGNAL(gsp_speed_set(const double &, const double &)), _controller, SLOT(gsp_speed_set(const double &, const double &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(gsp_absolute_angle_set(const double &, const double &)), _controller, SLOT(gsp_absolute_angle_set(const double &, const double &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(gsp_relative_angle_set(const double &, const double &)), _controller, SLOT(gsp_relative_angle_set(const double &, const double &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(gsp_arretir_reset()), _controller, SLOT(gsp_arretir_reset()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(gsp_stabilization_switch()), _controller, SLOT(gsp_stabilization_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(gsp_park()), _controller, SLOT(gsp_park()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(gsp_arretir_set()), _controller, SLOT(gsp_arretir_set()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(gsp_null_position()), _controller, SLOT(gsp_null_position()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_keyfilter, SIGNAL(digital_zoom_in()), _controller, SLOT(digital_zoom_in()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(digital_zoom_out()), _controller, SLOT(digital_zoom_out()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(pic_in_pic()), _controller, SLOT(pic_in_pic()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(inversion()), _controller, SLOT(inversion_mw()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(brightness_up()), _controller, SLOT(brightness_up()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(brightness_down()), _controller, SLOT(brightness_down()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(contrast_up()), _controller, SLOT(contrast_up()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(contrast_down()), _controller, SLOT(contrast_down()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_keyfilter, SIGNAL(lrf_enable_switch()), _controller, SLOT(lrf_enable_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(lrf_mode_switch()), _controller, SLOT(lrf_mode_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(lrf_start()), _controller, SLOT(lrf_start()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_keyfilter, SIGNAL(target_enable_switch()), _controller, SLOT(target_enable_switch_multi()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(target_capture()), _controller, SLOT(target_capture_multi()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(target_free()), _controller, SLOT(target_free_multi()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(target_window_set(const double &)), _controller, SLOT(target_window_set(const double &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(target_id_switch()), _controller, SLOT(target_id_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(target_choose()), _controller, SLOT(target_choose()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(target_fast_switch()), _controller, SLOT(target_fast_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_keyfilter, SIGNAL(video_record_switch()), _controller, SLOT(video_record_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_keyfilter, SIGNAL(image_save()), _controller, SLOT(image_save_on()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_keyfilter, SIGNAL(write_dump(const bool&)), _telemetryModel, SLOT(write_dump(const bool&)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    connect(_keyfilter, SIGNAL(full_screen()),     this, SLOT(full_screen()),                  Qt::DirectConnection);
//    connect(_keyfilter, SIGNAL(next_channel()),    this, SLOT(next_channel()),                 Qt::DirectConnection);
//    connect(_keyfilter, SIGNAL(prev_channel()),    this, SLOT(prev_channel()),                 Qt::DirectConnection);
    connect(_keyfilter, SIGNAL(help()),            this, SLOT(on_help_triggered()),            Qt::DirectConnection);
    connect(_keyfilter, SIGNAL(turnOnImitateJoystick()), this, SLOT(turnOnImitateJoystick()),            Qt::DirectConnection);
//    connect(_keyfilter, SIGNAL(telemetry()),       this, SLOT(on_telemetry_triggered()),       Qt::DirectConnection);
//    connect(_keyfilter, SIGNAL(settings()),        this, SLOT(settings()),                     Qt::DirectConnection);
//    connect(_keyfilter, SIGNAL(gspCtrl()),         this, SLOT(on_gspCtrl_triggered()),         Qt::DirectConnection);
//    connect(_keyfilter, SIGNAL(connectionCtrl()),  this, SLOT(on_connection_triggered()),      Qt::DirectConnection);
}
void MainWindow::init_joystick()
{
    m_vlxJoystickMapping = new Vlx::Input::JoystickMappingForm(this);
    m_vlxJoystick = new Vlx::Input::JoystickMapper(this);

    // --- Connection setup ------

    connect(_ui->actJoystickSetup, &QAction::triggered, this, [this]{
        qDebug() << "[VLX JOYSTICK]: STOP";
        m_vlxJoystick->stop();
        m_vlxJoystickMapping->open();
    });

    connect(m_vlxJoystickMapping, &Vlx::Input::JoystickMappingForm::mappingChanged, this, [this]{
        qDebug() << "[VLX JOYSTICK]: RELOAD MAPPING";
        m_vlxJoystick->reloadMapping();
    });

    connect(m_vlxJoystickMapping, &Vlx::Input::JoystickMappingForm::finished, this, [this]{
        qDebug() << "[VLX JOYSTICK]: START";
        m_vlxJoystick->start();
    });


    connect(VlxDisplayParametersDialog::inst(), &VlxDisplayParametersDialog::settingsSaved, this, [this]{
        m_vlxJoystick->setPollInterval(VlxSettings::i().joystickPollIntervalMs());
    });

    connect(m_vlxJoystick, &Vlx::Input::JoystickMapper::opened, this, [this](const QString &name) {
        _status_joystick->setText("Джойстик: " + name);
    });


    // --- Binding AXES ------


    m_vlxJoystick->bindAxis(Vlx::Input::Cmd::Axis_Pitch, [this](double v) {
        static double prev = 0.0f;
        if (v == prev)
            return;
        // if (abs(v) < 1e-5)
        //     return;
        prev = v;
        m_vlxJoystickPitchValue = v;
        _controller->gsp_speed_set(m_vlxJoystickYawValue, m_vlxJoystickPitchValue);
    });

    m_vlxJoystick->bindAxis(Vlx::Input::Cmd::Axis_Yaw, [this](double v) {
        static double prev = 0.0f;
        if (v == prev)
            return;
        // if (abs(v) < 1e-5)
        //     return;
        prev = v;
        m_vlxJoystickYawValue = v;
        _controller->gsp_speed_set(m_vlxJoystickYawValue, m_vlxJoystickPitchValue);
    });

    m_vlxJoystick->bindAxis(Vlx::Input::Cmd::Axis_TrackingTargetSize, [this](double v) {
        static double prev = 0.0f;
        if (v == prev)
            return;
        prev = v;
        _controller->target_window_set(v);
    });


    // --- Binding ACTIONS ------


    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_TrackingOn, [this]{ _controller->target_set_multi_enabled(true); });
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_TrackingCapture,  [this]{_controller->target_capture_multi();});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_TrackingOff,  [this]{_controller->target_set_multi_enabled(false);});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_ZoomIn,   [this]{_controller->zoom_out();}); // из за ошибки в протоколе, меняем местами in|out
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_ZoomOut,  [this]{_controller->zoom_in();});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_ZoomStop,     [this]{_controller->zoom_stop();});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_FocusNear,    [this]{_controller->focus_near();});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_FocusFar,     [this]{_controller->focus_far();});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_FocusStop,    [this]{_controller->focus_stop();});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_FocusAuto,    [this]{_controller->autofocus();});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_LrfOn,    [this]{_controller->lrf_set_enabled(true);});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_LrfOff,   [this]{_controller->lrf_set_enabled(false);});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_LrfSingle,    [this]{_controller->lrf_start_mono();});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_LrfStart,     [this]{_controller->lrf_start_5Hz();});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_LrfStop,  [this]{_controller->lrf_stop();});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_ChannelToggle,    [this]{switch_channel();});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_ChannelTv,    [this]{select_channel(CHANNEL_ID_VLX_STREAM_1);});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_ChannelIr,    [this]{select_channel(CHANNEL_ID_VLX_STREAM_2);});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_GspToggleStabilization,   [this]{_controller->gsp_stabilization_switch();});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_GspStabilizationOn,   [this]{_controller->gsp_stabilization_on();});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_GspStabilizationOff,  [this]{_controller->gsp_stabilization_off();});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_GspPark,  [this]{_controller->gsp_park();});
    m_vlxJoystick->bindAction(Vlx::Input::Cmd::Action_GspGotoZero,  [this]{_controller->gsp_null_position();});


    // --- Start ------


    qDebug() << "[VLX JOYSTICK]: START";
    m_vlxJoystick->setPollInterval(VlxSettings::i().joystickPollIntervalMs());
    m_vlxJoystick->openJoystick(0);
    m_vlxJoystick->start();



//     _joystick = new Joystick(this);
//     _joystick->setPollingIntervalMs(VlxSettings::i().joystickPollIntervalMs());
//     _joystick->set_enable(true); //NOTE: true to use joystick, false to use panel
//     if (_dev_type == DEV_TYPES::en_dev_type_350 || _dev_type == DEV_TYPES::en_dev_type_obzor) {
//         connect(_joystick, SIGNAL(agc_switch()), _controller, SLOT(agc_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//         connect(_joystick, SIGNAL(swir_gain_switch()), _controller, SLOT(swir_gain_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     }
//     connect(_joystick, SIGNAL(zoom_in()), _controller, SLOT(zoom_in()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, SIGNAL(zoom_out()), _controller, SLOT(zoom_out()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, SIGNAL(zoom_stop()), _controller, SLOT(zoom_stop()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, SIGNAL(zoom_pos_fix()), _controller, SLOT(zoom_pos_fix()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, SIGNAL(zoom_pos_fix_width()), _controller, SLOT(zoom_pos_fix_width()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, SIGNAL(focus_near()), _controller, SLOT(focus_near()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, SIGNAL(focus_far()), _controller, SLOT(focus_far()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, SIGNAL(focus_stop()), _controller, SLOT(focus_stop()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(iris_close()), _controller, SLOT(iris_close()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(iris_open()), _controller, SLOT(iris_open()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(iris_stop()), _controller, SLOT(iris_stop()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(field_sync_narrow()), _controller, SLOT(field_sync_narrow()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(field_sync()), _controller, SLOT(field_sync()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(field_sync_reg_switch()), _controller, SLOT(field_sync_reg_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, SIGNAL(autofocus()), _controller, SLOT(autofocus()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

//     connect(_joystick, SIGNAL(gsp_speed_set(const double &, const double &)), _controller, SLOT(gsp_speed_set(const double &, const double &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, SIGNAL(gspFixAngleSwitch()), this, SLOT(on_fixAngle_clicked()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(gsp_slow_speed_set(const double &, const double &)), _controller, SLOT(gsp_slow_speed_set(const double &, const double &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(gsp_absolute_angle_set(const double &, const double &)), _controller, SLOT(gsp_absolute_angle_set(const double &, const double &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(gsp_relative_angle_set(const double &, const double &)), _controller, SLOT(gsp_relative_angle_set(const double &, const double &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(gsp_arretir_reset()), _controller, SLOT(gsp_arretir_reset()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_joystick, SIGNAL(gsp_stabilization_switch()), _controller, SLOT(gsp_stabilization_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_joystick, SIGNAL(gsp_park()), _controller, SLOT(gsp_park()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(gsp_arretir_set()), _controller, SLOT(gsp_arretir_set()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_joystick, SIGNAL(gsp_null_position()), _controller, SLOT(gsp_null_position()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

// //    connect(_joystick, SIGNAL(digital_zoom_in()), _controller, SLOT(digital_zoom_in()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(digital_zoom_out()), _controller, SLOT(digital_zoom_out()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(pic_in_pic()), _controller, SLOT(pic_in_pic()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, SIGNAL(inversion()), _controller, SLOT(inversion_mw()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(brightness_up()), _controller, SLOT(brightness_up()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(brightness_down()), _controller, SLOT(brightness_down()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(contrast_up()), _controller, SLOT(contrast_up()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(_joystick, SIGNAL(contrast_down()), _controller, SLOT(contrast_down()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

//     connect(_joystick, &Joystick::lrf_enable_switch, _controller, &Controller::lrf_enable_switch, Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, &Joystick::lrf_toggle_continuous, _controller, &Controller::lrf_toggle_continuos, Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, &Joystick::lrf_single, _controller, &Controller::lrf_start_mono, Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

// //    connect(joystick, SIGNAL(lrf_mode_switch()), controller, SLOT(lrf_mode_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     // connect(_joystick, SIGNAL(lrf_start()), _controller, SLOT(lrf_start()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

//     connect(_joystick, SIGNAL(target_enable_switch()), _controller, SLOT(target_enable_switch_single()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, &Joystick::target_set_enabled, _controller, &Controller::target_set_multi_enabled, Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, SIGNAL(target_capture()), _controller, SLOT(target_capture_multi()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, SIGNAL(target_free()), _controller, SLOT(target_free_single()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, SIGNAL(target_window_set(const double &)), _controller, SLOT(target_window_set(const double &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
// //    connect(joystick, SIGNAL(target_id_switch()), controller, SLOT(target_id_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, SIGNAL(target_choose()), _controller, SLOT(target_choose()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//     connect(_joystick, SIGNAL(target_fast_switch()), _controller, SLOT(target_fast_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

//     connect(_joystick, SIGNAL(videoRecordSwitch()), _controller, SLOT(video_record_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(_joystick, SIGNAL(next_channel()), this, SLOT(next_channel()), Qt::DirectConnection);
//    connect(_joystick, SIGNAL(prev_channel()), this, SLOT(prev_channel()), Qt::DirectConnection);
//    connect(_joystick, &Joystick::switch_channel, this, &MainWindow::switch_channel, Qt::DirectConnection);
// //    connect(_joystick, SIGNAL(change_channel(const uint32_t &)), this, SLOT(select_channel(const uint32_t &)), Qt::DirectConnection);
}
void MainWindow::init_panel()
{
    _panel = new Panel(this);
    _panel->set_port_name(_panel_com_port_name);
    _panel->set_auto_connect(true);
    _panel->set_enable(false); //NOTE: true to use panel, false to use joystick

    connect(_panel, SIGNAL(select_channel(const uint32_t &)), _controller, SLOT(select_channel(const uint32_t &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(digital_zoom_switch()), _controller, SLOT(digital_zoom_switch()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(inversion()), _controller, SLOT(inversion_mw()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(brightness_up()), _controller, SLOT(brightness_up()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(brightness_down()), _controller, SLOT(brightness_down()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(contrast_up()), _controller, SLOT(contrast_up()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(contrast_down()), _controller, SLOT(contrast_down()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(bc_reset()), _controller, SLOT(bc_reset()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(target_switch()), _controller, SLOT(target_switch_multi()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(target_window_set(const int &)), _controller, SLOT(target_window_set(const int &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
//    connect(panel, SIGNAL(lrf_start()), controller, SLOT(lrf_start()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(gsp_speed_set(const double &, const double &)), _controller, SLOT(gsp_speed_set(const double &, const double &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(zoom_in(const double &)), _controller, SLOT(zoom_in(const double &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(zoom_out(const double &)), _controller, SLOT(zoom_out(const double &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(zoom_stop()), _controller, SLOT(zoom_stop()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(focus_near(const double &)), _controller, SLOT(focus_near(const double &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(focus_far(const double &)), _controller, SLOT(focus_far(const double &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(focus_stop()), _controller, SLOT(focus_stop()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(autofocus_en()),  _tvCtrlModel,   SLOT(set_autofocus_en_ui()),  Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(autofocus_dis()), _tvCtrlModel,   SLOT(set_autofocus_dis_ui()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    if (_dev_type == DEV_TYPES::en_dev_type_350 || _dev_type == DEV_TYPES::en_dev_type_obzor) {
        connect(_panel, SIGNAL(autofocus_en()),  _swirCtrlModel, SLOT(set_autofocus_en_ui()),  Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
        connect(_panel, SIGNAL(autofocus_dis()), _swirCtrlModel, SLOT(set_autofocus_dis_ui()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    }
    connect(_panel, SIGNAL(autofocus_en()),  _mwirCtrlModel, SLOT(set_autofocus_en_ui()),  Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(_panel, SIGNAL(autofocus_dis()), _mwirCtrlModel, SLOT(set_autofocus_dis_ui()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
}
void MainWindow::init_logger()
{
    // qInstallMessageHandler(messageHandler);
}

void MainWindow::init_gspInitializationModul()
{
    m_gspInit = new GSP_Initialization();
    connect(m_gspInit, SIGNAL(sg_turnOffMotors()),              this,        SLOT(on_motor_off_clicked()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(m_gspInit, SIGNAL(sg_blockAllGSPInterface(bool)),   this,        SLOT(blockAllInterface(bool)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(m_gspInit, SIGNAL(sg_turnOffMechArretires()),       this,        SLOT(on_fine_arretir_reset_clicked()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(m_gspInit, SIGNAL(sg_turnOnFineCircuit()),          this,        SLOT(on_el_arr_fine_clicked()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(m_gspInit, SIGNAL(sg_turnOnCoarseCircuit()),        this,        SLOT(on_el_arr_coarse_clicked()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(m_gspInit, SIGNAL(sg_setZero()),                    this,        SLOT(on_arretir_reset_clicked()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(m_gspInit, SIGNAL(sg_turnOnStabilization()),        _controller, SLOT(gsp_stabilization_on()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

}

void MainWindow::serviceCmdTurnOn()
{
//    _ui->gboxService->setVisible(true);
//    _ui->label->setVisible(true);
//    _ui->ledit_rls_angle->setVisible(true);
//    _ui->setAngle->setVisible(true);
//    _ui->fine_arretir_set->setVisible(true);
//    _ui->fine_arretir_reset->setVisible(true);
    _ui->left->setVisible(true);

    _serviceCmdsOn = true;
    _ui->btn_confirmPassword->setText("Выкл");
    _ui->service_enterPassword->setText("");
}
void MainWindow::serviceCmdTurnOff()
{
//    _ui->gboxService->setVisible(false);
//    _ui->label->setVisible(false);
//    _ui->ledit_rls_angle->setVisible(false);
//    _ui->setAngle->setVisible(false);
//    _ui->fine_arretir_set->setVisible(false);
//    _ui->fine_arretir_reset->setVisible(false);
    _ui->left->setVisible(false);

    _serviceCmdsOn = false;
    _ui->btn_confirmPassword->setText("Вкл");
    _ui->service_enterPassword->setText("");
}
void MainWindow::load()
{
    Resource resource(RESOURCE_SECTION);
    if(resource.load(RESOURCE_FILENAME))
    {
        bool ok;
        QVariant tmp;
//        tmp = resource.get("HOST_ETLD", ok);
//        if(ok) _ui->host_etld->setChecked(tmp.toBool());
        tmp = resource.get("INVERTED_POSITION", ok);
        if(ok) _ui->inverted_position->setChecked(tmp.toBool());
        tmp = resource.get("WINDOW_TITLE", ok);
        if(ok) _window_title = tmp.toString();
        tmp = resource.get("PANEL_COM_PORT_NAME", ok);
        if(ok) _panel_com_port_name = tmp.toString();
        tmp = resource.get("MEDIA_PATH", ok);
        if(ok) _media_path = tmp.toString();
    }
    else
    {
        save();
    }
}
void MainWindow::save()
{
    Resource resource(RESOURCE_SECTION);
//    resource.set("HOST_ETLD",               _ui->host_etld->isChecked());
    resource.set("INVERTED_POSITION",       _ui->inverted_position->isChecked());
    resource.set("WINDOW_TITLE",            _window_title);
    resource.set("PANEL_COM_PORT_NAME",     _panel_com_port_name);
    resource.set("MEDIA_PATH",              _media_path);
    resource.save(RESOURCE_FILENAME);
}
void MainWindow::closeEvent(QCloseEvent *)
{
    _interfaceView->hide();
    _telemetryView->hide();
    _tvCtrlView->hide();
    if (_dev_type == DEV_TYPES::en_dev_type_350 || _dev_type == DEV_TYPES::en_dev_type_obzor) _swirCtrlView->hide();
    _mwirCtrlView->hide();
    _highlighterView->hide();
    _gspCtrlView->hide();
    _fusionCtrlView->hide();
    _terminalSettingsView->close();
    _vdbMsgSenderView->close();
    QApplication::instance()->quit();

}
void MainWindow::on_telemetry_triggered()
{
    if(Accessor::check(this))
    {
        _telemetryView->show();
        _telemetryView->raise();
    }
}
void MainWindow::settings()
{
    uint32_t id = _controller->channelId();
    switch(id)
    {
    case CHANNEL_ID_TVRGB:
        on_tvCtrl_triggered();
        break;
    case CHANNEL_ID_FUSION:
        break;
    case CHANNEL_ID_SWIR:
            if (_dev_type == DEV_TYPES::en_dev_type_350  || _dev_type == DEV_TYPES::en_dev_type_obzor) on_swirCtrl_triggered();
        break;
    case CHANNEL_ID_MWIR:
        on_mwirCtrl_triggered();
        break;
    case CHANNEL_ID_TVGRAY:
        on_tvCtrl_triggered();
        break;
    case CHANNEL_ID_TVGRAYMWIR:
        on_fusionCtrl_triggered();
        break;
    case CHANNEL_ID_TEST:
        break;
    }
}
void MainWindow::on_tvCtrl_triggered()
{
    if(Accessor::check(this))
    {
        _tvCtrlView->show();
        _tvCtrlView->raise();
    }
}
void MainWindow::on_swirCtrl_triggered()
{
    if(Accessor::check(this) && (_dev_type == DEV_TYPES::en_dev_type_350  || _dev_type == DEV_TYPES::en_dev_type_obzor))
    {
        _swirCtrlView->show();
        _swirCtrlView->raise();
    }
}
void MainWindow::on_mwirCtrl_triggered()
{
    if(Accessor::check(this))
    {
        _mwirCtrlView->show();
        _mwirCtrlView->raise();
    }
}
void MainWindow::on_highlighter_triggered()
{
    if(Accessor::check(this))
    {
        _highlighterView->show();
        _highlighterView->raise();
    }
}
void MainWindow::on_gspCtrl_triggered()
{
    if(Accessor::check(this))
    {
        _gspCtrlView->show();
        _gspCtrlView->raise();
    }
}
void MainWindow::on_fusionCtrl_triggered()
{
    if(Accessor::check(this))
    {
        _fusionCtrlView->show();
        _fusionCtrlView->raise();
    }
}
void MainWindow::on_vdbSendMsg_triggered()
{
    if(Accessor::check(this))
    {
        _vdbMsgSenderView->show();
        _vdbMsgSenderView->raise();
    }
}
void MainWindow::on_help_triggered()
{
    Help::help(this);
}
void MainWindow::on_about_triggered()
{
    About::about(this);
}
void MainWindow::on_connection_triggered()
{
    if(Accessor::check(this))
    {
        _interfaceView->show();
        _interfaceView->raise();
    }
}
void MainWindow::on_terminal_settings_triggered()
{
    if(Accessor::check(this))
    {
        _terminalSettingsView->show();
        _terminalSettingsView->raise();
    }
}
void MainWindow::on_channel_tvrgb_clicked()
{
    // _controller->select_channel(CHANNEL_ID_TVRGB);
    qDebug() << "*** select: CHANNEL_ID_VLX_STREAM_1 (RGB)";
    _controller->select_channel(CHANNEL_ID_VLX_STREAM_1);
}

void MainWindow::on_channel_rtp_clicked()
{
    _controller->select_channel(CHANNEL_ID_VLX_STREAM_1);
}

void MainWindow::on_channel_rtp_2_clicked()
{
    _controller->select_channel(CHANNEL_ID_VLX_STREAM_2);
}

void MainWindow::on_channel_tvgray_clicked()
{
    _controller->select_channel(CHANNEL_ID_TVGRAY);
}
void MainWindow::on_channel_swir_clicked()
{
    if (_dev_type == DEV_TYPES::en_dev_type_350  || _dev_type == DEV_TYPES::en_dev_type_obzor) _controller->select_channel(CHANNEL_ID_SWIR);
}
void MainWindow::on_channel_mwir_clicked()
{
    // _controller->select_channel(CHANNEL_ID_MWIR);
    qDebug() << "*** select: CHANNEL_ID_VLX_STREAM_2 (MWIR)";
    _controller->select_channel(CHANNEL_ID_VLX_STREAM_2);
}
void MainWindow::on_channel_fusion_clicked()
{
    _controller->select_channel(CHANNEL_ID_FUSION);
}
void MainWindow::on_channel_tvgraymwir_clicked()
{
    _controller->select_channel(CHANNEL_ID_TVGRAYMWIR);
}
void MainWindow::on_channel_test_clicked()
{
    _controller->select_channel(CHANNEL_ID_TEST);
}
void MainWindow::on_channel_cpu_clicked()
{
    _controller->select_channel(CHANNEL_ID_CPU);
}
void MainWindow::on_arretir_set_clicked()
{
    _controller->gsp_arretir_set();
}
void MainWindow::on_arretir_reset_clicked()
{
    _controller->gsp_arretir_reset();
}
void MainWindow::on_el_arr_coarse_clicked()
{
    _controller->gsp_el_arr_coarse();
}
void MainWindow::on_el_arr_fine_clicked()
{
    _controller->gsp_el_arr_fine();
}
void MainWindow::on_park_clicked()
{
    _controller->gsp_park();
}
void MainWindow::on_stabilization_toggled(bool checked)
{
    if(checked) _controller->gsp_stabilization_on();
    else _controller->gsp_stabilization_off();
}
void MainWindow::on_motor_off_clicked()
{
    _controller->vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_motor, 0);
}
void MainWindow::on_gsp_self_test_clicked()
{
    _controller->vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_self_test, 1);
}
void MainWindow::image_ready()
{
    QTransform disp_to_frm_tr;
    QSize frm_size;
    QImage *img = _controller->image(frm_size, disp_to_frm_tr);
    auto ch = _controller->channel();

    if(img) {
        _ui->video_view->new_image(*img, frm_size, disp_to_frm_tr);
        static auto t_start = std::chrono::steady_clock::now();
        static int cnt = 0;
        if(++cnt > 24)
        {
            _fps = 1000.0 * double(cnt) / double((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t_start)).count());
            t_start = std::chrono::steady_clock::now();
            cnt = 0;
        }
        _status_mouse_pos->setText(QString(" (%1, %2) ").arg(_mouse_x, 4).arg(_mouse_y, 4));
        _status_fps->setText(QString(" %1 ").arg(_fps, 5, 'g', 5));
        _status_connection->setText(QString(" Соединение: %1 ").arg(EthController::inst().is_started() ? "Да" : "Нет", 5));

//        _status_panel->setText(QString(" Пульт: %1 ").arg(_panel->is_opened() ? "Да" : "Нет", 5));
    }
}
void MainWindow::mouse_pos(const int & x, const int & y)
{
    _mouse_x = x;
    _mouse_y = y;
}
void MainWindow::on_inverted_position_clicked(bool checked)
{
    if (_ui->chb_allChannels->isChecked()) {
        _controller->host_x_inv_set(checked);
        _controller->host_y_inv_set(checked);
    } else {
        if (auto ch = _controller->channel()) {
            ch->set_x_inv(checked);
            ch->set_y_inv(checked);
        }
    }
    indicateInvertedPos();
}
void MainWindow::on_host_etld_clicked(bool checked)
{
    _controller->host_etld_en_set(checked);
}
void MainWindow::on_self_test_toggled(bool checked)
{
    if(checked) _controller->self_test_on();
    else _controller->self_test_off();
}
void MainWindow::full_screen()
{
    if(isFullScreen())
    {
        setWindowState(Qt::WindowMaximized);
        _ui->menuBar->show();
        _ui->statusBar->show();
        _ui->top->show();
//            _ui->left->show();
        _ui->right->show();
        _ui->bot->show();
        _ui->main_layout->setMargin(2);
        _ui->main_layout->setSpacing(2);
        _ui->video_view->full_screen(false);
        qInfo(logMainWindow(), "full_screen: Off");
    }
    else
    {
        setWindowState(Qt::WindowFullScreen);
        _ui->menuBar->hide();
        _ui->statusBar->hide();
        _ui->top->hide();
        _ui->left->hide();
        serviceCmdTurnOff();
        _ui->right->hide();
        _ui->bot->hide();
        _ui->main_layout->setMargin(0);
        _ui->main_layout->setSpacing(0);
        _ui->video_view->full_screen(true);
        qInfo(logMainWindow(), "full_screen: On");
    }
}
void MainWindow::next_channel()
{
    uint32_t channel_id = _controller->channelId() <= CHANNEL_ID_TEST ? _controller->channelId() : CHANNEL_ID_FUSION;
    switch(channel_id)
    {
    case CHANNEL_ID_TVRGB:
        channel_id = CHANNEL_ID_TVGRAY;
        break;
    case CHANNEL_ID_TVGRAY:
        if (_dev_type == DEV_TYPES::en_dev_type_350  || _dev_type == DEV_TYPES::en_dev_type_obzor) {
            channel_id = CHANNEL_ID_SWIR;
        } else if (_dev_type == DEV_TYPES::en_dev_type_350p) {
            channel_id = CHANNEL_ID_MWIR;
        }
        break;
    case CHANNEL_ID_SWIR:
        channel_id = CHANNEL_ID_MWIR;
        break;
    case CHANNEL_ID_MWIR:
        channel_id = CHANNEL_ID_TVRGB;
        break;
//    case CHANNEL_ID_MWIR:
//        channel_id = CHANNEL_ID_FUSION;
//        break;
//    case CHANNEL_ID_FUSION:
//        channel_id = CHANNEL_ID_TVRGB;
//        break;
    default:
        channel_id = CHANNEL_ID_TVRGB;
        break;
    }
    select_channel(channel_id);
}
void MainWindow::prev_channel()
{
    uint32_t channel_id = _controller->channelId() <= CHANNEL_ID_TEST ? _controller->channelId() : CHANNEL_ID_TVGRAY;
    switch(channel_id)
    {
    case CHANNEL_ID_TVRGB:
//        channel_id = CHANNEL_ID_FUSION;
        channel_id = CHANNEL_ID_MWIR;
        break;
    case CHANNEL_ID_TVGRAY:
        channel_id = CHANNEL_ID_TVRGB;
        break;
    case CHANNEL_ID_SWIR:
        channel_id = CHANNEL_ID_TVGRAY;
        break;
    case CHANNEL_ID_MWIR:
        if (_dev_type == DEV_TYPES::en_dev_type_350  || _dev_type == DEV_TYPES::en_dev_type_obzor) {
            channel_id = CHANNEL_ID_SWIR;
        } else if (_dev_type == DEV_TYPES::en_dev_type_350p) {
            channel_id = CHANNEL_ID_TVGRAY;
        }
        break;
//    case CHANNEL_ID_FUSION:
//        channel_id = CHANNEL_ID_MWIR;
//        break;
    default:
        channel_id = CHANNEL_ID_TVRGB;
        break;
    }
    select_channel(channel_id);
}

void MainWindow::switch_channel()
{
    uint32_t channel_id = _controller->channelId();
    auto new_chan = (channel_id == CHANNEL_ID_VLX_STREAM_1) ? CHANNEL_ID_VLX_STREAM_2 : CHANNEL_ID_VLX_STREAM_1;
    select_channel(new_chan);
}

void MainWindow::select_channel(const uint32_t & id)
{
    switch(id)
    {
    case CHANNEL_ID_TVRGB:
        on_channel_tvrgb_clicked();
        break;
    case CHANNEL_ID_TVGRAY:
        on_channel_tvgray_clicked();
        break;
    case CHANNEL_ID_SWIR:
        if (_dev_type == DEV_TYPES::en_dev_type_350 || _dev_type == DEV_TYPES::en_dev_type_obzor) on_channel_swir_clicked();
        break;
    case CHANNEL_ID_MWIR:
        on_channel_mwir_clicked();
        break;
//    case CHANNEL_ID_FUSION:
//        on_channel_fusion_clicked();
//        break;
    case CHANNEL_ID_TVGRAYMWIR:
        on_channel_tvgraymwir_clicked();
        break;
    case CHANNEL_ID_TEST:
        on_channel_test_clicked();
        break;
    case CHANNEL_ID_VLX_STREAM_1:
        _controller->select_channel(CHANNEL_ID_VLX_STREAM_1);
        break;
    case CHANNEL_ID_VLX_STREAM_2:
        _controller->select_channel(CHANNEL_ID_VLX_STREAM_2);
        break;
    default:
        break;
    }
}
MainWindow::~MainWindow()
{
    save();
    EthController::inst().stop();
    delete _interfaceView;
    delete _ui;
}

void MainWindow::response(Telemetry t)
{
    Telemetry::t_telemetry_struct * tel = t.get();
    static bool previousGspState = false;
    bool stabilizationOn = (tel->gsp.mode.gyrostabilization == 0) ? false : true;
    if(previousGspState != stabilizationOn) {
        if(stabilizationOn) {
            _ui->stabilization->setStyleSheet("color: green;");
        } else {
            _ui->stabilization->setStyleSheet("color: white;");
        }
        previousGspState = stabilizationOn;
    }
    sl_alignJoystikWidget();


    Telemetry::t_gsp_raw_status* raw_status = reinterpret_cast<Telemetry::t_gsp_raw_status*>(&tel->dev.gsp_raw_status);
    bool mechArrEn = (tel->gsp.mode.mechanical_locking == 0) ? false : true;
    bool elArrFine = (raw_status->el_arr_x == 0) ? false : true;
    bool elArrCoarse = (raw_status->el_arr_z == 0) ? false : true;
    bool zeroEn = (tel->gsp.mode.zero == 0) ? false : true;
    bool stabEn = (tel->gsp.mode.gyrostabilization == 0) ? false : true;
    m_gspInitStatus = m_gspInit->runtime(mechArrEn, elArrFine, elArrCoarse, zeroEn, stabEn);
    _ui->lbl_initGspStatus->setText(m_gspInitStatus);

    if(m_fixAngle.enable)
    {
        if(m_fixAngle.rememberAngles) {
            m_fixAngle.angleZ = tel->gsp.z_angle;
            m_fixAngle.angleX = tel->gsp.x_angle;
            m_fixAngle.rememberAngles = false;
        }
        m_fixAngle.angleCtrlOn = (tel->gsp.mode.angle_control == 0) ? false : true;
    }
}

void MainWindow::on_fine_arretir_set_clicked()
{
    _controller->vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_arretir, 1);
}

void MainWindow::on_fine_arretir_reset_clicked()
{
    _controller->vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_arretir, 0);
}

void MainWindow::on_setAngle_released()
{
  bool result{false};
  double ledit_rls_angle = _ui->ledit_rls_angle->text().toDouble(&result);
  if(result) {
    _controller->gsp_set_z_delta_shift(ledit_rls_angle);
  }
}

void MainWindow::on_el_arr_clicked()
{
    _controller->vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_el_arr, (0 << 8) | 1);
}


void MainWindow::on_btn_confirmPassword_clicked()
{
    QString userPassword = _ui->service_enterPassword->text();
    if(userPassword == _serviceCmdPassword) {
        serviceCmdTurnOn();
    } else {
        serviceCmdTurnOff();
    }
}

void MainWindow::blockAllInterface(bool block)
{
    if(block) {
//        _controller->sl_blockGSPControl(true);

        disconnect(_keyfilter, SIGNAL(gsp_speed_set(const double &, const double &)), _controller, SLOT(gsp_speed_set(const double &, const double &)));
        disconnect(_keyfilter, SIGNAL(gsp_park()), _controller, SLOT(gsp_park()));
        disconnect(_keyfilter, SIGNAL(gsp_null_position()), _controller, SLOT(gsp_null_position()));

        disconnect(_ui->newJoystik, &JoystickWidget::sg_newExcursion, _controller, &Controller::gsp_speed_set);



    } else {
//        _controller->sl_blockGSPControl(false);

        connect(_keyfilter, SIGNAL(gsp_speed_set(const double &, const double &)), _controller, SLOT(gsp_speed_set(const double &, const double &)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
        connect(_keyfilter, SIGNAL(gsp_park()), _controller, SLOT(gsp_park()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
        connect(_keyfilter, SIGNAL(gsp_null_position()), _controller, SLOT(gsp_null_position()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

        connect(_ui->newJoystik, &JoystickWidget::sg_newExcursion, _controller, &Controller::gsp_speed_set, Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    }
    _ui->arretir_reset->setDisabled(block);
    _ui->stabilization->setDisabled(block);
    _ui->park->setDisabled(block);
    _ui->fine_arretir_set->setDisabled(block);
    _ui->fine_arretir_reset->setDisabled(block);
    _ui->el_arr_fine->setDisabled(block);
    _ui->el_arr_coarse->setDisabled(block);

}

void MainWindow::turnOnImitateJoystick()
{
    static bool imitateJoystickOn = false;
    imitateJoystickOn = !imitateJoystickOn;
    if(imitateJoystickOn == true) {
        _ui->newJoystik->setHidden(false);
    } else {
        _ui->newJoystik->setHidden(true);
    }
}

void MainWindow::onVlxIdmxStateChanged(Vlx::Av::InputDemuxer::State state)
{
    switch (state) {
    case Vlx::Av::InputDemuxer::State::NotInitialized:
        _status_vlx_idmx->setText("RTP: выкл.");
        return;
    case Vlx::Av::InputDemuxer::State::Initializing:
        _status_vlx_idmx->setText("RTP: ожидание");
        return;
    case Vlx::Av::InputDemuxer::State::Receiving:
        _status_vlx_idmx->setText("RTP: OK");
        return;
    case Vlx::Av::InputDemuxer::State::Deinitiliazing:
        _status_vlx_idmx->setText("RTP: закрытие");
        return;
    }
}

void MainWindow::onChannelChanged(uint32_t chanId)
{
    Q_UNUSED(chanId);
    indicateInvertedPos();
}


void MainWindow::sl_blockInterfaceForArretiresOff()
{
    blockAllInterface(false);
}

void MainWindow::sl_alignJoystikWidget()
{
    static QSize lastSize;
    QSize size = _ui->video_view->size();
    if(lastSize != size) {
        _ui->newJoystik->move(0, size.height() - 350);
        lastSize = size;
    }
}



void MainWindow::on_btn_prepareToWork_clicked()
{
    m_gspInit->turnON();
    blockAllInterface(true);
}


void MainWindow::on_angleZ_Max_180_clicked()
{
    _ui->angleZ_Max_360->setChecked(false);
    _ui->angleZ_Max_180->setChecked(true);
    _controller->sl_setZAngleRange180(true);

}
void MainWindow::on_angleZ_Max_360_clicked()
{
    _ui->angleZ_Max_180->setChecked(false);
    _ui->angleZ_Max_360->setChecked(true);
    _controller->sl_setZAngleRange180(false);
}


void MainWindow::on_btn_mwirDampEn_clicked()
{
    static bool mwirDampEn = false;
    mwirDampEn = !mwirDampEn;
    if(mwirDampEn == true) {
        _ui->btn_mwirDampEn->setText("ВЫКЛ СЫРОЙ ПОТОК ТПВ");
    } else {
        _ui->btn_mwirDampEn->setText("ВКЛ СЫРОЙ ПОТОК ТПВ");
    }
    _controller->sl_mwirDumpEnable(mwirDampEn);
}

void MainWindow::on_fixAngle_clicked()
{
    if(m_fixAngle.enable) {
        fixAngleTurnOFF();
    } else {
        fixAngleTurnON();
    }
}
void MainWindow::fixAngleTurnON()
{
    m_fixAngle.enable = true;
    m_fixAngle.rememberAngles = true;
    _controller->gsp_angle_ctrl_mode();
    QTimer::singleShot(m_fixAngle.tmrPeriodAnglCtrlON, this, &MainWindow::sl_fixAngleONTimer);
}
void MainWindow::fixAngleTurnOFF()
{
    _controller->gsp_velocity_ctrl_mode();
    QTimer::singleShot(m_fixAngle.tmrPeriodAnglCtrlOFF, this, &MainWindow::sl_fixAngleOFFTimer);
}

void MainWindow::indicateInvertedPos()
{
    bool on = false;
    if (auto ch = _controller->channel())
        on = (ch->x_inv() || ch->x_inv());
    _ui->inverted_position->setStyleSheet(on ? "color: green;" : "color: white;");
}
void MainWindow::sl_fixAngleONTimer()
{
    static uint32_t cntr = 0;

    if(m_fixAngle.angleCtrlOn == true) {
        cntr = 0;
        _controller->gsp_absolute_angle_set(m_fixAngle.angleX, m_fixAngle.angleZ);
        _ui->fixAngle->setStyleSheet("color: green;");
    } else {
        if(cntr < 3) {
            cntr++;
            _controller->gsp_angle_ctrl_mode();
            QTimer::singleShot(m_fixAngle.tmrPeriodAnglCtrlON, this, &MainWindow::sl_fixAngleONTimer);
        } else {
            cntr = 0;
            fixAngleTurnOFF();
        }
    }
}
void MainWindow::sl_fixAngleOFFTimer()
{
    static uint32_t cntr = 0;

    if(m_fixAngle.angleCtrlOn == false) {
        cntr = 0;
        _ui->fixAngle->setStyleSheet("color: white;");
        m_fixAngle.enable = false;
        m_fixAngle.angleX = 0.0;
        m_fixAngle.angleZ = 0.0;
        m_fixAngle.rememberAngles = false;
    } else {
        if(cntr < 3) {
            cntr++;
            _controller->gsp_velocity_ctrl_mode();
            QTimer::singleShot(m_fixAngle.tmrPeriodAnglCtrlOFF, this, &MainWindow::sl_fixAngleOFFTimer);
        } else {
            cntr = 0;
            m_fixAngle.enable = false;
            m_fixAngle.angleX = 0.0;
            m_fixAngle.angleZ = 0.0;
            m_fixAngle.rememberAngles = false;
            _ui->fixAngle->setStyleSheet("color: white;");
        }
    }
}


void MainWindow::on_btn_turnOnTVMWIRChannel_clicked()
{
    select_channel(CHANNEL_ID_TVGRAYMWIR);
}
void MainWindow::on_btn_turnOnFusionChannel_clicked()
{
    select_channel(CHANNEL_ID_FUSION);
}

