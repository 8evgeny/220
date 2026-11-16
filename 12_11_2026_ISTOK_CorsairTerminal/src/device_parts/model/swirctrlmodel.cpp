#include "swirctrlmodel.h"

#include <QVariant>
#include <qmv/property/booleanvalueproperty.h>
#include <qmv/property/doublevalueproperty.h>
#include <qmv/property/intvalueproperty.h>
#include <qmv/property/cmdproperty.h>

#include "device_parts/channel/swirchannel.h"

#include "tools/autofocusmodel.h"

#include "tools/logger.h"
Q_LOGGING_CATEGORY(logSWIRCtrl, "SWIRCtrl")

#ifdef CORSAIR_SET_1
#define AF_MODEL_CONFIG_NAME ("table/afmodel_swir_1.txt")
#endif
#ifdef CORSAIR_SET_2
#define AF_MODEL_CONFIG_NAME ("table/afmodel_swir_2.txt")
#endif
#ifdef CORSAIR_SET_3
#define AF_MODEL_CONFIG_NAME ("table/afmodel_swir_3.txt")
#endif

SWIRCtrlModel::SWIRCtrlModel(QObject * parent, uint32_t id, QString name) :
    QMVModel(parent, id, name),
    sensor_status(new IntValueProperty(this, id_sensor_status, "sensor_status", 0, -65536, 65536)),
    lens_status(new IntValueProperty(this, id_lens_status, "lens_status", 0, -65536, 65536)),
    fps(new IntValueProperty(this, id_fps, "fps", 0, -65536, 65536)),
    vres(new IntValueProperty(this, id_vres, "vres", 0, -65536, 65536)),
    hres(new IntValueProperty(this, id_hres, "hres", 0, -65536, 65536)),
    exp(new IntValueProperty(this, id_exp, "exp", 0, -65536, 65536)),
    zoom_drive(new IntValueProperty(this, id_zoom_drive, "zoom_drive", 0, -65536, 65536)),
    focus_drive(new IntValueProperty(this, id_focus_drive, "focus_drive", 0, -65536, 65536)),
    lens_temp(new IntValueProperty(this, id_lens_temp, "lens_temp", 0, -65536, 65536)),
    focus_mm(new DoubleValueProperty(this, id_focus_mm, "focus_mm", 0, -65536, 65536)),
    field_grad(new DoubleValueProperty(this, id_field_grad, "field_grad", 0, -65536, 65536)),
    zoom(new IntValueProperty(this, id_zoom, "zoom", 0, 0, 65536)),
    focus(new IntValueProperty(this, id_focus, "focus", 0, 0, 65536)),
    _autofocus_lens_en(new BooleanValueProperty(this, id_autofocus_lens_en, "autofocus_lens_en", true)),
    _autofocus_host_en(new BooleanValueProperty(this, id_autofocus_host_en, "autofocus_host_en", false)),
    _thermostat_on(new CmdProperty(this, id_thermostat_on, "thermostat_on")),
    _thermostat_off(new CmdProperty(this, id_thermostat_off, "thermostat_off")),
    _heat_on(new CmdProperty(this, id_heat_on, "heat_on")),
    _heat_off(new CmdProperty(this, id_heat_off, "heat_off")),
    agc(new CmdProperty(this, id_agc, "agc")),
    _autofocus(new CmdProperty(this, id_autofocus, "autofocus")),
    _field_sync(new CmdProperty(this, id_field_sync, "field_sync")),
    _field_sync_narrow(new CmdProperty(this, id_field_sync_narrow, "field_sync_narrow")),
    field_sync_reg(new CmdProperty(this, id_field_sync_reg, "field_sync_reg")),
    zoom_in_start(new CmdProperty(this, id_zoom_in_start, "zoom_in_start")),
    zoom_in_stop(new CmdProperty(this, id_zoom_in_stop, "zoom_in_stop")),
    zoom_out_start(new CmdProperty(this, id_zoom_out_start, "zoom_out_start")),
    zoom_out_stop(new CmdProperty(this, id_zoom_out_stop, "zoom_out_stop")),
    iris_in_start(new CmdProperty(this, id_iris_in_start, "iris_in_start")),
    iris_in_stop(new CmdProperty(this, id_iris_in_stop, "iris_in_stop")),
    iris_out_start(new CmdProperty(this, id_iris_out_start, "iris_out_start")),
    iris_out_stop(new CmdProperty(this, id_iris_out_stop, "iris_out_stop")),
    focus_near_start(new CmdProperty(this, id_focus_near_start, "focus_near_start")),
    focus_near_stop(new CmdProperty(this, id_focus_near_stop, "focus_near_stop")),
    focus_far_start(new CmdProperty(this, id_focus_far_start, "focus_far_start")),
    focus_far_stop(new CmdProperty(this, id_focus_far_stop, "focus_far_stop")),
    agc_on(false),
    autofocus_on(false),
    _autofocus_host_pause(false),
    _autofocus_host_last_state(false),
    field_sync_reg_on(false),
    af_model(new AutofocusModel(AF_MODEL_CONFIG_NAME))
{
}
void SWIRCtrlModel::set(const uint32_t & id)
{
    uint32_t channel_id = CHANNEL_ID_SWIR;
    switch(id)
    {
    case id_zoom:
        emit zoom_pos(uint32_t(zoom->get()), channel_id);
        break;
    case id_focus:
        emit focus_pos(uint32_t(focus->get()), channel_id);
        break;
      case id_autofocus_lens_en:
        if(_autofocus_lens_en->get())
        {
            _autofocus_host_last_state = _autofocus_host_en->get();
            _autofocus_host_en->set(false);
            emit autofocus_en(channel_id);
        }
        else
        {
            emit autofocus_dis(channel_id);
            _autofocus_host_en->set(_autofocus_host_last_state);
        }
        break;
      case id_autofocus_host_en:
        if(_autofocus_lens_en->get())
        {
            _autofocus_host_en->set(false);
        }
        else
        {
            _autofocus_host_pause = false;
        }
        break;
      default:
        break;
    }
}
void SWIRCtrlModel::get(const uint32_t &)
{
}
void SWIRCtrlModel::cmd(const uint32_t & id)
{
    uint32_t channel_id = CHANNEL_ID_SWIR;
    switch(id)
    {
    case id_thermostat_on:
        qInfo(logSWIRCtrl(), "cmd: Objective Thermostat On");
        emit thermostat_on(channel_id);
        break;
    case id_thermostat_off:
        qInfo(logSWIRCtrl(), "cmd: Objective Thermostat Off");
        emit thermostat_off(channel_id);
        break;
    case id_heat_on:
        qInfo(logSWIRCtrl(), "cmd: Objective Heating On");
        emit heat_on(channel_id);
        break;
    case id_heat_off:
        qInfo(logSWIRCtrl(), "cmd: Objective Heating Off");
        emit heat_off(channel_id);
        break;
    case id_agc:
        qInfo(logSWIRCtrl(), "cmd: AGC %s", !agc_on ? "On" : "Off");
        emit agc_switch(channel_id);
        break;
    case id_autofocus:
        qInfo(logSWIRCtrl(), "cmd: Autofocus");
        emit autofocus(channel_id);
        break;
    case id_field_sync:
        qInfo(logSWIRCtrl(), "cmd: Field Synchronization");
        emit field_sync(channel_id);
        break;
    case id_field_sync_narrow:
        qInfo(logSWIRCtrl(), "cmd: Narrow Field Synchronization");
        emit field_sync_narrow(channel_id);
        break;
    case id_field_sync_reg:
        qInfo(logSWIRCtrl(), "cmd: Regular Field Synchronization %s", !field_sync_reg_on ? "On" : "Off");
        emit field_sync_reg_switch(channel_id);
        break;
    case id_zoom_in_start:
        qInfo(logSWIRCtrl(), "cmd: Zoom In start");
        emit zoom_in(1.0, channel_id);
        break;
    case id_zoom_in_stop:
        qInfo(logSWIRCtrl(), "cmd: Zoom In stop");
        emit zoom_stop(channel_id);
        break;
    case id_zoom_out_start:
        qInfo(logSWIRCtrl(), "cmd: Zoom Out start");
        emit zoom_out(1.0, channel_id);
        break;
    case id_zoom_out_stop:
        qInfo(logSWIRCtrl(), "cmd: Zoom Out stop");
        emit zoom_stop(channel_id);
        break;
    case id_iris_in_start:
        qInfo(logSWIRCtrl(), "cmd: Iris Close start");
        emit iris_close(1.0, channel_id);
        break;
    case id_iris_in_stop:
        qInfo(logSWIRCtrl(), "cmd: Iris Close stop");
        emit iris_stop(channel_id);
        break;
    case id_iris_out_start:
        qInfo(logSWIRCtrl(), "cmd: Iris Open start");
        emit iris_open(1.0, channel_id);
        break;
    case id_iris_out_stop:
        qInfo(logSWIRCtrl(), "cmd: Iris Open stop");
        emit iris_stop(channel_id);
        break;
    case id_focus_near_start:
        qInfo(logSWIRCtrl(), "cmd: Focus Near start");
        emit focus_near(1.0, channel_id);
        break;
    case id_focus_near_stop:
        qInfo(logSWIRCtrl(), "cmd: Focus Near stop");
        emit focus_stop(channel_id);
        break;
    case id_focus_far_start:
        qInfo(logSWIRCtrl(), "cmd: Focus Far start");
        emit focus_far(1.0, channel_id);
        break;
    case id_focus_far_stop:
        qInfo(logSWIRCtrl(), "cmd: Focus Far stop");
        emit focus_stop(channel_id);
        break;
    }
}
void SWIRCtrlModel::response(Cmd)
{
}
void SWIRCtrlModel::response(Telemetry t)
{
    Telemetry::t_telemetry_struct * tel = t.get();

    sensor_status->set(tel->sw.sensor_status);
    lens_status->set(tel->sw.lens_status);
    fps->set(tel->sw.fps);
    vres->set(tel->sw.vres);
    hres->set(tel->sw.hres);
    exp->set(tel->sw.exp);
    zoom_drive->set(tel->sw.zoom_drive);
    focus_drive->set(tel->sw.focus_drive);
    lens_temp->set(tel->sw.lens_temp);
    focus_mm->set(tel->sw.focus_mm);
    field_grad->set(tel->sw.field_grad);

    agc_on = bool(tel->agc.swir_en);
    autofocus_on = bool(tel->controller.autofocus.state && (static_cast<Telemetry::camera_codes>(tel->controller.autofocus.channel) == Telemetry::sw_camera));
    field_sync_reg_on = bool(tel->controller.field_sync.state && (static_cast<Telemetry::camera_codes>(tel->controller.field_sync.channel) == Telemetry::sw_camera));

    if(agc_on) emit agc->set_success();
    else emit agc->set_neutral();
    if(autofocus_on) emit _autofocus->set_success();
    else emit _autofocus->set_neutral();
    if(field_sync_reg_on) emit field_sync_reg->set_success();
    else emit field_sync_reg->set_neutral();

    if(!_autofocus_lens_en->get() && _autofocus_host_en->get() && !_autofocus_host_pause)
    {
        int focus = af_model->get_focus(lens_temp->get(), zoom_drive->get());
        if(abs(focus_drive->get() - focus) > 3)
        {
            if(focus > 0) emit focus_pos(uint32_t(focus), CHANNEL_ID_SWIR);
        }
    }
}
void SWIRCtrlModel::set_autofocus_en_ui()
{
    _autofocus_lens_en->set(true);
    _autofocus_host_last_state = _autofocus_host_en->get();
    _autofocus_host_en->set(false);
}
void SWIRCtrlModel::set_autofocus_dis_ui()
{
    _autofocus_lens_en->set(false);
    _autofocus_host_en->set(_autofocus_host_last_state);
}
void SWIRCtrlModel::set_autofocus_host_pause(bool pause)
{
    _autofocus_host_pause = pause;
}
