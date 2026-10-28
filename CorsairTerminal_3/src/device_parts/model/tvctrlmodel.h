#ifndef TVCTRLMODEL_H
#define TVCTRLMODEL_H

#include <qmv/qmvmodel.h>
class BooleanValueProperty;
class IntValueProperty;
class DoubleValueProperty;
class AutofocusModel;

#include "device_parts/cmd/telemetry.h"

class TVCtrlModel : public QMVModel
{
    Q_OBJECT
public:
    TVCtrlModel(QObject * parent = nullptr, uint32_t id = 0, QString name = "");

    enum VAL_ID {
        id_sensor_status,
        id_lens_status,
        id_fps,
        id_vres,
        id_hres,
        id_exp,
        id_zoom_drive,
        id_focus_drive,
        id_lens_temp,
        id_focus_mm,
        id_field_grad,
        id_zoom,
        id_focus,
        id_autofocus_lens_en,
        id_autofocus_host_en
    };
    enum CMD_ID {
        id_thermostat_on,
        id_thermostat_off,
        id_heat_on,
        id_heat_off,
        id_agc_colour,
        id_agc_grey,
        id_autofocus,
        id_field_sync,
        id_field_sync_narrow,
        id_field_sync_reg,
        id_zoom_in_start,
        id_zoom_in_stop,
        id_zoom_out_start,
        id_zoom_out_stop,
        id_iris_in_start,
        id_iris_in_stop,
        id_iris_out_start,
        id_iris_out_stop,
        id_focus_near_start,
        id_focus_near_stop,
        id_focus_far_start,
        id_focus_far_stop
    };
    void set(const uint32_t &);
    void get(const uint32_t &);
    void cmd(const uint32_t &);

public slots:
    void response(Cmd);
    void response(Telemetry);
    void set_autofocus_en_ui();
    void set_autofocus_dis_ui();
    void set_autofocus_host_pause(bool pause);

signals:
    void agc_switch(const uint32_t & channel_id);
    void field_sync_narrow(const uint32_t & channel_id);
    void field_sync(const uint32_t & channel_id);
    void field_sync_reg_switch(const uint32_t & channel_id);
    void autofocus(const uint32_t & channel_id);
    void autofocus_en(const uint32_t & channel_id);
    void autofocus_dis(const uint32_t & channel_id);
    void zoom_in(const double & speed, const uint32_t & channel_id);
    void zoom_out(const double & speed, const uint32_t & channel_id);
    void zoom_stop(const uint32_t & channel_id);
    void zoom_pos(const uint32_t & pos, const uint32_t & channel_id);
    void focus_near(const double & speed, const uint32_t & channel_id);
    void focus_far(const double & speed, const uint32_t & channel_id);
    void focus_stop(const uint32_t & channel_id);
    void focus_pos(const uint32_t & pos, const uint32_t & channel_id);
    void iris_close(const double & speed, const uint32_t & channel_id);
    void iris_open(const double & speed, const uint32_t & channel_id);
    void iris_stop(const uint32_t & channel_id);
    void thermostat_on(const uint32_t & channel_id);
    void thermostat_off(const uint32_t & channel_id);
    void heat_on(const uint32_t & channel_id);
    void heat_off(const uint32_t & channel_id);

private:
    IntValueProperty * sensor_status;
    IntValueProperty * lens_status;
    IntValueProperty * fps;
    IntValueProperty * vres;
    IntValueProperty * hres;
    IntValueProperty * exp;
    IntValueProperty * zoom_drive;
    IntValueProperty * focus_drive;
    IntValueProperty * lens_temp;
    DoubleValueProperty * focus_mm;
    DoubleValueProperty * field_grad;
    IntValueProperty * zoom;
    IntValueProperty * focus;
    BooleanValueProperty * _autofocus_lens_en;
    BooleanValueProperty * _autofocus_host_en;
    CmdProperty * _thermostat_on;
    CmdProperty * _thermostat_off;
    CmdProperty * _heat_on;
    CmdProperty * _heat_off;
    CmdProperty * agc_colour;
    CmdProperty * agc_grey;
    CmdProperty * _autofocus;
    CmdProperty * _field_sync;
    CmdProperty * _field_sync_narrow;
    CmdProperty * field_sync_reg;
    CmdProperty * zoom_in_start;
    CmdProperty * zoom_in_stop;
    CmdProperty * zoom_out_start;
    CmdProperty * zoom_out_stop;
    CmdProperty * iris_in_start;
    CmdProperty * iris_in_stop;
    CmdProperty * iris_out_start;
    CmdProperty * iris_out_stop;
    CmdProperty * focus_near_start;
    CmdProperty * focus_near_stop;
    CmdProperty * focus_far_start;
    CmdProperty * focus_far_stop;
    bool agc_colour_on;
    bool agc_grey_on;
    bool autofocus_on;
    bool _autofocus_host_pause;
    bool _autofocus_host_last_state;
    bool field_sync_reg_on;
    AutofocusModel * af_model;
};

#endif // TVCTRLMODEL_H
