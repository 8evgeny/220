#include "fusionctrlmodel.h"

#include <qmv/property/booleanvalueproperty.h>
#include <qmv/property/comboboxproperty.h>
#include <qmv/property/intvalueproperty.h>
#include <qmv/property/doublevalueproperty.h>

#include "tools/logger.h"
Q_LOGGING_CATEGORY(logFusionCtrl, "FusionCtrl")

FusionCtrlModel::FusionCtrlModel(QObject * parent, uint32_t id, QString name) :
    QMVModel(parent, id, name),
    enable(new BooleanValueProperty(this, enable_id, "enable", false)),
    method(new ComboBoxProperty(this, method_id, "method")),
    offset_x(new IntValueProperty(this, offset_x_id, "offset_x", 0, -50, 50)),
    offset_y(new IntValueProperty(this, offset_y_id, "offset_y", 0, -50, 50)),
    method_agc_en(new BooleanValueProperty(this, method_agc_en_id, "method_agc_en", false)),
    method_c(new DoubleValueProperty(this, method_c_id, "method_c", 2.0, 0.0, 50.0))
{
}
void FusionCtrlModel::update_view()
{
    QList<QString> keys;
    QList<QVariant> values;
    keys.append("Veremyova");
    keys.append("other");
    values.append(0);
    values.append(1);
    method->set_items(keys, values);

    QMVModel::update_view();
}
void FusionCtrlModel::set(const uint32_t & id)
{
    switch(id)
    {
    case enable_id:
        qInfo(logFusionCtrl(), "set: PC Fusion %s", enable->get() ? "On" : "Off");
        emit enable_changed(enable->get());
        break;
    case method_id:
        qInfo(logFusionCtrl(), "set: Set PC Fusion method %i", method->get_val().toInt());
        emit method_changed(method->get_val().toInt());
        break;
    case offset_x_id:
        qInfo(logFusionCtrl(), "set: Set PC Fusion X offset %i", offset_x->get());
        emit offset_x_changed(offset_x->get());
        break;
    case offset_y_id:
        qInfo(logFusionCtrl(), "set: Set PC Fusion Y offset %i", offset_y->get());
        emit offset_y_changed(offset_y->get());
        break;
    case method_agc_en_id:
        qInfo(logFusionCtrl(), "set: PC Fusion AGC method %s", method_agc_en->get() ? "On" : "Off");
        emit method_agc_en_changed(method_agc_en->get());
        break;
    case method_c_id:
        qInfo(logFusionCtrl(), "set: PC Fusion method coeff %f", method_c->get());
        emit method_c_changed(method_c->get());
        break;
    }
}
void FusionCtrlModel::get(const uint32_t &)
{
}
void FusionCtrlModel::cmd(const uint32_t &)
{
}
void FusionCtrlModel::update()
{
    emit enable_changed(enable->get());
    emit method_changed(method->get_val().toInt());
    emit offset_x_changed(offset_x->get());
    emit offset_y_changed(offset_y->get());
    emit method_agc_en_changed(method_agc_en->get());
    emit method_c_changed(method_c->get());
}
void FusionCtrlModel::response(Cmd)
{
}
void FusionCtrlModel::response(Telemetry)
{
}
