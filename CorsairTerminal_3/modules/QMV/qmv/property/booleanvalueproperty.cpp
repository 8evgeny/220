#include "booleanvalueproperty.h"
#include <QVariant>

BooleanValueProperty::BooleanValueProperty(QMVModel * model, uint32_t id, QString name, bool def) :
    ValueProperty(model, id, name),
    val(def),
    def(def)
{
}
bool BooleanValueProperty::get() const
{
    return val;
}
bool BooleanValueProperty::set_val(const QVariant & v)
{
    val = v.toBool();
    return true;
}
QVariant BooleanValueProperty::get_val() const
{
    return val;
}
