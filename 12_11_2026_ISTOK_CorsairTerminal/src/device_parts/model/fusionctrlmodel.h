#ifndef FUSIONCTRLMODEL_H
#define FUSIONCTRLMODEL_H

#include <qmv/qmvmodel.h>

class BooleanValueProperty;
class ComboBoxProperty;
class IntValueProperty;
class DoubleValueProperty;

#include "device_parts/cmd/telemetry.h"

class FusionCtrlModel : public QMVModel
{
    Q_OBJECT
public:
    FusionCtrlModel(QObject * parent = nullptr, uint32_t id = 0, QString name = "");

    enum VAL_ID {
        enable_id,
        method_id,
        offset_x_id,
        offset_y_id,
        method_agc_en_id,
        method_c_id
    };
    enum CMD_ID {
    };

    void update_view();

    void set(const uint32_t &);
    void get(const uint32_t &);
    void cmd(const uint32_t &);

    void update();

signals:
    void enable_changed(bool);
    void method_changed(int);
    void offset_x_changed(int);
    void offset_y_changed(int);
    void method_agc_en_changed(bool);
    void method_c_changed(double);

public slots:
    void response(Cmd);
    void response(Telemetry);
private:
    BooleanValueProperty * enable;
    ComboBoxProperty * method;
    IntValueProperty * offset_x;
    IntValueProperty * offset_y;
    BooleanValueProperty * method_agc_en;
    DoubleValueProperty * method_c;
};

#endif // FUSIONCTRLMODEL_H
